


#include "Core/Generation/FR_ObjectiveGraphGenerator.h"

#include "Core/Data/FR_ObjectiveGraphAsset.h"
#include "Core/Data/FR_ObjectiveGraphTypes.h"
#include "Math/RandomStream.h"


namespace FR_ObjectiveGraphGeneratorInternal
{
    struct FBuildState
    {
        explicit FBuildState(int32 InSeed) : Random(InSeed)
        {
        }

        const UFR_ObjectiveGraphAsset* Asset = nullptr;
        const FFR_ObjectiveGraphContext* Context = nullptr;

        FRandomStream Random;

        TMap<FName, const FFR_ObjectiveNodeTemplate*> TemplatesByNodeId;
        TSet<FName> VisitingNodeIds;

        TMap<FName, int32> GeneratedNodeIndices;
        TMap<FName, int32> UsedSpawnGroups;

        FFR_GeneratedObjectiveGraph* OutGraph = nullptr;
    };

    bool Fail(FText& OutError, const FString& Message)
    {
        OutError = FText::FromString(Message);
        return false;
    }

    bool BuildTemplateLookup(FBuildState& State, FText& OutError)
    {
        for (const auto& NodeTemplate : State.Asset->NodeTemplates)
        {
            if (NodeTemplate.NodeId.IsNone())
            {
                return Fail(OutError, TEXT("Objective graph contains an empty NodeId."));
            }
            if (State.TemplatesByNodeId.Contains(NodeTemplate.NodeId))
            {
                return Fail(OutError, FString::Printf(TEXT("Duplicate NodeId: %s"), *NodeTemplate.NodeId.ToString()));
            }
            State.TemplatesByNodeId.Add(NodeTemplate.NodeId, &NodeTemplate);
        }
        return true;
    }

    bool ValidateAsset(const FBuildState& State, FText& OutError)
    {
        const UFR_ObjectiveGraphAsset& Asset = *State.Asset;

        if (Asset.NodeTemplates.IsEmpty())
        {
            return Fail(OutError, TEXT("NodeTemplates is empty."));
        }

        if (Asset.RootNodeId.IsNone())
        {
            return Fail(OutError, TEXT("RootNodeId is not set."));
        }

        if (Asset.EscapeAvailableNodeId.IsNone())
        {
            return Fail(OutError, TEXT("EscapeAvailableNodeId is not set."));
        }

        if (!State.TemplatesByNodeId.Contains(Asset.RootNodeId))
        {
            return Fail(OutError,FString::Printf(TEXT("Root node was not found: %s"), *Asset.RootNodeId.ToString()));
        }
        if (!State.TemplatesByNodeId.Contains(Asset.EscapeAvailableNodeId))
        {
            return Fail(OutError, FString::Printf(TEXT("EscapeAvailable node was not found: %s"), *Asset.EscapeAvailableNodeId.ToString()));
        }
        if (Asset.MaxDepth < 1 || Asset.MaxGeneratedObjectives < 1 || Asset.MaxGenerationAttempts < 1)
        {
            return Fail(OutError, TEXT("Generation limits must be greater than zero."));
        }
        
        TSet<FName> UniqueObjectiveIds;

        for (const auto& NodeTemplate : Asset.NodeTemplates)
        {
            if (NodeTemplate.ObjectiveId.IsNone())
            {
                return Fail(OutError, FString::Printf(TEXT("Node %s has an empty ObjectiveId."), *NodeTemplate.NodeId.ToString()));
            }
            if (UniqueObjectiveIds.Contains(NodeTemplate.ObjectiveId))
            {
                return Fail(OutError, FString::Printf(TEXT("Duplicate ObjectiveId: %s"), *NodeTemplate.ObjectiveId.ToString()));
            }
            
            UniqueObjectiveIds.Add(NodeTemplate.ObjectiveId);
            
            if (NodeTemplate.RequiredAmount < 1)
            {
                return Fail(OutError, FString::Printf(TEXT("Node %s has RequiredAmount less than 1."), *NodeTemplate.NodeId.ToString()));
            }

            for (const auto& Variant : NodeTemplate.RequirementVariants)
            {
                for (const FName RequiredNodeId : Variant.RequiredNodeIds)
                {
                    if (RequiredNodeId.IsNone())
                    {
                        return Fail(OutError, FString::Printf(TEXT("Node %s contains an empty RequiredNodeId."), *NodeTemplate.NodeId.ToString()));
                    }
                    if (!State.TemplatesByNodeId.Contains(RequiredNodeId))
                    {
                        return Fail(OutError, FString::Printf(TEXT("Node %s references unknown node %s."), *NodeTemplate.NodeId.ToString(), *RequiredNodeId.ToString()));
                    }
                }
            }
        }
        return true;
    }

