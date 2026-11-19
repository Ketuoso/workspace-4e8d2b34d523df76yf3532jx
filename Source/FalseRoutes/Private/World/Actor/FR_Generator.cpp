


#include "World/Actor/FR_Generator.h"

#include "Core/GameState/FR_GameState.h"
#include "Core/Character/FR_CharacterBase.h"
#include "Core/GameMode/FR_LevelGameMode.h"
#include "Core/Components/FR_ObjectiveComponent.h"
#include "Core/Components/FR_ObjectiveSourceComponent.h"
#include "Core/Data/FR_ObjectiveIds.h"

AFR_Generator::AFR_Generator()
{

	PrimaryActorTick.bCanEverTick = false;
	ObjectiveSource = CreateDefaultSubobject<UFR_ObjectiveSourceComponent>(TEXT("ObjectiveSource"));
}

void AFR_Generator::Interact_Implementation(AFR_CharacterBase* Interactor)
{
	const EFR_GeneratorInteractionAction Action = ResolveInteractionAction(Interactor);
	
	if (Action == EFR_GeneratorInteractionAction::None) return;
	UWorld* World = GetWorld();
	if (!World) return;
	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	if (!GameMode) return;

	switch (Action)
	{
	case EFR_GeneratorInteractionAction::Inspect:
		InspectGenerator(Interactor);
		break;

	case EFR_GeneratorInteractionAction::Repair:
	{
		// Старый фиксированный тестовый сценарий сохраняет
		// прежнее поведение: обе задачи видимы сразу.
		if (!GameMode->IsProceduralObjectiveGraphActive())
		{
			FName RepairObjectiveId;
			FName StartObjectiveId;

			if (ResolveObjectiveId(FR_ObjectiveIds::GeneratorRepair,RepairObjectiveId))
			{
				GameMode->RevealObjective(RepairObjectiveId);
			}

			if (ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart,StartObjectiveId))
			{
				GameMode->RevealObjective(StartObjectiveId);
			}
		}

		StartRepair(Interactor);
		break;
	}
	case EFR_GeneratorInteractionAction::Start:
		TryStartGenerator(Interactor);
		break;

	default:
		break;
	}
}

bool AFR_Generator::CanInteract_Implementation(AFR_CharacterBase* Interactor)
{
	return ResolveInteractionAction(Interactor) != EFR_GeneratorInteractionAction::None;
}

void AFR_Generator::BeginPlay()
{
	Super::BeginPlay();
	
}

EFR_GeneratorInteractionAction AFR_Generator::ResolveInteractionAction(AFR_CharacterBase* Interactor) const
{
	if (!IsValid(Interactor) || !Interactor->IsPlayerControlled()) return EFR_GeneratorInteractionAction::None;
	if (GeneratorState == EFR_GeneratorState::Repairing || GeneratorState == EFR_GeneratorState::Running) return EFR_GeneratorInteractionAction::None;

	UWorld* World = GetWorld();
	if (!World) return EFR_GeneratorInteractionAction::None;

	AFR_GameState* FRGameState =World->GetGameState<AFR_GameState>();
	AFR_LevelGameMode* GameMode =World->GetAuthGameMode<AFR_LevelGameMode>();
	if (!FRGameState || !GameMode || FRGameState->GetMatchPhase() != EFR_MatchPhase::Playing) return EFR_GeneratorInteractionAction::None;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	if (!Objectives) return EFR_GeneratorInteractionAction::None;


	if (GeneratorState == EFR_GeneratorState::Offline)
	{
		FName RepairObjectiveId;

		if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorRepair, RepairObjectiveId)) return EFR_GeneratorInteractionAction::None;

		// Fixed fallback: старая цепочка без отдельного осмотра.
		if (!GameMode->IsProceduralObjectiveGraphActive())
		{
			return Objectives->IsObjectiveCompleted(RepairObjectiveId) ? EFR_GeneratorInteractionAction::None : EFR_GeneratorInteractionAction::Repair;
		}

		FName InspectObjectiveId;
		FName StartObjectiveId;

		// Все три привязки обязательны для процедурной цепочки.
		if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorInspect,InspectObjectiveId) ||!ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart,StartObjectiveId)) return EFR_GeneratorInteractionAction::None;

		if (!Objectives->IsObjectiveCompleted(InspectObjectiveId))
		{
			FFR_ObjectiveRuntimeState InspectState;
			if (!Objectives->GetObjectiveRuntimeState(InspectObjectiveId,InspectState) ||!InspectState.bIsDiscovered) return EFR_GeneratorInteractionAction::None;
			return EFR_GeneratorInteractionAction::Inspect;
		}

		if (!Objectives->IsObjectiveCompleted(RepairObjectiveId))
		{
			FFR_ObjectiveRuntimeState RepairState;
			if (!Objectives->GetObjectiveRuntimeState(RepairObjectiveId,RepairState) ||!RepairState.bIsDiscovered) return EFR_GeneratorInteractionAction::None;
			return EFR_GeneratorInteractionAction::Repair;
		}
		return EFR_GeneratorInteractionAction::None;
	}

	if (GeneratorState == EFR_GeneratorState::Repaired)
	{
		FName StartObjectiveId;

		if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart,StartObjectiveId)) return EFR_GeneratorInteractionAction::None;
		FFR_ObjectiveRuntimeState StartState;

		if (!Objectives->GetObjectiveRuntimeState(StartObjectiveId,StartState) ||!StartState.bIsDiscovered ||StartState.Status == EFR_ObjectiveStatus::Completed) return EFR_GeneratorInteractionAction::None;
		if (!Objectives->AreObjectiveRequirementsMet(StartObjectiveId)) return EFR_GeneratorInteractionAction::None;
		return EFR_GeneratorInteractionAction::Start;
	}

	return EFR_GeneratorInteractionAction::None;
}

