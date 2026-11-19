


#include "World/Actor/FR_Generator.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Character/FR_CharacterBase.h"
#include "Core/GameMode/FR_LevelGameMode.h"
#include "Core/Components/FR_ObjectiveComponent.h"
#include "Core/Data/FR_ObjectiveIds.h"
#include "Core/Components/FR_ObjectiveSourceComponent.h"

AFR_Generator::AFR_Generator()
{

	PrimaryActorTick.bCanEverTick = false;
	ObjectiveSource = CreateDefaultSubobject<UFR_ObjectiveSourceComponent>(TEXT("ObjectiveSource"));
}

void AFR_Generator::Interact_Implementation(AFR_CharacterBase* Interactor)
{
	if (!CanInteract_Implementation(Interactor)) return;
	UWorld* World = GetWorld();
	if (!World) return;
	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	if (!GameMode) return;

	FName RepairObjectiveId;
	FName StartObjectiveId;

	if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorRepair,RepairObjectiveId) ||!ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart,StartObjectiveId)) return;

	GameMode->RevealObjective(RepairObjectiveId);
	GameMode->RevealObjective(StartObjectiveId);

	StartRepair(Interactor);

}

bool AFR_Generator::CanInteract_Implementation(AFR_CharacterBase* Interactor)
{
	if (!IsValid(Interactor) || !Interactor->IsPlayerControlled()) return false;
	if (GeneratorState != EFR_GeneratorState::Ready) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	AFR_GameState* FR_GameState = GetWorld()->GetGameState<AFR_GameState>();
	if (!FR_GameState || FR_GameState->GetMatchPhase() != EFR_MatchPhase::Playing) return false;

	UFR_ObjectiveComponent* Objectives = FR_GameState->GetObjectiveComponent();

	FName RepairObjectiveId;
	FName StartObjectiveId;

	if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorRepair, RepairObjectiveId) || !ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart, StartObjectiveId)) return false;

	return !Objectives->IsObjectiveCompleted(RepairObjectiveId);
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
	else
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
	if (!FR_GameState || !GameMode) 
	{
		CanceledRepair();
		return;
	}


	UFR_ObjectiveComponent* Objectives = FR_GameState->GetObjectiveComponent();
	if (!Objectives)
	{
		CanceledRepair();
		return;
	}
	
	FName RepairObjectiveId;
	FName StartObjectiveId;

	if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorRepair,RepairObjectiveId) ||!ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart,StartObjectiveId))
	{
		CanceledRepair();
		return;
	}

	AFR_CharacterBase* SourceActor = RepairInstigator.Get();
	const bool bAdvanced = GameMode->TryAdvanceObjective(RepairObjectiveId, SourceActor);
	if (!bAdvanced)
	{
		GeneratorState = EFR_GeneratorState::Ready;
		RepairInstigator = nullptr;
		return;
	}
	GeneratorState = EFR_GeneratorState::Ready;
	// Здесь можно запускать эффекты успешного ремонта.

	if (Objectives->IsObjectiveCompleted(RepairObjectiveId))
	{
		GeneratorState = EFR_GeneratorState::Completed;

		GameMode->TryCompletedObjective(RepairObjectiveId, SourceActor);
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
	RepairInstigator = nullptr;
}

bool AFR_Generator::ResolveObjectiveId(FName SourceNodeId, FName& OutObjectiveId) const
{
	OutObjectiveId = NAME_None;

	if (SourceNodeId.IsNone()) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	const AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();

	if (!GameMode) return false;
	if (GameMode->IsProceduralObjectiveGraphActive())
	{
		// В процедурном режиме отсутствие привязки запрещает действие.
		return ObjectiveSource && ObjectiveSource->FindObjectiveId(SourceNodeId,OutObjectiveId);
	}

	// В фиксированном тестовом графе NodeId совпадает с ObjectiveId.
	OutObjectiveId = SourceNodeId;
	return true;
}