    bool SelectRequirementVariant(const FFR_ObjectiveNodeTemplate& NodeTemplate, FRandomStream& Random, const FFR_ObjectiveRequirementVariant*& OutVariant, FText& OutError)
    {
        OutVariant = nullptr;
        float TotalWeight = 0.0f;
        const FFR_ObjectiveRequirementVariant* LastValidVariant = nullptr;

        for (const auto& Variant : NodeTemplate.RequirementVariants)
        {
            if (Variant.Weight <= 0.0f)
            {
                continue;
            }

            TotalWeight += Variant.Weight;
            LastValidVariant = &Variant;
        }

        if (TotalWeight <= 0.0f || LastValidVariant == nullptr)
        {
            return Fail(OutError, FString::Printf(TEXT("Node %s has no variants with a positive weight."), *NodeTemplate.NodeId.ToString()));
        }

        const float RandomValue = Random.FRandRange(0.0f, TotalWeight);
        float AccumulatedWeight = 0.0f;

        for (const auto& Variant : NodeTemplate.RequirementVariants)
        {
            if (Variant.Weight <= 0.0f)
            {
                continue;
            }

            AccumulatedWeight += Variant.Weight;

            if (RandomValue <= AccumulatedWeight)
            {
                OutVariant = &Variant;
                return true;
            }
        }
        OutVariant = LastValidVariant;
        return true;
    }

