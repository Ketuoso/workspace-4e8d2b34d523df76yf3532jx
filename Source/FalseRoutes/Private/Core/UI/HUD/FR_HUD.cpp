


#include "Core/UI/HUD/FR_HUD.h"
#include "Core/UI/Widget/FR_OverlayWidget.h"

void AFR_HUD::ShowInteractionPrompt(const FText& PromptText)
{
	if (!OverlayWidget) return;
	OverlayWidget->ShowInteractionPrompt(PromptText);
}

void AFR_HUD::HideInteractionPrompt()
{
	if (!OverlayWidget) return;
	OverlayWidget->HideInteractionPrompt();
}

void AFR_HUD::BeginPlay()
{
	if (OverlayWidgetClass)
	{
		UFR_OverlayWidget* Widget = CreateWidget<UFR_OverlayWidget>(GetWorld(), OverlayWidgetClass);
		OverlayWidget = Cast<UFR_OverlayWidget>(Widget);
		OverlayWidget->AddToViewport();
	}
}
