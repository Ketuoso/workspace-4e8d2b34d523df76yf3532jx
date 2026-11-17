


#include "World/Actor/FR_Generator.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Character/FR_CharacterBase.h"
#include "Core/GameMode/FR_LevelGameMode.h"


AFR_Generator::AFR_Generator()
{

	PrimaryActorTick.bCanEverTick = false;

}

void AFR_Generator::Interact_Implementation(AFR_CharacterBase* Interactor)
{
	// В будущем тут можно добавить проверку наличия ключей и выводить сообщение если их нет.
	if (!Interactor) return;
	if (GeneratorState == EFR_GeneratorState::Ready)
	{
		StartRepair();
	}
	else
	{
		CanceledRepair();
	}

}

bool AFR_Generator::CanInteract_Implementation(AFR_CharacterBase* Interactor)
{
	//Проверка можно ли взаимодействовать с Актром
	if (GeneratorState == EFR_GeneratorState::Completed) return false;
	AFR_GameState* FR_GameState = GetWorld()->GetGameState<AFR_GameState>();
	if (!FR_GameState) return false;
	//Дополнительная проверка у Актора что матч находится в стадии Playing
	if (FR_GameState->GetMatchPhase() != EFR_MatchPhase::Playing) return false;
	if (!Interactor->IsLocallyControlled()) return false;
	return true;
}





void AFR_Generator::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFR_Generator::StartRepair()
{
	GeneratorState = EFR_GeneratorState::Repairing;
	if (RepairDuration > 0.f)
	{
		GetWorldTimerManager().SetTimer(RepairTimerHandle, this, &AFR_Generator::CompletedRepair, RepairDuration, false);
	}
	if (RepairDuration == 0.f)
	{
		CompletedRepair();
	}
}

void AFR_Generator::CompletedRepair()
{
	GetWorldTimerManager().ClearTimer(RepairTimerHandle);
	AFR_GameState* FR_GameState = GetWorld()->GetGameState<AFR_GameState>();
	if (!FR_GameState) return;
	//Дополнительная проверка у Актора что матч находится в стадии Playing
	if (FR_GameState->GetMatchPhase() != EFR_MatchPhase::Playing) return;
	if (GeneratorState != EFR_GeneratorState::Repairing) return;
	if (RepairCount >= RequiredRepairs) return;

	RepairCount = FMath::Min(RepairCount + 1, RequiredRepairs);
	if (RepairCount >= RequiredRepairs)
	{
		GeneratorState = EFR_GeneratorState::Completed;
		AFR_LevelGameMode* FR_LevelGameMode = GetWorld()->GetAuthGameMode<AFR_LevelGameMode>();
		FR_LevelGameMode->RequestEscapeAvailable();
	}
	else
	{
		GeneratorState = EFR_GeneratorState::Ready;
	}
}

void AFR_Generator::CanceledRepair()
{
	if (GeneratorState == EFR_GeneratorState::Repairing)
	{
		GetWorldTimerManager().ClearTimer(RepairTimerHandle);
		GeneratorState = EFR_GeneratorState::Ready;
	}
}


