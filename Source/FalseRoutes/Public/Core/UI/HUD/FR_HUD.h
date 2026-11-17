

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FR_HUD.generated.h"

class UFR_OverlayWidget;

/**
 * 
 */
UCLASS()
class FALSEROUTES_API AFR_HUD : public AHUD
{
	GENERATED_BODY()
public:

	void ShowInteractionPrompt(const FText& PromptText);
	void HideInteractionPrompt();

protected:

	virtual void BeginPlay() override;



private:

	UPROPERTY()
	TObjectPtr<UFR_OverlayWidget>  OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFR_OverlayWidget> OverlayWidgetClass;

};
