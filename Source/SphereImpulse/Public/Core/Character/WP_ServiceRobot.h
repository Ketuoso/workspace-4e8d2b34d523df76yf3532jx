
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/Interface/WP_Interactable.h"
#include "WP_ServiceRobot.generated.h"


class ATargetPoint;

UCLASS()
class SPHEREIMPULSE_API AWP_ServiceRobot : public ACharacter, public IWP_Interactable
{
	GENERATED_BODY()

public:

	AWP_ServiceRobot();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Ropot|Point") ATargetPoint* ServicePoint;


protected:

	virtual void BeginPlay() override;
	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot") TObjectPtr<USceneComponent> PivotRoller;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot") TObjectPtr<UStaticMeshComponent> Roller;


private:


};
