// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SS_SecurityScanner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchingSecurityScannerAlarmChange);

class UBoxComponent;

UCLASS()
class SPHEREIMPULSE_API ASS_SecurityScanner : public AActor
{
	GENERATED_BODY()
	
public:	

	ASS_SecurityScanner();

protected:

	virtual void BeginPlay() override;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner") TObjectPtr<UBoxComponent> BoxCollision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner") TObjectPtr<UStaticMeshComponent> SecurityScanner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner") TObjectPtr<UStaticMeshComponent> ZoneScanner;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Scanner") TObjectPtr<UStaticMeshComponent> ZoneScannerCollision;
	
	UPROPERTY(BlueprintReadOnly) bool bCurrentAlarmStatus = false;

	UPROPERTY(BlueprintAssignable) FOnSwitchingSecurityScannerAlarmChange SwitchingSecurityScannerAlarmChange;

private:

	UFUNCTION() void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	UFUNCTION() void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable) void SwitchingSecurityScannerAlarm(bool bAlarmStatus);

	UPROPERTY() TArray<AActor*> DetectedActors;
	

	UPROPERTY() UMaterialInstanceDynamic* ZoneMaterialMID;
	UPROPERTY() UMaterialInstanceDynamic* ScannerMaterialMID;

	UPROPERTY(EditDefaultsOnly, Category = "Scanner|Visual")FLinearColor SafeColor = FLinearColor::Green;
	UPROPERTY(EditDefaultsOnly, Category = "Scanner|Visual")FLinearColor AlarmColor = FLinearColor::Red;
};
