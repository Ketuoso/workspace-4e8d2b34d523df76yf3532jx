


#include "World/Actor/FR_Door.h"
#include "Core/Character/FR_CharacterBase.h"
#include "Core/GameMode/FR_LevelGameMode.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Components/FR_ObjectiveComponent.h"
#include "Core/Components/FR_ObjectiveSourceComponent.h"
#include "Core/Data/FR_ObjectiveIds.h"


AFR_Door::AFR_Door()
{

	PrimaryActorTick.bCanEverTick = false;
	ObjectiveSource =CreateDefaultSubobject<UFR_ObjectiveSourceComponent>(TEXT("ObjectiveSource"));
}

void AFR_Door::Interact_Implementation(AFR_CharacterBase* Interactor)
{
	if (!CanInteract_Implementation(Interactor)) return;
	UWorld* World = GetWorld();
	if (!World) return;
	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	if (!GameMode) return;

	FName DoorObjectiveId;

	if (!ResolveObjectiveId(FR_ObjectiveIds::DoorOpen,DoorObjectiveId)) return;

	GameMode->RevealObjectiveWithRequirements(DoorObjectiveId);

	// Если требования ещё не выполнены, дверь останется закрытой.
	TryOpenDoor(Interactor);
}

bool AFR_Door::CanInteract_Implementation(AFR_CharacterBase* Interactor)
{
	if (!IsValid(Interactor) || !Interactor->IsPlayerControlled()) return false;
	if (DoorState != EFR_DoorState::Closed) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	AFR_GameState* FR_GameState = GetWorld()->GetGameState<AFR_GameState>();
	if (!FR_GameState || FR_GameState->GetMatchPhase() != EFR_MatchPhase::Playing) return false;

	UFR_ObjectiveComponent* Objectives = FR_GameState->GetObjectiveComponent();

	FName DoorObjectiveId;
	if (!ResolveObjectiveId(FR_ObjectiveIds::DoorOpen,DoorObjectiveId)) return false;

	FFR_ObjectiveRuntimeState RuntimeState;
	if (!Objectives->GetObjectiveRuntimeState(DoorObjectiveId,RuntimeState)) return false;

	// До срабатывания триггера дверь взаимодействие не разрешает.
	if (!RuntimeState.bIsDiscovered) return false;
	if (RuntimeState.Status == EFR_ObjectiveStatus::Completed) return false;

	return true;
}


void AFR_Door::BeginPlay()
{
	Super::BeginPlay();
	
}


void AFR_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AFR_Door::ResolveObjectiveId(FName SourceNodeId, FName& OutObjectiveId) const
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
		return ObjectiveSource && ObjectiveSource->FindObjectiveId(SourceNodeId, OutObjectiveId);
	}

	// В фиксированном тестовом графе NodeId совпадает с ObjectiveId.
	OutObjectiveId = SourceNodeId;
	return true;
}

bool AFR_Door::TryOpenDoor(AFR_CharacterBase* Interactor)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	if (!GameMode) return false;

	FName DoorObjectiveId;
	if (!ResolveObjectiveId(FR_ObjectiveIds::DoorOpen,DoorObjectiveId)) return false;

	const bool bCompleted = GameMode->TryCompletedObjective(DoorObjectiveId,Interactor);

	if (!bCompleted)
	{
		UE_LOG(LogTemp,Verbose,TEXT("Door cannot be opened yet: requirements are not met."));
		// Здесь позднее будет событие для плавающего сообщения.
		return false;
	}

	OpenDoor();
	return true;
}

void AFR_Door::OpenDoor()
{
	if (DoorState == EFR_DoorState::Open) return;
	DoorState = EFR_DoorState::Open;

	UE_LOG(LogTemp,Display,TEXT("Door opened: %s"),*GetName());

	// Позднее:
	// - анимация;
	// - звук;
	// - изменение коллизии;
	// - открытие прохода к зоне побега.
}

