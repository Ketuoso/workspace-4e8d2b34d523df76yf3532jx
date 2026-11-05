// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/WP_Interactable.h"
#include "PC_BasePowerDevice.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchingDeviceStatus, bool, bNewStatus);

class UCO_HackableComponent;

UENUM(BlueprintType)
enum class EPoweredState : uint8
{
	Powered,
	Unpowered
};

UCLASS()
class SPHEREIMPULSE_API APC_BasePowerDevice : public AActor, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	APC_BasePowerDevice();

	UFUNCTION(BlueprintCallable, BlueprintPure) float GetPowerCost() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) EPoweredState GetPoweredState() const;
	UFUNCTION() UCO_HackableComponent* GetHackableComponent() const { return HackableComponent; }

	UFUNCTION(BlueprintCallable) void SwitchingPowerDevice(bool bStatus);

	UFUNCTION() void HandleHackedStateChangeDelegate(bool NewStatus);

	UPROPERTY(BlueprintAssignable) FOnSwitchingDeviceStatus SwitchingPowerDeviceStatus;

protected:

	virtual void BeginPlay() override;
	virtual void SetMIDs();
	virtual void Execute_Interact(APlayerController* InteractingController);
	virtual void BindCallbacksToDependencies();
	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components") TObjectPtr<UCO_HackableComponent> HackableComponent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor SafeColor = FLinearColor::Green;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor AlarmColor = FLinearColor::Red;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials") TArray<UMaterialInstanceDynamic*> MIDs;
	UPROPERTY(BlueprintReadOnly) bool bCurrentStatus = false;
private:



	UPROPERTY(EditAnywhere, Category = "Powered") float PowerCost = 0.f;
	UPROPERTY(EditAnywhere, Category = "Powered") EPoweredState PoweredState = EPoweredState::Unpowered;




};