    bool TryBuildNode(FName NodeId, int32 Depth, FBuildState& State, FText& OutError)
    {
        if (State.VisitingNodeIds.Contains(NodeId))
        {
            return Fail(OutError,FString::Printf(TEXT("Dependency cycle detected at node %s."),*NodeId.ToString()));
        }
        if (State.GeneratedNodeIndices.Contains(NodeId))
        {
            return true;
        }
        if (Depth > State.Asset->MaxDepth)
        {
            return Fail(OutError, FString::Printf(TEXT("MaxDepth exceeded at node %s."), *NodeId.ToString()));
        }

        const FFR_ObjectiveNodeTemplate* NodeTemplate = State.TemplatesByNodeId.FindRef(NodeId);

        if (NodeTemplate == nullptr)
        {
            return Fail(OutError, FString::Printf(TEXT("Node template was not found: %s"), *NodeId.ToString()));
        }
        if (State.OutGraph->Nodes.Num() >= State.Asset->MaxGeneratedObjectives)
        {
            return Fail(OutError, TEXT("MaxGeneratedObjectives exceeded."));
        }
        if (!NodeTemplate->SourceSlot.IsNone() && !State.Context->AvailableSourceSlots.Contains(NodeTemplate->SourceSlot))
        {
            return Fail(OutError, FString::Printf(TEXT("SourceSlot %s is not available for node %s."), *NodeTemplate->SourceSlot.ToString(), *NodeId.ToString()));
        }
        if (!NodeTemplate->SpawnGroup.IsNone())
        {
            const int32* AvailableCount = State.Context->AvailableSpawnGroups.Find(NodeTemplate->SpawnGroup);

            if (AvailableCount == nullptr || *AvailableCount <= 0)
            {
                return Fail(OutError, FString::Printf(TEXT("SpawnGroup %s is not available."), *NodeTemplate->SpawnGroup.ToString()));
            }
            
            const int32 UsedCount = State.UsedSpawnGroups.FindRef(NodeTemplate->SpawnGroup);

            if (UsedCount >= *AvailableCount)
            {
                return Fail(OutError, FString::Printf(TEXT("SpawnGroup %s has no free entries."), *NodeTemplate->SpawnGroup.ToString()));
            }

            State.UsedSpawnGroups.FindOrAdd(NodeTemplate->SpawnGroup) += 1;
        }

        const FFR_ObjectiveRequirementVariant* SelectedVariant = nullptr;

        if (!NodeTemplate->RequirementVariants.IsEmpty())
        {
            if (!SelectRequirementVariant(*NodeTemplate, State.Random, SelectedVariant, OutError))
            {
                return false;
            }
        }

        State.VisitingNodeIds.Add(NodeId);

        if (SelectedVariant != nullptr)
        {
            for (const auto& RequiredNodeId : SelectedVariant->RequiredNodeIds)
            {
                if (!TryBuildNode(RequiredNodeId, Depth + 1, State, OutError))
                {
                    return false;
                }
            }
        }

        FFR_GeneratedObjectiveNode NewNode;
        
        NewNode.SourceNodeId = NodeTemplate->NodeId;
        NewNode.Depth = Depth;
        NewNode.bInitiallySatisfied = false;
        NewNode.SourceSlot = NodeTemplate->SourceSlot;
        NewNode.SpawnGroup = NodeTemplate->SpawnGroup;

        NewNode.Definition.ObjectiveId = NodeTemplate->ObjectiveId;
        NewNode.Definition.DisplayText = NodeTemplate->DisplayText;
        NewNode.Definition.RequiredAmount = NodeTemplate->RequiredAmount;
        NewNode.Definition.DisplayOrder = NodeTemplate->DisplayOrder;
        NewNode.Definition.bMainObjective = NodeTemplate->bMainObjective;
        NewNode.Definition.bVisibleFromStart = NodeTemplate->bVisibleFromStart;
        NewNode.Definition.bRevealWithParent = NodeTemplate->bRevealWithParent;

        const bool bCanBeInitiallySatisfied = NodeTemplate->bCanBeInitiallySatisfied &&  NodeTemplate->NodeId != State.Asset->RootNodeId && NodeTemplate->NodeId != State.Asset->EscapeAvailableNodeId;

        if (bCanBeInitiallySatisfied && State.Asset->InitialSatisfactionChanceCurve)
        {
            const float MaxChance = FMath::Clamp(State.Asset->MaxInitialSatisfactionChance,0.f,1.f);
            const float InitialChance = FMath::Clamp(State.Asset->InitialSatisfactionChanceCurve->GetFloatValue(static_cast<float>(Depth)),0.f,MaxChance);
            NewNode.bInitiallySatisfied = State.Random.FRand() < InitialChance;
        }

        if (SelectedVariant != nullptr)
        {
            NewNode.Definition.RequirementMode = SelectedVariant->RequirementMode;
            for (const FName RequiredNodeId : SelectedVariant->RequiredNodeIds)
            {
                const FFR_ObjectiveNodeTemplate* RequiredTemplate = State.TemplatesByNodeId.FindRef(RequiredNodeId);
                if (RequiredTemplate == nullptr)
                {
                    return Fail(OutError, FString::Printf(TEXT("Required template was not found: %s"), *RequiredNodeId.ToString()));
                }

                NewNode.Definition.RequiredObjectives.Add(RequiredTemplate->ObjectiveId);
            }
        }
        else
        {
            NewNode.Definition.RequirementMode = EFR_ObjectiveRequirementMode::All;
        }

        const int32 NewNodeIndex = State.OutGraph->Nodes.Add(MoveTemp(NewNode));
        State.GeneratedNodeIndices.Add(NodeId, NewNodeIndex);
        State.VisitingNodeIds.Remove(NodeId);
        return true;
    }

