// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/ET_LaserInterface.h"
#include "ET_LaserReceiver.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLaserReceiverActivatedChange,bool, NewValue);

UENUM()
enum class ELaserReceiverState : uint8
{
	Activeted,
	Deactiveted
};

class UBoxComponent;
class AET_LaserEmitter;

UCLASS()
class SPHEREIMPULSE_API AET_LaserReceiver : public AActor, public IET_LaserInterface
{
	GENERATED_BODY()
	
public:	

	AET_LaserReceiver();

	void SetLaserReceiverState(ELaserReceiverState NewState);

	UPROPERTY(BlueprintAssignable) FLaserReceiverActivatedChange LaserReceiverActivatedChangeDelegate;

protected:

	virtual void BeginPlay() override;

	virtual void SetLaserActive_Implementation(bool bActive) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Receiver") TObjectPtr<UBoxComponent> BoxCollision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Receiver") TObjectPtr<UStaticMeshComponent> LaserReceiver;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor GreenColor = FLinearColor::Green;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor RedColor = FLinearColor::Red;

private:

	UPROPERTY(EditAnywhere, Category = "Receiver") ELaserReceiverState CurrentLaserReceiverState = ELaserReceiverState::Deactiveted;
	UPROPERTY() UMaterialInstanceDynamic* LaserReceiverMaterialMID;

	void SetMaterialLaserReceiver();

	bool bInizialization = true;
	bool bLaserReceiverActiveted = false;




};
