// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WP_Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UWP_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPHEREIMPULSE_API IWP_Interactable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(APlayerController* InteractingController, FHitResult HitResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractRBM(APlayerController* InteractingController, FHitResult HitResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanInteract(APlayerController* InteractingController) const;
};
