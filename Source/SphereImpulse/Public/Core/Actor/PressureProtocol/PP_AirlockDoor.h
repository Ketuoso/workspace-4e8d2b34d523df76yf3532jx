

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/WP_Interactable.h"
#include "PP_AirlockDoor.generated.h"



UENUM()
enum class EDoorState : uint8
{
	Closed,
	Opening,
	Opened,
	Closing
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorInteractionRequested, APP_AirlockDoor*, ThisActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorStateChanged, EDoorState, NewState);

UCLASS()
class SPHEREIMPULSE_API APP_AirlockDoor : public AActor, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	APP_AirlockDoor();

	void SetBlockedDoor(bool NewbBlocked);
	void SetAirController(bool NewAirController) { bAirController = NewAirController; }
	void SetDoorState(EDoorState NewState);

	bool GetBlockedDoor() { return bBlocked; }
	EDoorState GetDoorState() { return CurrentDoorState; }

	void UpdateDoorVisual();
	void OpenDoor();
	void CloseDoor();

	FOnDoorInteractionRequested OnDoorInteractionRequestedDelegate;
	FOnDoorStateChanged OnDoorStateChangedDelegate;
	
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AirlockDoor") TObjectPtr<UStaticMeshComponent> Doorway;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AirlockDoor") TObjectPtr<UStaticMeshComponent> Door_Left;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AirlockDoor") TObjectPtr<UStaticMeshComponent> Door_Right;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AirlockDoor") TObjectPtr<UStaticMeshComponent> DoorPanel_Front;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AirlockDoor") TObjectPtr<UStaticMeshComponent> DoorPanel_Back;

private:

	UPROPERTY() UMaterialInstanceDynamic* DoorwayMID;
	UPROPERTY() UMaterialInstanceDynamic* DoorPanel_FrontMID;
	UPROPERTY() UMaterialInstanceDynamic* DoorPanel_BackMID;

	FLinearColor GreenColor = FLinearColor::Green;
	FLinearColor RedColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, Category = "AirlockDoor") EDoorState CurrentDoorState = EDoorState::Closed;
	bool bBlocked = false;
	bool bAirController = false;

	bool bMoveDoor = false;
	UPROPERTY(EditAnywhere) float DoorMoveSpeed = 0.5f;

	void MoveDoor(float DeltaTime);
	void SwitchingMID();
	void UpdateDoorStartPosition();

};
