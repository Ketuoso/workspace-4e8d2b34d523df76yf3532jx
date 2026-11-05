// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Actor/Generator/PC_BasePowerDevice.h"
#include "CO_SecurityTurret.generated.h"

/**
 * 
 */
UCLASS()
class SPHEREIMPULSE_API ACO_SecurityTurret : public APC_BasePowerDevice
{
	GENERATED_BODY()
	
public:

	ACO_SecurityTurret();

protected:

	virtual void SetMIDs() override;
	virtual void Execute_Interact(APlayerController* InteractingController) override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Turret") TObjectPtr<UStaticMeshComponent> BaseTurret;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Turret") TObjectPtr<UStaticMeshComponent> SwivelMechanismTurret;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Turret") TObjectPtr<UStaticMeshComponent> CannonTurret;

};