void AFR_Generator::InspectGenerator(AFR_CharacterBase* Interactor)
{
	UWorld* World = GetWorld();
	if (!World) return;

	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	AFR_GameState* FRGameState = World->GetGameState<AFR_GameState>();
	if (!GameMode || !FRGameState) return;

	UFR_ObjectiveComponent* Objectives = FRGameState->GetObjectiveComponent();
	if (!Objectives) return;

	FName InspectObjectiveId;
	FName RepairObjectiveId;
	FName StartObjectiveId;

	if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorInspect,InspectObjectiveId) ||
		!ResolveObjectiveId(FR_ObjectiveIds::GeneratorRepair,RepairObjectiveId) ||
		!ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart,StartObjectiveId)) return;

	if (!GameMode->TryCompletedObjective(InspectObjectiveId,Interactor)) return;

	// Если ремонт был выполнен заранее, осмотр обнаруживает
	// фактическую готовность устройства к запуску.
	if (Objectives->IsObjectiveCompleted(RepairObjectiveId))
	{
		GeneratorState = EFR_GeneratorState::Repaired;
	}

	// Start появится сразу, если все требования уже выполнены.
	// Иначе появятся только оставшиеся подзадачи.
	GameMode->RequestObjectiveDiscovery(StartObjectiveId);
}

void AFR_Generator::StartRepair(AFR_CharacterBase* Interactor)
{
	if (GeneratorState != EFR_GeneratorState::Offline) return;

	GeneratorState = EFR_GeneratorState::Repairing;
	RepairInstigator = Interactor;

	if (RepairDuration > 0.f)
	{
		GetWorldTimerManager().SetTimer(RepairTimerHandle,this,&AFR_Generator::CompletedRepair,RepairDuration,false);
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
	if (!World)
	{
		CanceledRepair();
		return;
	}

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

	if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorRepair,RepairObjectiveId))
	{
		CanceledRepair();
		return;
	}

	AFR_CharacterBase* SourceActor = RepairInstigator.Get();

	// За одно завершение таймера добавляем ровно один ремонт.
	const bool bAdvanced = GameMode->TryAdvanceObjective(RepairObjectiveId,SourceActor);

	if (!bAdvanced)
	{
		GeneratorState = EFR_GeneratorState::Offline;
		RepairInstigator = nullptr;
		return;
	}

	const bool bRepairCompleted = Objectives->IsObjectiveCompleted(RepairObjectiveId);

	GeneratorState = bRepairCompleted ? EFR_GeneratorState::Repaired : EFR_GeneratorState::Offline;

	// Только старый fixed fallback автоматически запускает генератор
	// после последнего ремонта.
	if (bRepairCompleted && !GameMode->IsProceduralObjectiveGraphActive())
	{
		FName StartObjectiveId;

		if (ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart,StartObjectiveId))
		{
			TryStartGenerator(SourceActor);
		}
	}

	RepairInstigator = nullptr;
}

void AFR_Generator::CanceledRepair()
{
	if (GeneratorState == EFR_GeneratorState::Repairing)
	{
		GetWorldTimerManager().ClearTimer(RepairTimerHandle);
		GeneratorState = EFR_GeneratorState::Offline;
	}
	RepairInstigator = nullptr;
}

bool AFR_Generator::TryStartGenerator(AFR_CharacterBase* Interactor)
{
	if (GeneratorState == EFR_GeneratorState::Repairing) return false;
	UWorld* World = GetWorld();
	
	if (!World) return false;

	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	if (!GameMode) return false;

	FName StartObjectiveId;

	if (!ResolveObjectiveId(FR_ObjectiveIds::GeneratorStart,StartObjectiveId)) return false;

	// Не переводим устройство в Running при отказе ObjectiveComponent:
	// например, если ещё нет топлива или ключа.
	if (!GameMode->TryCompletedObjective(StartObjectiveId,Interactor))
	{
		UE_LOG(LogTemp,Verbose,TEXT("Generator cannot be started: requirements are not met."));
		return false;
	}

	GeneratorState = EFR_GeneratorState::Running;

	UE_LOG(LogTemp,Display,TEXT("Generator started: %s"),*GetName());
	return true;
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


