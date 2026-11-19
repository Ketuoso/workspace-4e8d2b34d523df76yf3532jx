

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/FR_Interactable.h"
#include "FR_Door.generated.h"

class UFR_ObjectiveSourceComponent;
class AFR_CharacterBase;

UENUM()
enum class EFR_DoorState
{
	Closed,
	Open
};

UCLASS()
class FALSEROUTES_API AFR_Door : public AActor , public IFR_Interactable
{
	GENERATED_BODY()
	
public:	

	AFR_Door();



	virtual void Interact_Implementation(AFR_CharacterBase* Interactor) override;
	virtual bool CanInteract_Implementation(AFR_CharacterBase* Interactor) override;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<UFR_ObjectiveSourceComponent> ObjectiveSource;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") EFR_DoorState DoorState = EFR_DoorState::Closed;
private:

	bool ResolveObjectiveId(FName SourceNodeId, FName& OutObjectiveId) const;
	bool TryOpenDoor(AFR_CharacterBase* Interactor);
	void OpenDoor();

};
