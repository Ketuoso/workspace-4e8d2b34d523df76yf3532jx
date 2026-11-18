


#include "Core/GameMode/FR_LevelGameMode.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Controller/FR_PlayerController.h"
#include "Core/Components/FR_ObjectiveComponent.h"
#include "Core/Data/FR_ObjectiveIds.h"

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
	RegisterTestObjectives();
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
		UE_LOG(
			LogTemp,
			Log,
			TEXT("Objective %s advanced by %s"),
			*ObjectiveId.ToString(),
			SourceActor
			? *SourceActor->GetName()
			: TEXT("Unknown")
		);
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
		UE_LOG(
			LogTemp,
			Log,
			TEXT("Objective %s completed by %s"),
			*ObjectiveId.ToString(),
			SourceActor
			? *SourceActor->GetName()
			: TEXT("Unknown")
		);
	}

	return bCompleted;
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

void AFR_LevelGameMode::RegisterTestObjectives()
{
	AFR_GameState* FRGameState =GetGameState<AFR_GameState>();
	if (!FRGameState) return;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	if (!Objectives) return;

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
