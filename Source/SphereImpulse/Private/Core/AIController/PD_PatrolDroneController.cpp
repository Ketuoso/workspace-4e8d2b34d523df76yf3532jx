// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AIController/PD_PatrolDroneController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Character/PD_PatrolDrone.h"
#include "TimerManager.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/TargetPoint.h"

APD_PatrolDroneController::APD_PatrolDroneController()
{
	DronePerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("DronePerception"));
	SetPerceptionComponent(*DronePerception);
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	DronePerception->ConfigureSense(*SightConfig);
	DronePerception->SetDominantSense(UAISense_Sight::StaticClass());
}

void APD_PatrolDroneController::SetPlayerInScanner(AActor* Actor, bool bInside)
{
	if (Actor != UGameplayStatics::GetPlayerPawn(this, 0)) return;
	bPlayerInScanner = bInside;
	TryDetectPlayer();
}

void APD_PatrolDroneController::BeginPlay()
{
	Super::BeginPlay();

	PatrolDrone = Cast<APD_PatrolDrone>(GetPawn());

	DronePerception->OnTargetPerceptionUpdated.AddDynamic(this, &APD_PatrolDroneController::OnTargetDetected);
}

void APD_PatrolDroneController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	GetWorldTimerManager().SetTimerForNextTick(
		this,
		&APD_PatrolDroneController::StartPatrol
	);
}

void APD_PatrolDroneController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (!Result.IsSuccess()) return;
	if (CurrentState == EPatrolDroneState::Patrol)
	{
		if (!IsValid(PatrolDrone) || PatrolDrone->PatrolPoints.IsEmpty()) return;

		CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolDrone->PatrolPoints.Num();
		StartPatrol();
	}
	else if (CurrentState == EPatrolDroneState::Search)
	{
		StartSearchWait();
	}
}

void APD_PatrolDroneController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor != UGameplayStatics::GetPlayerPawn(this, 0)) return;
	bPlayerVisible = Stimulus.WasSuccessfullySensed();
	if (bPlayerVisible)
	{
		LastKnownPlayerLocation = Actor->GetActorLocation();
		TryDetectPlayer();
	}
	else if (CurrentState == EPatrolDroneState::Chase)
	{
		SetDroneState(EPatrolDroneState::Search);
	}
	TryDetectPlayer();
}

void APD_PatrolDroneController::StartPatrol()
{
	if (CurrentState != EPatrolDroneState::Patrol) return;
	if (!IsValid(PatrolDrone) || PatrolDrone->PatrolPoints.IsEmpty()) return;

	ATargetPoint* Point = PatrolDrone->PatrolPoints[CurrentPatrolIndex].Get();
	if (!IsValid(Point)) return;
		
	EPathFollowingRequestResult::Type Result = MoveToActor(Point, 130.f, false);
	

	if (Result == EPathFollowingRequestResult::AlreadyAtGoal
		&& PatrolDrone->PatrolPoints.Num() > 1)
	{
		CurrentPatrolIndex =
			(CurrentPatrolIndex + 1) % PatrolDrone->PatrolPoints.Num();

		GetWorldTimerManager().SetTimerForNextTick(
			this,
			&APD_PatrolDroneController::StartPatrol
		);
	}
}

void APD_PatrolDroneController::TryDetectPlayer()
{
	if (CurrentState != EPatrolDroneState::Chase && bPlayerVisible && bPlayerInScanner)
	{
		SetDroneState(EPatrolDroneState::Chase);
	}
}

void APD_PatrolDroneController::SetDroneState(EPatrolDroneState NewState)
{

	if (CurrentState == NewState) return;
	if (CurrentState == EPatrolDroneState::Chase)
	{
		GetWorldTimerManager().ClearTimer(ChaseCheckTimer);
	}
	if (CurrentState == EPatrolDroneState::Search)
	{
		GetWorldTimerManager().ClearTimer(SearchWaitTimer);
	}
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EPatrolDroneState::Patrol:
	{
		StartPatrol();
		if (PatrolDrone)
		{
			PatrolDrone->SwitchingSecurityScannerAlarm(PatrolColor);
		}
		break;
	}
	case EPatrolDroneState::Chase:
	{
		if (PatrolDrone)
		{
			PatrolDrone->SwitchingSecurityScannerAlarm(ChaseColor);
		}
		
		ChaseTarget = UGameplayStatics::GetPlayerPawn(this, 0);
		if (IsValid(ChaseTarget))
		{
			MoveToActor(ChaseTarget.Get(), 150.f);
			GetWorldTimerManager().SetTimer(ChaseCheckTimer, this, &APD_PatrolDroneController::CheckChaseMovement, 0.25f, true);

		}

		break;
	}

	case EPatrolDroneState::Search:
	{
		if (PatrolDrone)
		{
			PatrolDrone->SwitchingSecurityScannerAlarm(SearchColor);
		}
		EPathFollowingRequestResult::Type MoveResult = MoveToLocation(LastKnownPlayerLocation, 130.f);
		
		if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			StartSearchWait();
		}
		break;
	}

	default:
		break;
	}
}

void APD_PatrolDroneController::CheckChaseMovement()
{
	if (CurrentState != EPatrolDroneState::Chase || !bPlayerVisible || !IsValid(ChaseTarget) || !IsValid(GetPawn())) return;
	LastKnownPlayerLocation = ChaseTarget->GetActorLocation();
	if (GetMoveStatus() == EPathFollowingStatus::Moving) return;
	const float Distance = FVector::Dist2D(GetPawn()->GetActorLocation(), ChaseTarget->GetActorLocation());
	if (Distance > 300.f)
	{
		MoveToActor(ChaseTarget.Get(), 150.f);
	}

}

void APD_PatrolDroneController::StartSearchWait()
{
	if (CurrentState != EPatrolDroneState::Search || GetWorldTimerManager().IsTimerActive(SearchWaitTimer)) return;

	GetWorldTimerManager().SetTimer(SearchWaitTimer, this, &APD_PatrolDroneController::FinishSearchWait, 3.f, false);
}

void APD_PatrolDroneController::FinishSearchWait()
{
	if (CurrentState == EPatrolDroneState::Search)
	{
		SetDroneState(EPatrolDroneState::Patrol);
	}
}
