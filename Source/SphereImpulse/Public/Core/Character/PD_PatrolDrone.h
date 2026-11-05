// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PD_PatrolDrone.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchingPatrolDroneAlarmChange);

class ATargetPoint;

UCLASS()
class SPHEREIMPULSE_API APD_PatrolDrone : public ACharacter
{
	GENERATED_BODY()

public:

	APD_PatrolDrone();

	UFUNCTION(BlueprintCallable) void SwitchingSecurityScannerAlarm(FLinearColor TargetColor);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Drone|Patrol")
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DRONE") TObjectPtr<USceneComponent> PivotScanner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DRONE") TObjectPtr<UStaticMeshComponent> Scanner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DRONE") TObjectPtr<UStaticMeshComponent> ZoneScanner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DRONE") TObjectPtr<UCapsuleComponent> CapsuleCollisionScanner;

	UPROPERTY(BlueprintReadOnly) bool bCurrentAlarmStatus = true;

	UPROPERTY(BlueprintAssignable) FOnSwitchingPatrolDroneAlarmChange SwitchingpatrolDroneAlarmChange;


private:

	UFUNCTION() void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	

	UPROPERTY() TArray<AActor*> DetectedActors;

	UPROPERTY() UMaterialInstanceDynamic* MeshMaterialMID;
	UPROPERTY() UMaterialInstanceDynamic* ZoneMaterialMID;
	UPROPERTY() UMaterialInstanceDynamic* ScannerMaterialMID;

	UPROPERTY(EditDefaultsOnly, Category = "DRONE|Visual")FLinearColor SafeColor = FLinearColor::Green;
	UPROPERTY(EditDefaultsOnly, Category = "DRONE|Visual")FLinearColor AlarmColor = FLinearColor::Red;

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