    bool RecalculateNodeDepth(FFR_GeneratedObjectiveGraph& Graph, FText& OutError)
    {
        TMap<FName, int32> NodeIndicesByObjectiveId;

        for (int32 Index = 0; Index < Graph.Nodes.Num(); ++Index)
        {
            NodeIndicesByObjectiveId.Add(Graph.Nodes[Index].Definition.ObjectiveId, Index);
        }

        if (!NodeIndicesByObjectiveId.Contains(Graph.RootObjectiveId))
        {
            return Fail(OutError,TEXT("Cannot calculate depths: root objective is missing."));
        }

        TMap<FName, int32> DepthsByObjectiveId;
        TArray<FName> Queue;
        DepthsByObjectiveId.Add(Graph.RootObjectiveId, 0);
        Queue.Add(Graph.RootObjectiveId);
        int32 QueueHead = 0;

        while (Queue.IsValidIndex(QueueHead))
        {
            const FName CurrentObjectiveId = Queue[QueueHead++];
            const int32 CurrentDepth = DepthsByObjectiveId.FindRef(CurrentObjectiveId);
            const int32* CurrentNodeIndex = NodeIndicesByObjectiveId.Find(CurrentObjectiveId);
            
            if (CurrentNodeIndex == nullptr)
            {
                return Fail(OutError, TEXT("Depth calculation encountered an unknown objective."));
            }

            const FFR_GeneratedObjectiveNode& CurrentNode = Graph.Nodes[*CurrentNodeIndex];

            for (const FName RequiredObjectiveId : CurrentNode.Definition.RequiredObjectives)
            {
                if (!NodeIndicesByObjectiveId.Contains(RequiredObjectiveId))
                {
                    return Fail(OutError, FString::Printf(TEXT("Missing generated dependency: %s"), *RequiredObjectiveId.ToString()));
                }
                if (!DepthsByObjectiveId.Contains(RequiredObjectiveId))
                {
                    DepthsByObjectiveId.Add(RequiredObjectiveId, CurrentDepth + 1);
                    Queue.Add(RequiredObjectiveId);
                }
            }
        }

        for (FFR_GeneratedObjectiveNode& Node : Graph.Nodes)
        {
            const int32* Depth = DepthsByObjectiveId.Find(Node.Definition.ObjectiveId);
            if (Depth == nullptr)
            {
                return Fail(OutError, FString::Printf(TEXT("Generated node is not reachable from root: %s"), *Node.Definition.ObjectiveId.ToString()));
            }
            Node.Depth = *Depth;
        }
        return true;
    }

    bool HasDependencyCycle(FName ObjectiveId, const TMap<FName, const FFR_ObjectiveDefinition*>& Definitions, TSet<FName>& Visiting, TSet<FName>& Visited)
    {
        if (Visited.Contains(ObjectiveId)) return false;
        if (Visiting.Contains(ObjectiveId)) return true;

        const FFR_ObjectiveDefinition* Definition = Definitions.FindRef(ObjectiveId);
        if (Definition == nullptr) return false;

        Visiting.Add(ObjectiveId);

        for (const FName RequiredObjectiveId : Definition->RequiredObjectives)
        {
            if (Definitions.Contains(RequiredObjectiveId) && HasDependencyCycle(RequiredObjectiveId, Definitions, Visiting, Visited)) return true;
        }

        Visiting.Remove(ObjectiveId);
        Visited.Add(ObjectiveId);
        return false;
    }

    bool ValidateGeneratedGraph(const FFR_GeneratedObjectiveGraph& Graph,const FFR_ObjectiveGraphContext& Context,const UFR_ObjectiveGraphAsset& Asset,FText& OutError)
    {
        if (Graph.Nodes.IsEmpty())
        {
            return Fail(OutError, TEXT("Generated graph is empty."));
        }
        if (Graph.Nodes.Num() > Asset.MaxGeneratedObjectives)
        {
            return Fail(OutError,TEXT("Generated graph exceeds MaxGeneratedObjectives."));
        }

        TMap<FName, const FFR_ObjectiveDefinition*> Definitions;

        for (const auto& Node : Graph.Nodes)
        {
            const FFR_ObjectiveDefinition& Definition = Node.Definition;
            if (Definition.ObjectiveId.IsNone() || Definitions.Contains(Definition.ObjectiveId))
            {
                return Fail(OutError, TEXT("Generated graph contains an empty or duplicate ObjectiveId."));
            }
            if (Node.Depth <0 || Node.Depth > Asset.MaxDepth)
            {
                return Fail(OutError, FString::Printf(TEXT("Invalid depth for objective %s."), *Definition.ObjectiveId.ToString()));
            }
            if (Definition.RequiredAmount < 1)
            {
                return Fail(OutError, TEXT("Generated objective has RequiredAmount less than 1."));
            }
            if (!Node.SourceSlot.IsNone() && !Context.AvailableSourceSlots.Contains(Node.SourceSlot))
            {
                return Fail(OutError,TEXT("Generated graph contains an unavailable SourceSlot."));
            }
            if (!Node.SpawnGroup.IsNone())
            {
                const int32* Count = Context.AvailableSpawnGroups.Find(Node.SpawnGroup);
                if (Count == nullptr || *Count <= 0)
                {
                    return Fail(OutError,TEXT("Generated graph contains an unavailable SpawnGroup."));
                }
            }
            Definitions.Add(Definition.ObjectiveId, &Definition);
        }
        if (!Definitions.Contains(Graph.RootObjectiveId) || !Definitions.Contains(Graph.EscapeAvailableObjectiveId))
        {
            return Fail(OutError,TEXT("Generated graph is missing root or escape objective."));
        }

        for (const auto& Node : Graph.Nodes)
        {
            for (const FName RequiredObjectiveId : Node.Definition.RequiredObjectives)
            {
                if (!Definitions.Contains(RequiredObjectiveId))
                {
                    return Fail(OutError, FString::Printf(TEXT("Generated dependency is missing: %s"), *RequiredObjectiveId.ToString()));
                }
            }
        }

        TSet<FName> Visiting;
        TSet<FName> Visited;

        for (const auto& Pair : Definitions)
        {
            if (HasDependencyCycle(Pair.Key, Definitions, Visiting, Visited))
            {
                return Fail(OutError, FString::Printf(TEXT("Generated graph contains a cycle at %s."), *Pair.Key.ToString()));
            }
        }
        return true;
    }
}

