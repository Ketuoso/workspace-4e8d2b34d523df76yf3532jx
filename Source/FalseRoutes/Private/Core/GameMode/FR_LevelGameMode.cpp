


#include "Core/GameMode/FR_LevelGameMode.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Controller/FR_PlayerController.h"
#include "Core/Components/FR_ObjectiveComponent.h"
#include "Core/Data/FR_ObjectiveIds.h"
#include "Core/Data/FR_ObjectiveGraphAsset.h"
#include "Core/Data/FR_ObjectiveGraphTypes.h"
#include "Core/Generation/FR_ObjectiveGraphGenerator.h"
#include "Core/Components/FR_ObjectiveSourceComponent.h"
#include "EngineUtils.h"

AFR_LevelGameMode::AFR_LevelGameMode()
{
	GameStateClass = AFR_GameState::StaticClass();
	PlayerControllerClass = AFR_PlayerController::StaticClass();
}

void AFR_LevelGameMode::BeginPlay()
{
	Super::BeginPlay();

	AFR_GameState* FRGameState = GetGameState<AFR_GameState>();
	if (!FRGameState) return;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	if (!Objectives) return;

	Objectives->OnObjectiveChangedDelegate.AddUniqueDynamic(this, &AFR_LevelGameMode::HandleObjectiveChanged);
	
	ActiveObjectiveGraph = FFR_GeneratedObjectiveGraph();
	ActiveObjectiveGraph.Seed = ObjectiveSeed;

	if (bUseProceduralObjectiveGraph)
	{
		if (InitializeProceduralObjectives()) return;
		UE_LOG(LogTemp,Error,TEXT("Procedural objectives initialization failed. Using fixed fallback objectives."));
		ActiveObjectiveGraph.bUsedFallback = true;
	}

	// Фиксированный режим или резервный вариант после ошибки генерации.
	ClearObjectiveSourceBindings();
	RegisterTestObjectives();

	ActiveObjectiveGraph.RootObjectiveId = FR_ObjectiveIds::EscapeWarehouse;
	ActiveObjectiveGraph.EscapeAvailableObjectiveId = FR_ObjectiveIds::GeneratorStart;


	// Старую диагностическую серию запускаем только в фиксированном режиме.
	if (!bUseProceduralObjectiveGraph)
	{
		TestGenerateObjectiveGraph();
	}
}

void AFR_LevelGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AFR_GameState* FRGameState = GetGameState<AFR_GameState>())
	{
		if (UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent())
		{
			Objectives->OnObjectiveChangedDelegate.RemoveDynamic(this, &AFR_LevelGameMode::HandleObjectiveChanged);
		}
	}
	Super::EndPlay(EndPlayReason);
}

bool AFR_LevelGameMode::RevealObjective(FName ObjectiveId)
{

	AFR_GameState* FRGameState = GetGameState<AFR_GameState>();
	if (!FRGameState || FRGameState->GetMatchPhase() != EFR_MatchPhase::Playing) return false;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	return Objectives && Objectives->RevealObjective(ObjectiveId);
}

bool AFR_LevelGameMode::TryAdvanceObjective(FName ObjectiveId, AActor* SourceActor)
{
	AFR_GameState* FRGameState = GetGameState<AFR_GameState>();
	if (!FRGameState || FRGameState->GetMatchPhase() != EFR_MatchPhase::Playing) return false;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	if (!Objectives) return false;
	const bool bAdvanced = Objectives->TryAdvanceObjective(ObjectiveId);
	if (bAdvanced)
	{
		UE_LOG(LogTemp,Log,TEXT("Objective %s advanced by %s"),*ObjectiveId.ToString(),SourceActor? *SourceActor->GetName(): TEXT("Unknown"));
	}

	return bAdvanced;
}

bool AFR_LevelGameMode::TryCompletedObjective(FName ObjectiveId, AActor* SourceActor)
{
	AFR_GameState* FRGameState = GetGameState<AFR_GameState>();
	if (!FRGameState || FRGameState->GetMatchPhase() != EFR_MatchPhase::Playing) return false;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	if (!Objectives) return false;
	
	const bool bCompleted = Objectives->TryCompletedObjective(ObjectiveId);

	if (bCompleted)
	{
		UE_LOG(LogTemp,Log,TEXT("Objective %s completed by %s"),*ObjectiveId.ToString(),SourceActor? *SourceActor->GetName(): TEXT("Unknown"));
	}

	return bCompleted;
}

