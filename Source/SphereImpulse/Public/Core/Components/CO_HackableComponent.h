

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CO_HackableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHackedStateChange, bool, NewStatus);


UENUM(BlueprintType)
enum class EHackedState : uint8
{
	Active,
	Hacked
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPHEREIMPULSE_API UCO_HackableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCO_HackableComponent();


	void TryHacked();

	EHackedState GetHackedState() const { return HackedState; }

	FOnHackedStateChange HackedStateChangeDelegate;

protected:

	virtual void BeginPlay() override;

	void SetHackedState(EHackedState NewState);

private:

	UPROPERTY(EditInstanceOnly, Category = "Hacking") float HackDuration = 5.f;

	EHackedState HackedState = EHackedState::Active;
	FTimerHandle WaitingToActive;

	void StartWaiting();
	void FinishWaiting();

};
