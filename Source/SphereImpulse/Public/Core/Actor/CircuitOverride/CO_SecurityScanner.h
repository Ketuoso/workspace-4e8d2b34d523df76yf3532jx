// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Actor/Generator/PC_BasePowerDevice.h"
#include "CO_SecurityScanner.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class SPHEREIMPULSE_API ACO_SecurityScanner : public APC_BasePowerDevice
{
	GENERATED_BODY()
	
public:

	ACO_SecurityScanner();

protected:

	virtual void Tick(float DeltaTime) override;

	virtual void SetMIDs() override;
	virtual void Execute_Interact(APlayerController* InteractingController) override;
	virtual void BindCallbacksToDependencies() override;

	UFUNCTION() void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION() void SwitchingScannerAlarm(bool bStatus);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner") TObjectPtr<UBoxComponent> BoxCollision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner") TObjectPtr<UStaticMeshComponent> SecurityScanner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner") TObjectPtr<UStaticMeshComponent> ZoneScanner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner") TObjectPtr<UStaticMeshComponent> ZoneScannerCollision;

private:

	UPROPERTY() TArray<AActor*> DetectedActors;

	float MinAngleZoneScanner = -60.f;
	float MaxAngleZoneScanner = -10.f;
	float ScanAlpha = 1.f;
	float AlphaSpeed = 0.5f;
	bool bUp = true;

};
