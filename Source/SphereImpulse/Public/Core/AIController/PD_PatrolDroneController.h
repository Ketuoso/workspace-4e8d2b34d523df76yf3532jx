
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "PD_PatrolDroneController.generated.h"


class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class APD_PatrolDrone;


UENUM(BlueprintType)
enum class EPatrolDroneState : uint8
{
	Patrol,
	Chase,
	Search
};

/**
 * 
 */
UCLASS()
class SPHEREIMPULSE_API APD_PatrolDroneController : public AAIController
{
	GENERATED_BODY()

public:

	APD_PatrolDroneController();

	UFUNCTION(BlueprintCallable, Category = "Drone|Scanner") void SetPlayerInScanner(AActor* Actor, bool bInside);

protected:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID,const FPathFollowingResult& Result) override;

	UFUNCTION() void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI") UAIPerceptionComponent* DronePerception;
	UPROPERTY() UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone|AI") EPatrolDroneState CurrentState = EPatrolDroneState::Patrol;

	bool bPlayerVisible = false;
	bool bPlayerInScanner = false;


private:

	void StartPatrol();
	void TryDetectPlayer();
	void SetDroneState(EPatrolDroneState NewState);
	void CheckChaseMovement();
	void StartSearchWait();
	void FinishSearchWait();

	APD_PatrolDrone* PatrolDrone;
	FTimerHandle ChaseCheckTimer;
	FVector LastKnownPlayerLocation = FVector::ZeroVector;
	FTimerHandle SearchWaitTimer;
	int32 CurrentPatrolIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Scanner|Visual")FLinearColor PatrolColor = FLinearColor::Green;
	UPROPERTY(EditDefaultsOnly, Category = "Scanner|Visual")FLinearColor ChaseColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Scanner|Visual")FLinearColor SearchColor = FLinearColor::Yellow;

	UPROPERTY() TObjectPtr<APawn> ChaseTarget = nullptr;



};
