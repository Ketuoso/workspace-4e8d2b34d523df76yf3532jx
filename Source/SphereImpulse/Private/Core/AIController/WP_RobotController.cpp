
#include "Core/AIController/WP_RobotController.h"
#include "Core/Character/WP_ServiceRobot.h"
#include "TimerManager.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/TargetPoint.h"

void AWP_RobotController::TryInteract()
{
	if (ServiceRopotState != EServiceRobotState::Idle || ServiceRobot->ServicePoint == nullptr) return;
	MovingPoint = ServiceRobot->ServicePoint->GetActorLocation();
	SetRobotState(EServiceRobotState::Moving);

}

void AWP_RobotController::BeginPlay()
{
	Super::BeginPlay();
	ServiceRobot = Cast<AWP_ServiceRobot>(GetPawn());
}

void AWP_RobotController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AWP_RobotController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	if (!Result.IsSuccess())
	{
		SetRobotState(EServiceRobotState::Idle);
	}
	if (ServiceRopotState != EServiceRobotState::Moving) return;
	
	StartWaiting();
}

void AWP_RobotController::SetRobotState(EServiceRobotState NewState)
{
	if (ServiceRopotState == NewState) return;

	if (ServiceRopotState == EServiceRobotState::Waiting)
	{
		GetWorldTimerManager().ClearTimer(WaitingTimer);
	}

	ServiceRopotState = NewState;

	switch (ServiceRopotState)
	{
	case EServiceRobotState::Idle:
	{

		break;
	}
	case EServiceRobotState::Moving:
	{
		EPathFollowingRequestResult::Type MoveResult = MoveToLocation(MovingPoint, 100.f);

		if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			StartWaiting();
		}
		if (MoveResult == EPathFollowingRequestResult::Failed)
		{
			SetRobotState(EServiceRobotState::Idle);
		}

		break;
	}
	case EServiceRobotState::Waiting:
	{
		StartWaiting();

		break;
	}
	default:
		break;
	}
}

void AWP_RobotController::StartWaiting()
{
	GetWorldTimerManager().SetTimer(WaitingTimer, this, &AWP_RobotController::FinishWaiting, 3.f, false);
}

void AWP_RobotController::FinishWaiting()
{
	SetRobotState(EServiceRobotState::Idle);
}
