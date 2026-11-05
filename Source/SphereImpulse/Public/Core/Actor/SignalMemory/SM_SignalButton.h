

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/WP_Interactable.h"
#include "SM_SignalButton.generated.h"

class APLayerController;

UENUM()
enum class EButtonState 
{
	Deactivated,
	WaitInterract,
	Activated

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonInteractionRequestedFinish,ASM_SignalButton*, ButtonInteraction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonInteractionRequestedStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonInteractionRequested,APlayerController*, InteractingController, ASM_SignalButton*, ButtonInteraction);

UCLASS()
class SPHEREIMPULSE_API ASM_SignalButton : public AActor, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	ASM_SignalButton();

	void SetButtonState(EButtonState NewState);

	void SwitchingButtonMID(bool bSequence);

	FOnButtonInteractionRequestedFinish OnButtonInteractionRequestedFinishDelegate;
	FOnButtonInteractionRequestedStart OnButtonInteractionRequestedStartDelegate;
	FOnButtonInteractionRequested OnButtonInteractionRequestedDelegate;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Button") TObjectPtr<UStaticMeshComponent> Button;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Button") TObjectPtr<USceneComponent> SceneRootComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Button") EButtonState CurrentButtonState = EButtonState::Deactivated;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Button") FLinearColor SequenceColor = FLinearColor::White;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Button") FLinearColor BaseColor = FLinearColor::Blue;

private:

	UPROPERTY() UMaterialInstanceDynamic* ButtonMID;

	APlayerController* CurrentInteractingController = nullptr;

	float MinLocationZ = -20;
	float MaxLocationZ = 0;
	float Alpha = 1.f;
	float AlphaSpeed = 2.f;
	bool bUp = false;
	bool bInteract = false;
};
