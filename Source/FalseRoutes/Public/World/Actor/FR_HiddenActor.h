

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/FR_Interactable.h"
#include "FR_HiddenActor.generated.h"

class UArrowComponent;
class AFR_CharacterBase;

UCLASS()
class FALSEROUTES_API AFR_HiddenActor : public AActor, public IFR_Interactable
{
	GENERATED_BODY()
	
public:	

	AFR_HiddenActor();


	virtual void Interact_Implementation(AFR_CharacterBase* Interactor) override;
	virtual bool CanInteract_Implementation(AFR_CharacterBase* Interactor) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HideSpot") TObjectPtr<USceneComponent> Root;
	UPROPERTY(EditAnywhere, Category = "HideSpot") TObjectPtr<USceneComponent> ExitPoint;
	UPROPERTY(EditAnywhere, Category = "HideSpot") TObjectPtr<USceneComponent> HidePoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HideSpot") TObjectPtr<UStaticMeshComponent> HideSpot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HideSpot") TObjectPtr<UArrowComponent> ForwardHideSpot;

};
