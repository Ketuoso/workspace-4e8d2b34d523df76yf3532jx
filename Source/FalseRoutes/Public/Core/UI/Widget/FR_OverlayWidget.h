

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FR_OverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class FALSEROUTES_API UFR_OverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent) void ShowInteractionPrompt(const FText& PromptText);
	UFUNCTION(BlueprintImplementableEvent) void HideInteractionPrompt();

};