bool AFR_LevelGameMode::RevealObjectiveWithRequirements(FName ObjectiveId)
{
	AFR_GameState* FRGameState = GetGameState<AFR_GameState>();
	if (!FRGameState || FRGameState->GetMatchPhase() != EFR_MatchPhase::Playing) return false;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	if (!Objectives) return false;

	FFR_ObjectiveDefinition Definition;
	if (!Objectives->GetObjectiveDefinition(ObjectiveId, Definition)) return false;

	bool bAnyObjectiveRevealed = Objectives->RevealObjective(ObjectiveId);

	// Раскрываем только непосредственные требования.
	// Не вызываем этот метод рекурсивно.
	for (const FName RequiredObjectiveId : Definition.RequiredObjectives)
	{
		const bool bRequirementRevealed = Objectives->RevealObjective(RequiredObjectiveId);
		bAnyObjectiveRevealed = bAnyObjectiveRevealed || bRequirementRevealed;
	}

	return bAnyObjectiveRevealed;
}

bool AFR_LevelGameMode::CanTransition(EFR_MatchPhase CurrentPhase, EFR_MatchPhase NewPhase) const
{
	if (CurrentPhase == NewPhase)
	{
		return false;
	}
	switch (CurrentPhase)
	{
	case EFR_MatchPhase::Playing:
	{
		if (NewPhase == EFR_MatchPhase::Won) return false;
		return true;
		break;
	}
	case EFR_MatchPhase::EscapeAvailable:
	{
		if (NewPhase == EFR_MatchPhase::Playing) return false;
		return true;
		break;
	}
	case EFR_MatchPhase::Won:
	{
		return false;
		break;
	}
	case EFR_MatchPhase::Lost:
	{
		return false;
		break;
	}
	default:
		break;
	}
	return false;
}

bool AFR_LevelGameMode::InitializeProceduralObjectives()
{
	if (!ObjectiveGraphAsset)
	{
		UE_LOG(LogTemp,Error,TEXT("Cannot initialize procedural objectives: ObjectiveGraphAsset is null."));
		return false;
	}

	FFR_ObjectiveGraphContext Context;
	BuildObjectiveGraphContext(Context);

	FFR_GeneratedObjectiveGraph GeneratedGraph;
	FText GenerationError;

	const bool bGenerated = UFR_ObjectiveGraphGenerator::GenerateGraph(ObjectiveGraphAsset.Get(),Context,ObjectiveSeed,GeneratedGraph,GenerationError);

	if (!bGenerated)
	{
		UE_LOG(LogTemp,Error,TEXT("Objective graph generation failed: %s"),*GenerationError.ToString());
		return false;
	}

	if (!BindGeneratedObjectivesToSources(GeneratedGraph))
	{
		UE_LOG(LogTemp,Error,TEXT("Generated objectives could not be bound to level sources."));
		return false;
	}

	if (!ApplyGeneratedObjectiveGraph(GeneratedGraph))
	{
		UE_LOG(LogTemp,Error,TEXT("Generated objective graph could not be registered."));
		return false;
	}

	ActiveObjectiveGraph = MoveTemp(GeneratedGraph);
	EscapeAvailableObjectiveId =ActiveObjectiveGraph.EscapeAvailableObjectiveId;

	UE_LOG(LogTemp,Display,TEXT("Procedural objectives initialized. EscapeAvailableObjectiveId=%s"),*EscapeAvailableObjectiveId.ToString());

	LogGeneratedObjectiveGraph(ActiveObjectiveGraph);

	return true;
}

bool AFR_LevelGameMode::ApplyGeneratedObjectiveGraph(const FFR_GeneratedObjectiveGraph& Graph)
{
	AFR_GameState* FRGameState = GetGameState<AFR_GameState>();
	if (!FRGameState) return false;

	UFR_ObjectiveComponent* Objectives =FRGameState->GetObjectiveComponent();

	if (!Objectives || Graph.Nodes.IsEmpty()) return false;

	// Метод вызывается только для нового ObjectiveComponent в BeginPlay.
	for (const FFR_GeneratedObjectiveNode& Node : Graph.Nodes)
	{
		if (!Objectives->RegisterObjective(Node.Definition))
		{
			UE_LOG(LogTemp,Error,TEXT("Failed to register generated objective: %s"),*Node.Definition.ObjectiveId.ToString());
			return false;
		}
	}

	UE_LOG(LogTemp,Display,TEXT("Registered %d procedural objectives."),Graph.Nodes.Num());
	return true;
}

