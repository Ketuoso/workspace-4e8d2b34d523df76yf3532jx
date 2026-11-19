#pragma once

#include "CoreMinimal.h"
#include "Core/Data/FR_ObjectiveTypes.h"
#include "FR_ObjectiveGraphTypes.generated.h"


USTRUCT(BlueprintType)
struct FFR_ObjectiveRequirementVariant
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName VariantId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float Weight = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EFR_ObjectiveRequirementMode RequirementMode = EFR_ObjectiveRequirementMode::All;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FName> RequiredNodeIds;
};

USTRUCT(BlueprintType)
struct FFR_ObjectiveNodeTemplate
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName NodeId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName ObjectiveId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText DisplayText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 RequiredAmount = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 DisplayOrder = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bMainObjective = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bVisibleFromStart = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bCanBeInitiallySatisfied = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bRevealWithParent = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bAutoCompleteWhenRequirementsMet = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName SourceSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName SpawnGroup;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FFR_ObjectiveRequirementVariant> RequirementVariants;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FName> DiscoveryRequiredNodeIds;
};

USTRUCT(BlueprintType)
struct FFR_GeneratedObjectiveNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName SourceNodeId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FFR_ObjectiveDefinition Definition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 Depth = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bInitiallySatisfied = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName SourceSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName SpawnGroup;
};

USTRUCT(BlueprintType)
struct FFR_GeneratedObjectiveGraph
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 Seed = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TArray<FFR_GeneratedObjectiveNode> Nodes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName RootObjectiveId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FName EscapeAvailableObjectiveId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bUsedFallback = false;
};

USTRUCT(BlueprintType)
struct FFR_ObjectiveGraphContext
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSet<FName> AvailableSourceSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TMap<FName, int32> AvailableSpawnGroups;
};