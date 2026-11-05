


#include "Core/Components/CO_HackableComponent.h"
#include "Core/Actor/Generator/PC_BasePowerDevice.h"
#include "TimerManager.h"
#include "Engine/World.h"


UCO_HackableComponent::UCO_HackableComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

void UCO_HackableComponent::TryHacked()
{
	if (HackedState == EHackedState::Hacked) return;
	SetHackedState(EHackedState::Hacked);
}


void UCO_HackableComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCO_HackableComponent::SetHackedState(EHackedState NewState)
{
	if (NewState == HackedState) return;
	if (HackedState == EHackedState::Hacked)
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(WaitingToActive);
	}
	HackedState = NewState;

	switch (HackedState)
	{
	case EHackedState::Active:
	{
		UE_LOG(LogTemp, Warning, TEXT("Active device: %s"), *GetOwner()->GetName());
		HackedStateChangeDelegate.Broadcast(true);
		break;
	}
	case EHackedState::Hacked:
	{
		UE_LOG(LogTemp, Warning, TEXT("Haked device: %s"), *GetOwner()->GetName());
		StartWaiting();
		HackedStateChangeDelegate.Broadcast(false);
		break;
	}
	default:
		break;
	}
}

void UCO_HackableComponent::StartWaiting()
{
	GetOwner()->GetWorldTimerManager().SetTimer(WaitingToActive, this, &UCO_HackableComponent::FinishWaiting, HackDuration, false);
}

void UCO_HackableComponent::FinishWaiting()
{
	SetHackedState(EHackedState::Active);

}




