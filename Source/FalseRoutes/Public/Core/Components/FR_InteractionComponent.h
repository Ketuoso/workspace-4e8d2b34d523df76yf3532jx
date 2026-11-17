

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FR_InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTargetChanged, AActor*, NewTarget);

class AFR_CharacterBase;
class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FALSEROUTES_API UFR_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UFR_InteractionComponent();

	void TryInteract();
	void StopInteractionUpdates();

	FOnInteractionTargetChanged OnInteractionTargetChangedDelegate;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interaction") float InteractionRefreshInterval = 0.025f; //40 раз в секунду

private:

	void SetCurrentInteractableActor(AActor* NewActor);
	void StartInteractionUpdates();
	void RefreshInteraction();
	void FindBestInteractable();
	bool CanUpdateInteraction();
	void ClearInteractionTarget();

	AFR_CharacterBase* OwnerCharacter = nullptr;
	USphereComponent* InteractionSphere = nullptr;

	AActor* CurrentInteractableActor = nullptr;
	AActor* PreviousActor = nullptr;
	FTimerHandle RefreshInteractionTimer;
};