void AFR_LevelGameMode::TestGenerateObjectiveGraph()
{
	if (!ObjectiveGraphAsset)
	{
		UE_LOG(LogTemp,Warning,TEXT("ObjectiveGraphAsset is not assigned. Diagnostic generation skipped."));
		return;
	}
	FFR_ObjectiveGraphContext Context;
	BuildObjectiveGraphContext(Context);

	for (int32 SeedIndex = 0;SeedIndex < DiagnosticSeedCount; ++SeedIndex)
	{
		const int32 CurrentSeed = ObjectiveSeed + SeedIndex;

		FFR_GeneratedObjectiveGraph GeneratedGraph;
		FText GenerationError;

		const bool bGenerated =UFR_ObjectiveGraphGenerator::GenerateGraph(ObjectiveGraphAsset.Get(),Context,CurrentSeed,GeneratedGraph,GenerationError);

		if (!bGenerated)
		{
			UE_LOG(LogTemp,Error,TEXT("Objective graph generation failed. Seed=%d. Error=%s"),CurrentSeed,*GenerationError.ToString());
			continue;
		}

		LogGeneratedObjectiveGraph(GeneratedGraph);
	}
}

void AFR_LevelGameMode::BuildObjectiveGraphContext(FFR_ObjectiveGraphContext& OutContext) const
{
	OutContext = FFR_ObjectiveGraphContext();

	TArray<UFR_ObjectiveSourceComponent*> Sources;
	CollectObjectiveSources(Sources);

	for (const UFR_ObjectiveSourceComponent* Source : Sources)
	{
		if (!Source) continue;
		const FName SourceSlot = Source->GetSourceSlot();

		if (SourceSlot.IsNone()) continue;
		OutContext.AvailableSourceSlots.Add(SourceSlot);
	}

	// Spawn-группы пока остаются диагностическими.
	// Их автоматический сбор подключим вместе с предметами.
	OutContext.AvailableSpawnGroups.Add(FName(TEXT("Fuel.Warehouse")),1);
	OutContext.AvailableSpawnGroups.Add(FName(TEXT("StartKey.Warehouse")),1);
	UE_LOG(LogTemp,Display,TEXT("Objective graph context: SourceSlots=%d | SpawnGroups=%d"),OutContext.AvailableSourceSlots.Num(),OutContext.AvailableSpawnGroups.Num());
}

void AFR_LevelGameMode::LogGeneratedObjectiveGraph(const FFR_GeneratedObjectiveGraph& Graph) const
{
	UE_LOG(LogTemp,Display,TEXT("ObjectiveGraph | Seed=%d | Nodes=%d | Root=%s | EscapeAvailable=%s"),
		Graph.Seed,
		Graph.Nodes.Num(),
		*Graph.RootObjectiveId.ToString(),
		*Graph.EscapeAvailableObjectiveId.ToString());

	for (const FFR_GeneratedObjectiveNode& Node : Graph.Nodes)
	{
		FString RequiredObjectives;

		for (const FName RequiredObjectiveId :
		Node.Definition.RequiredObjectives)
		{
			if (!RequiredObjectives.IsEmpty())
			{
				RequiredObjectives += TEXT(", ");
			}

			RequiredObjectives += RequiredObjectiveId.ToString();
		}

		if (RequiredObjectives.IsEmpty())
		{
			RequiredObjectives = TEXT("None");
		}

		UE_LOG(LogTemp,Display,TEXT("Node=%s | Objective=%s | Depth=%d | Mode=%d | Requires=[%s]"),
			*Node.SourceNodeId.ToString(),
			*Node.Definition.ObjectiveId.ToString(),
			Node.Depth,
			static_cast<int32>(Node.Definition.RequirementMode),
			*RequiredObjectives);
	}
}

