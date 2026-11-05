// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actor/CircuitOverride/CO_SecurityTurret.h"

ACO_SecurityTurret::ACO_SecurityTurret()
{
	BaseTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseTurret"));
	SwivelMechanismTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwivelMechanismTurret"));
	CannonTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonTurret"));

	RootComponent = BaseTurret;
	SwivelMechanismTurret->SetupAttachment(BaseTurret);
	CannonTurret->SetupAttachment(SwivelMechanismTurret);


}

void ACO_SecurityTurret::SetMIDs()
{
	MIDs.Empty();
	MIDs.AddUnique(BaseTurret->CreateDynamicMaterialInstance(1));
	MIDs.AddUnique(SwivelMechanismTurret->CreateDynamicMaterialInstance(1));
	MIDs.AddUnique(CannonTurret->CreateDynamicMaterialInstance(1));
}

void ACO_SecurityTurret::Execute_Interact(APlayerController* InteractingController)
{
}

void ACO_SecurityTurret::BindCallbacksToDependencies()
{
}