bool UFR_ObjectiveGraphGenerator::GenerateGraph(const UFR_ObjectiveGraphAsset* GraphAsset, const FFR_ObjectiveGraphContext& Context, int32 Seed, FFR_GeneratedObjectiveGraph& OutGraph, FText& OutError)
{
    using namespace FR_ObjectiveGraphGeneratorInternal;

    OutGraph = FFR_GeneratedObjectiveGraph();
    OutGraph.Seed = Seed;
    OutGraph.bUsedFallback = false;
    OutError = FText::GetEmpty();

    if (GraphAsset == nullptr)
    {
        return Fail(OutError, TEXT("GraphAsset is null."));
    }

    FBuildState State(Seed);
    State.Asset = GraphAsset;
    State.Context = &Context;

    if (!BuildTemplateLookup(State, OutError) || !ValidateAsset(State, OutError)) return false;

    FText LastError = FText::FromString(TEXT("Unknown generation error."));

    for (int32 Attempt = 0; Attempt < GraphAsset->MaxGenerationAttempts; ++Attempt)
    {
        FFR_GeneratedObjectiveGraph Candidate;
        Candidate.Seed = Seed;
        Candidate.bUsedFallback = false;

        State.OutGraph = &Candidate;
        State.VisitingNodeIds.Empty();
        State.GeneratedNodeIndices.Empty();
        State.UsedSpawnGroups.Empty();

        FText AttemptError;

        if (!TryBuildNode(GraphAsset->RootNodeId, 0, State, AttemptError))
        {
            LastError = AttemptError;
            continue;
        }

        const FFR_ObjectiveNodeTemplate* RootTemplate = State.TemplatesByNodeId.FindRef(GraphAsset->RootNodeId);
        const FFR_ObjectiveNodeTemplate* EscapeTemplate = State.TemplatesByNodeId.FindRef(GraphAsset->EscapeAvailableNodeId);

        Candidate.RootObjectiveId = RootTemplate->ObjectiveId;
        Candidate.EscapeAvailableObjectiveId = EscapeTemplate->ObjectiveId;

        if (!RecalculateNodeDepth(Candidate, AttemptError) || !ValidateGeneratedGraph(Candidate, Context, *GraphAsset, AttemptError))
        {
            LastError = AttemptError;
            continue;
        }

        OutGraph = MoveTemp(Candidate);
        OutError = FText::GetEmpty();
        return true;
    }

    OutGraph = FFR_GeneratedObjectiveGraph();
    OutGraph.Seed = Seed;
    OutGraph.bUsedFallback = false;
    OutError = FText::FromString(FString::Printf(TEXT("Graph generation failed after %d attempts. Last error: %s"),GraphAsset->MaxGenerationAttempts,*LastError.ToString()));

    return false;
}