void AFR_LevelGameMode::CollectObjectiveSources(TArray<UFR_ObjectiveSourceComponent*>& OutSources) const
{
	OutSources.Reset();

	UWorld* World = GetWorld();
	if (!World) return;

	for (TActorIterator<AActor> ActorIt(World); ActorIt; ++ActorIt)
	{
		AActor* Actor = *ActorIt;
		if (!Actor) continue;

		TArray<UFR_ObjectiveSourceComponent*> ActorSources;
		Actor->GetComponents<UFR_ObjectiveSourceComponent>(ActorSources);

		for (UFR_ObjectiveSourceComponent* Source : ActorSources)
		{
			if (!Source) continue;
			OutSources.Add(Source);
			UE_LOG(LogTemp,Verbose,TEXT("Objective source found: Actor=%s | SourceSlot=%s"),*Actor->GetName(),*Source->GetSourceSlot().ToString());
		}
	}
}

bool AFR_LevelGameMode::BindGeneratedObjectivesToSources(const FFR_GeneratedObjectiveGraph& Graph)
{
	TArray<UFR_ObjectiveSourceComponent*> Sources;
	CollectObjectiveSources(Sources);

	TMap<FName, TArray<UFR_ObjectiveSourceComponent*>> SourcesBySlot;

	for (UFR_ObjectiveSourceComponent* Source : Sources)
	{
		if (!Source)
		{
			continue;
		}

		const FName SourceSlot = Source->GetSourceSlot();

		if (SourceSlot.IsNone())
		{
			continue;
		}

		SourcesBySlot.FindOrAdd(SourceSlot).Add(Source);
	}

	// Сначала проверяем весь граф, не изменяя компоненты.
	for (const FFR_GeneratedObjectiveNode& Node : Graph.Nodes)
	{
		if (Node.SourceSlot.IsNone())
		{
			// Задача не требует привязки к источнику.
			continue;
		}

		if (!SourcesBySlot.Contains(Node.SourceSlot))
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("No objective source found. Node=%s | RequiredSourceSlot=%s"),
				*Node.SourceNodeId.ToString(),
				*Node.SourceSlot.ToString()
			);

			return false;
		}
	}

	// Только после полной проверки удаляем старые привязки.
	for (UFR_ObjectiveSourceComponent* Source : Sources)
	{
		if (Source)
		{
			Source->ClearObjectiveBindings();
		}
	}

	// Назначаем задачи всем источникам с соответствующим слотом.
	for (const FFR_GeneratedObjectiveNode& Node : Graph.Nodes)
	{
		if (Node.SourceSlot.IsNone()) continue;

		TArray<UFR_ObjectiveSourceComponent*>* MatchingSources = SourcesBySlot.Find(Node.SourceSlot);

		if (!MatchingSources) continue;

		for (UFR_ObjectiveSourceComponent* Source : *MatchingSources)
		{
			if (!Source) continue;
			const bool bBound = Source->BindObjective(Node.SourceNodeId,Node.Definition.ObjectiveId);

			if (!bBound)
			{
				UE_LOG(LogTemp,Error,TEXT("Failed to bind objective. Node=%s | Objective=%s | SourceSlot=%s"),
					*Node.SourceNodeId.ToString(),*Node.Definition.ObjectiveId.ToString(),*Node.SourceSlot.ToString());
				return false;
			}

			UE_LOG(LogTemp,Display,TEXT("Objective bound: Node=%s | Objective=%s | Actor=%s | SourceSlot=%s"),
				*Node.SourceNodeId.ToString(),*Node.Definition.ObjectiveId.ToString(),*GetNameSafe(Source->GetOwner()),*Node.SourceSlot.ToString());
		}
	}

	return true;
}

void AFR_LevelGameMode::ClearObjectiveSourceBindings()
{
	TArray<UFR_ObjectiveSourceComponent*> Sources;
	CollectObjectiveSources(Sources);

	for (UFR_ObjectiveSourceComponent* Source : Sources)
	{
		if (Source)
		{
			Source->ClearObjectiveBindings();
		}
	}
}

