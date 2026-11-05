// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actor/Generator/PC_BasePowerDevice.h"
#include "Core/Components/CO_HackableComponent.h"


APC_BasePowerDevice::APC_BasePowerDevice()
{

	PrimaryActorTick.bCanEverTick = false;

	HackableComponent = CreateDefaultSubobject<UCO_HackableComponent>(TEXT("HackableComponent"));

}

float APC_BasePowerDevice::GetPowerCost() const
{
	return PowerCost;
}
EPoweredState APC_BasePowerDevice::GetPoweredState() const
{
	return PoweredState;
}


void APC_BasePowerDevice::BeginPlay()
{
	Super::BeginPlay();
	SetMIDs();
	BindCallbacksToDependencies();
	HackableComponent->HackedStateChangeDelegate.AddDynamic(this, &APC_BasePowerDevice::HandleHackedStateChangeDelegate);
}

void APC_BasePowerDevice::SetMIDs()
{
}

void APC_BasePowerDevice::Execute_Interact(APlayerController* InteractingController)
{
}

void APC_BasePowerDevice::BindCallbacksToDependencies()
{
}

void APC_BasePowerDevice::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	HackableComponent->TryHacked();
	Execute_Interact(InteractingController);
}

bool APC_BasePowerDevice::CanInteract_Implementation(APlayerController* InteractingController) const
{
	return true;
}

void APC_BasePowerDevice::SwitchingPowerDevice(bool bStatus)
{
	if (bCurrentStatus == bStatus) return;
	bCurrentStatus = bStatus;

	const FLinearColor TargetColor = bStatus ? SafeColor : AlarmColor;
	PoweredState = bStatus ? EPoweredState::Powered : EPoweredState::Unpowered;
	
	if (MIDs.Num() > 0)
	{
		for (auto& MID : MIDs)
		{
			MID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
		}
	}
	SwitchingPowerDeviceStatus.Broadcast(bStatus);
}

void APC_BasePowerDevice::HandleHackedStateChangeDelegate(bool NewStatus)
{
	SwitchingPowerDevice(NewStatus);
}


