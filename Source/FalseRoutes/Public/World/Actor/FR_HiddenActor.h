

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<USceneComponent> ExitPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<USceneComponent> HidePoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<UStaticMeshComponent> HideSpot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<UArrowComponent> ForwardHideSpot;

};
