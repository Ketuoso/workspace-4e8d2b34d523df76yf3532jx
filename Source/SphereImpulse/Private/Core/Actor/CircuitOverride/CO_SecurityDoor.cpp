// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actor/CircuitOverride/CO_SecurityDoor.h"

ACO_SecurityDoor::ACO_SecurityDoor()
{
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	EnergyShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnergyShield"));


	RootComponent = DoorFrame;
	EnergyShield->SetupAttachment(DoorFrame);

	EnergyShield->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
}

void ACO_SecurityDoor::SetMIDs()
{
	MIDs.Empty();
	MIDs.AddUnique(DoorFrame->CreateDynamicMaterialInstance(1));
}

void ACO_SecurityDoor::Execute_Interact(APlayerController* InteractingController)
{
}

void ACO_SecurityDoor::BindCallbacksToDependencies()
{
	SwitchingPowerDeviceStatus.AddDynamic(this, &ACO_SecurityDoor::SetEnergyShielState);
}

void ACO_SecurityDoor::SetEnergyShielState(bool bValue)
{
	EnergyShield->SetVisibility(bValue);
	if (bValue)
	{
		EnergyShield->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		EnergyShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	}
	else
	{
		EnergyShield->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EnergyShield->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}
