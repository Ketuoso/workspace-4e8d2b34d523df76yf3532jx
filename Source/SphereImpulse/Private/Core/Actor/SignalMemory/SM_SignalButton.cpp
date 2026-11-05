


#include "Core/Actor/SignalMemory/SM_SignalButton.h"


ASM_SignalButton::ASM_SignalButton()
{

	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button"));
	RootComponent = SceneRootComponent;
	Button->SetupAttachment(SceneRootComponent);

}

void ASM_SignalButton::SetButtonState(EButtonState NewState)
{
	if (CurrentButtonState == NewState) return;
	CurrentButtonState = NewState;

	switch (CurrentButtonState)
	{
	case EButtonState::Deactivated:
	{
		break;
	}
	case EButtonState::WaitInterract:
	{
		break;
	}
	case EButtonState::Activated:
	{
		break;
	}
	default:
		break;
	}
}


void ASM_SignalButton::BeginPlay()
{
	Super::BeginPlay();
	
	ButtonMID = Button->CreateDynamicMaterialInstance(0);

	if (ButtonMID)
	{
		ButtonMID->SetVectorParameterValue(TEXT("SequnceColor"), BaseColor);
	}

}


void ASM_SignalButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bInteract)
	{
		float Step = AlphaSpeed * DeltaTime;
		if (!bUp)
		{
			Step *= -1.f;
		}
		Alpha += Step;
		Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
		float CurrentSLocationZ = FMath::Lerp(MinLocationZ, MaxLocationZ, Alpha);
		Button->SetRelativeLocation(FVector(0.f, 0.f, CurrentSLocationZ));
		if (Alpha <= 0.0f) bUp = true;
		if (Alpha >= 1.0f)
		{
			bUp = false;
			bInteract = false;
			SwitchingButtonMID(false);
			OnButtonInteractionRequestedFinishDelegate.Broadcast(this);
			OnButtonInteractionRequestedDelegate.Broadcast(CurrentInteractingController,this);
			CurrentInteractingController = nullptr;
		}
	}
}

void ASM_SignalButton::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	if (!CanInteract_Implementation(InteractingController)) return;
	CurrentInteractingController = InteractingController;
	//UE_LOG(LogTemp, Warning, TEXT("Нажата кнопка %s "), *GetName());
	bInteract = true;
	SwitchingButtonMID(true);
	OnButtonInteractionRequestedStartDelegate.Broadcast();

	//Здесь необходимо запустить визуализацию опускания кнопки, поменять цвет на SequnceColor и только когда кнопка вернется обратно поменять цвет обратно на BаseColor и отправить Делегат что кнопка нажата

}

bool ASM_SignalButton::CanInteract_Implementation(APlayerController* InteractingController) const
{
	if (CurrentButtonState != EButtonState::Activated) return false;
	if (bInteract) return false;
	return true;
}

void ASM_SignalButton::SwitchingButtonMID(bool bSequence)
{
	const FLinearColor TargetColor = bSequence ? SequenceColor : BaseColor;

	if (!IsValid(ButtonMID)) return;

	ButtonMID->SetVectorParameterValue(TEXT("SequnceColor"), TargetColor);
}

