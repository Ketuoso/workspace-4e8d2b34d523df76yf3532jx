#pragma once


#include "CoreMinimal.h"
#include "FR_ObjectiveTypes.generated.h"

UENUM(BlueprintType)
enum class EFR_ObjectiveStatus : uint8
{
	Hidden, //Скрыт
	Available, // Доступен
	InProgress, //В процессе
	Completed // Завершен
};

UENUM(BlueprintType)
enum class EFR_ObjectiveRequirementMode : uint8
{
	All, // Все обязательные задачи
	Any  // Достаточно одной
};

USTRUCT(BlueprintType)
struct FFR_ObjectiveDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ObjectiveId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredAmount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DisplayOrder = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bMainObjective = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bVisibleFromStart = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRevealWithParent = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoCompleteWhenRequirementsMet = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFR_ObjectiveRequirementMode RequirementMode = EFR_ObjectiveRequirementMode::All;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FName> RequiredObjectives;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FName> DiscoveryRequiredObjectives;
};

USTRUCT(BlueprintType)
struct FFR_ObjectiveRuntimeState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	EFR_ObjectiveStatus Status = EFR_ObjectiveStatus::Hidden;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	int32 CurrentAmount = 0;

	// Отдельно от статуса: задача может быть уже выполнена,
	// но ещё не обнаружена игроком.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	bool bIsDiscovered = false;

	// Для этапов 2–3.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	FString CompletedBy;
};