void AFR_LevelGameMode::RegisterTestObjectives()
{
	AFR_GameState* FRGameState =GetGameState<AFR_GameState>();
	if (!FRGameState) return;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	if (!Objectives) return;

	EscapeAvailableObjectiveId = FR_ObjectiveIds::GeneratorStart;

	// Главная задача.
	FFR_ObjectiveDefinition EscapeDefinition;
	EscapeDefinition.ObjectiveId = FR_ObjectiveIds::EscapeWarehouse;
	EscapeDefinition.DisplayText = FText::FromString(TEXT("Сбегите со склада"));
	EscapeDefinition.RequiredAmount = 1;
	EscapeDefinition.DisplayOrder = 0;
	EscapeDefinition.bMainObjective = true;
	EscapeDefinition.bVisibleFromStart = true;

	Objectives->RegisterObjective(EscapeDefinition);

	// Ремонт генератора.
	FFR_ObjectiveDefinition RepairDefinition;
	RepairDefinition.ObjectiveId = FR_ObjectiveIds::GeneratorRepair;
	RepairDefinition.DisplayText = FText::FromString(TEXT("Отремонтируйте генератор"));
	RepairDefinition.RequiredAmount = 3;
	RepairDefinition.DisplayOrder = 10;
	RepairDefinition.bVisibleFromStart = false;

	Objectives->RegisterObjective(RepairDefinition);

	// Запуск генератора.
	FFR_ObjectiveDefinition StartDefinition;
	StartDefinition.ObjectiveId =FR_ObjectiveIds::GeneratorStart;
	StartDefinition.DisplayText = FText::FromString(TEXT("Запустите генератор"));
	StartDefinition.RequiredAmount = 1;
	StartDefinition.DisplayOrder = 20;
	StartDefinition.bVisibleFromStart = false;
	StartDefinition.RequiredObjectives.Add(FR_ObjectiveIds::GeneratorRepair);
	StartDefinition.RequirementMode = EFR_ObjectiveRequirementMode::All;

	Objectives->RegisterObjective(StartDefinition);
}

void AFR_LevelGameMode::HandleObjectiveChanged(FName ObjectiveId)
{
	if (ObjectiveId != EscapeAvailableObjectiveId) return;

	AFR_GameState* FRGameState = GetGameState<AFR_GameState>();
	if (!FRGameState) return;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();

	if (Objectives && Objectives->IsObjectiveCompleted(EscapeAvailableObjectiveId))
	{
		RequestEscapeAvailable();
	}
	
}

bool AFR_LevelGameMode::TryChangeMatchPhase(EFR_MatchPhase NewPhase)
{
	AFR_GameState* FR_GameState = GetGameState<AFR_GameState>();
	if (!IsValid(FR_GameState)) return false;
	const EFR_MatchPhase CurrentPhase = FR_GameState->GetMatchPhase();
	const bool bCanTransition = CanTransition(CurrentPhase, NewPhase);
	if (!bCanTransition)
	{
		return false;
	}
	FR_GameState->InternalSetMatchPhase(NewPhase);
	return true;
}

void AFR_LevelGameMode::RequestEscapeAvailable()
{
	UE_LOG(LogTemp, Warning, TEXT("Переход в фазу EscapeAvailable"));
	bool bSuccesses = TryChangeMatchPhase(EFR_MatchPhase::EscapeAvailable);
	if (!bSuccesses)
	{
		UE_LOG(LogTemp, Warning, TEXT("Переход в фазу EscapeAvailableне удалось"));
	}
}

void AFR_LevelGameMode::RequestWin()
{
	UE_LOG(LogTemp, Warning, TEXT("Переход в фазу Won"));
	bool bSuccesses = TryChangeMatchPhase(EFR_MatchPhase::Won);
	if (!bSuccesses)
	{
		UE_LOG(LogTemp, Warning, TEXT("Переход в фазу Won не удалось"));
	}
}

void AFR_LevelGameMode::RequestLose()
{
	UE_LOG(LogTemp, Warning, TEXT("Переход в фазу Lost"));
	bool bSuccesses = TryChangeMatchPhase(EFR_MatchPhase::Lost);
	if (!bSuccesses)
	{
		UE_LOG(LogTemp, Warning, TEXT("Переход в фазу Lost не удалось"));
	}
}

bool AFR_LevelGameMode::IsProceduralObjectiveGraphActive() const
{
	return bUseProceduralObjectiveGraph &&!ActiveObjectiveGraph.bUsedFallback && !ActiveObjectiveGraph.Nodes.IsEmpty();
}
