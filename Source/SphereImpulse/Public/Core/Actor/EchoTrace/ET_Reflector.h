// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/ET_LaserInterface.h"
#include "Core/Interface/WP_Interactable.h"
#include "ET_Reflector.generated.h"

UENUM()
enum class ELaserReflectorState : uint8
{
	Activeted,
	Deactiveted
};

class AET_LaserEmitter;
class UBoxComponent;

UCLASS()
class SPHEREIMPULSE_API AET_Reflector : public AActor, public IET_LaserInterface, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	AET_Reflector();

	void SetLaserReceiverState(ELaserReflectorState NewState);

	FVector GetSurfaceNormal() const;

protected:

	virtual void BeginPlay() override;

	virtual void SetLaserActive_Implementation(bool bActive) override;

	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual void InteractRBM_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reflector") TObjectPtr<UBoxComponent> BoxCollision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reflector") TObjectPtr<UStaticMeshComponent> LaserReflector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reflector") TObjectPtr<UBoxComponent> ReflectionSurface;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor GreenColor = FLinearColor::Green;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor RedColor = FLinearColor::Red;

private:

	UPROPERTY(EditAnywhere, Category = "Reflector") ELaserReflectorState CurrentLaserReflectorState = ELaserReflectorState::Deactiveted;
	UPROPERTY() UMaterialInstanceDynamic* LaserReflectorMaterialMID;

	void SetMaterialLaser();

	bool bInizialization = true;
	bool bLaserActiveted = false;
	UPROPERTY(EditAnywhere, Category = "Reflector") float AddAngle = 15.f;



};
