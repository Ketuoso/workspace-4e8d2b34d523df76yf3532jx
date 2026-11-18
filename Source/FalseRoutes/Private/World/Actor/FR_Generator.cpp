


#include "World/Actor/FR_Generator.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Character/FR_CharacterBase.h"
#include "Core/GameMode/FR_LevelGameMode.h"
#include "Core/Components/FR_ObjectiveComponent.h"
#include "Core/Data/FR_ObjectiveIds.h"


AFR_Generator::AFR_Generator()
{

	PrimaryActorTick.bCanEverTick = false;

}

void AFR_Generator::Interact_Implementation(AFR_CharacterBase* Interactor)
{
	// В будущем тут можно добавить проверку наличия ключей и выводить сообщение если их нет.
	if (!CanInteract_Implementation(Interactor)) return;
	UWorld* World = GetWorld();
	if (!World) return;
	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	if (!GameMode) return;

	GameMode->RevealObjective(FR_ObjectiveIds::GeneratorRepair);
	GameMode->RevealObjective(FR_ObjectiveIds::GeneratorStart);

	StartRepair(Interactor);

}

bool AFR_Generator::CanInteract_Implementation(AFR_CharacterBase* Interactor)
{
	if (!IsValid(Interactor) || !Interactor->IsLocallyControlled()) return false;
	if (GeneratorState != EFR_GeneratorState::Ready) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	AFR_GameState* FR_GameState = GetWorld()->GetGameState<AFR_GameState>();
	if (!FR_GameState || FR_GameState->GetMatchPhase() != EFR_MatchPhase::Playing) return false;

	UFR_ObjectiveComponent* Objectives = FR_GameState->GetObjectiveComponent();

	return Objectives && !Objectives->IsObjectiveCompleted(FR_ObjectiveIds::GeneratorRepair);
}





void AFR_Generator::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFR_Generator::StartRepair(AFR_CharacterBase* Interactor)
{
	GeneratorState = EFR_GeneratorState::Repairing;
	RepairInstigator = Interactor;
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

	if (GeneratorState != EFR_GeneratorState::Repairing) return;

	UWorld* World = GetWorld();
	if (!World) return;

	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	AFR_GameState* FR_GameState = GetWorld()->GetGameState<AFR_GameState>();
	if (!FR_GameState || !GameMode) return;

	UFR_ObjectiveComponent* Objectives = FR_GameState->GetObjectiveComponent();
	if (!Objectives) return;
	
	AFR_CharacterBase* SourceActor = RepairInstigator.Get();
	const bool bAdvanced = GameMode->TryAdvanceObjective(FR_ObjectiveIds::GeneratorRepair, SourceActor);
	if (!bAdvanced)
	{
		GeneratorState = EFR_GeneratorState::Ready;
		RepairInstigator = nullptr;
		return;
	}
	GeneratorState = EFR_GeneratorState::Ready;
	// Здесь можно запускать эффекты успешного ремонта.

	if (Objectives->IsObjectiveCompleted(FR_ObjectiveIds::GeneratorRepair))
	{
		GeneratorState = EFR_GeneratorState::Completed;

		GameMode->TryCompletedObjective(FR_ObjectiveIds::GeneratorStart, SourceActor);
	}
	RepairInstigator = nullptr;
}

void AFR_Generator::CanceledRepair()
{
	if (GeneratorState == EFR_GeneratorState::Repairing)
	{
		GetWorldTimerManager().ClearTimer(RepairTimerHandle);
		GeneratorState = EFR_GeneratorState::Ready;
	}
}


