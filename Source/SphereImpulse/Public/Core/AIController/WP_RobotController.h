

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WP_RobotController.generated.h"

UENUM(BlueprintType)
enum class EServiceRobotState : uint8
{
	Idle,
	Moving,
	Waiting
};

class AWP_ServiceRobot;

/**
 * 
 */
UCLASS()
class SPHEREIMPULSE_API AWP_RobotController : public AAIController
{
	GENERATED_BODY()

public:
	
	void TryInteract();

protected:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	void SetRobotState(EServiceRobotState NewState);
	
private:

	EServiceRobotState ServiceRopotState = EServiceRobotState::Idle;
	FTimerHandle WaitingTimer;
	
	AWP_ServiceRobot* ServiceRobot;

	FVector MovingPoint = FVector::ZeroVector;

	void StartWaiting();
	void FinishWaiting();
};
