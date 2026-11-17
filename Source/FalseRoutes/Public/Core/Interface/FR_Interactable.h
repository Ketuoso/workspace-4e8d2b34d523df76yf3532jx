

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FR_Interactable.generated.h"

class AFR_CharacterBase;

UINTERFACE(MinimalAPI)
class UFR_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FALSEROUTES_API IFR_Interactable
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void Interact(AFR_CharacterBase* Interactor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) bool CanInteract(AFR_CharacterBase* Interactor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) FText GetInteractionText(AFR_CharacterBase* Interactor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void SetInteractionHighlighted(AFR_CharacterBase* Interactor, bool bIsHighlighted);
};
