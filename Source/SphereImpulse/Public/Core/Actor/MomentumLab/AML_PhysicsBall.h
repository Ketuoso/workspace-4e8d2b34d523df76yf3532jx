

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/WP_Interactable.h"
#include "AML_PhysicsBall.generated.h"

class USphereComponent;
class UPhysicsHandleComponent;

UCLASS()
class SPHEREIMPULSE_API AAML_PhysicsBall : public AActor, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	AAML_PhysicsBall();

	void AddImpulseBall(FVector VectorImpulse);

	USphereComponent* GetSphereCollision() const { return SphereCollision; }

protected:

	virtual void BeginPlay() override;
	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PhysicsBall") TObjectPtr<USphereComponent> SphereCollision;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PhysicsBall") TObjectPtr<UStaticMeshComponent> PhysicsBall;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PhysicsBall") float PhysicsBallMass = 10.f;

private:

	FVector StartPositionBall = FVector::ZeroVector;


};
