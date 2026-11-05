

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/WP_Interactable.h"
#include "ET_LaserEmitter.generated.h"



UENUM()
enum class ELaserEmitterState : uint8
{
	Activeted,
	Deactiveted
};

class UBoxComponent;
class UArrowComponent;
class USplineComponent;
class USplineMeshComponent;

UCLASS()
class SPHEREIMPULSE_API AET_LaserEmitter : public AActor, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	AET_LaserEmitter();

	void SetLaserEmitterState(ELaserEmitterState NewState);



protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual void InteractRBM_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Laser") TObjectPtr<UBoxComponent> BoxCollision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Laser") TObjectPtr<UStaticMeshComponent> LaserEmitter;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Laser") TObjectPtr<USceneComponent> LaserPoint;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Laser") TObjectPtr<UArrowComponent> ArrowLaser;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Laser|Visual")TObjectPtr<USplineComponent> LaserSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Laser|Visual")TObjectPtr<USplineMeshComponent> TestLaserMesh;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor GreenColor = FLinearColor::Green;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor RedColor = FLinearColor::Red;

private:

	UPROPERTY(EditAnywhere, Category = "Laser") ELaserEmitterState CurrentLaserEmitterState = ELaserEmitterState::Deactiveted;
	UPROPERTY(EditAnywhere, Category = "Laser") float MaxLaserDistance = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Laser") int32 MaxReflections = 99;
	UPROPERTY() UMaterialInstanceDynamic* LaserEmitterMaterialMID;

	void SetMaterialLaser();

	bool bInizialization = true;
	bool bLaserActiveted = false;

	void UpdateLaser();
	void UpdateLaserVisual();

	TSet<TWeakObjectPtr<AActor>> PreviousHitActors;
	TArray<FVector> LaserPoints;
	UPROPERTY(Transient) TArray<TObjectPtr<USplineMeshComponent>> LaserMeshSegments;
};
