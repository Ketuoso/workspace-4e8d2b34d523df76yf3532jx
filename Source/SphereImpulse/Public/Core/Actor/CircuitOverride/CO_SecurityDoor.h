// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Actor/Generator/PC_BasePowerDevice.h"
#include "CO_SecurityDoor.generated.h"

/**
 * 
 */
UCLASS()
class SPHEREIMPULSE_API ACO_SecurityDoor : public APC_BasePowerDevice
{
	GENERATED_BODY()

public:

	ACO_SecurityDoor();

protected:

	virtual void SetMIDs() override;
	virtual void Execute_Interact(APlayerController* InteractingController) override;
	virtual void BindCallbacksToDependencies() override;
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door") TObjectPtr<UStaticMeshComponent> DoorFrame;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door") TObjectPtr<UStaticMeshComponent> EnergyShield;

private:

	UFUNCTION() void SetEnergyShielState(bool bValue);



};
