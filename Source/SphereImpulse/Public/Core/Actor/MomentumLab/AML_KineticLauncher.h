

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/WP_Interactable.h"
#include "AML_KineticLauncher.generated.h"

class UBoxComponent;
class AAML_PhysicsBall;

UCLASS()
class SPHEREIMPULSE_API AAML_KineticLauncher : public AActor, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	AAML_KineticLauncher();

protected:

	virtual void BeginPlay() override;
	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KineticLauncher") TObjectPtr<UStaticMeshComponent> KineticLauncherMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KineticLauncher") TObjectPtr<UBoxComponent> BoxCollision;

	UFUNCTION() void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	UPROPERTY(EditInstanceOnly, Category = "KineticLauncher") AAML_PhysicsBall* CurrentBall = nullptr;
	UPROPERTY(EditAnywhere, Category = "KineticLauncher") float LaunchPower = 1000.f;

	void LaunchBall();
};
