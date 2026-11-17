


#include "Core/Controller/FR_PlayerController.h"
#include "Core/GameState/FR_GameState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Core/Character/FR_CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Core/Components/FR_InteractionComponent.h"
#include "Core/UI/HUD/FR_HUD.h"
#include "Core/Interface/FR_Interactable.h"

void AFR_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	InitializeInputMappings();

	UWorld* World = GetWorld();
	if (!World) return;
	AFR_GameState* FR_GameState = GetWorld()->GetGameState<AFR_GameState>();
	if (!FR_GameState) return;
	OwnerCharacter = Cast<AFR_CharacterBase>(GetPawn());
	if (!OwnerCharacter) return;

	FR_GameState->MatchPhaseChangedDelegate.AddUniqueDynamic(this, &AFR_PlayerController::HandleMatchPhaseChanged);
	OwnerCharacter->GetInteractionComponent()->OnInteractionTargetChangedDelegate.AddUniqueDynamic(this, &AFR_PlayerController::HandleInteractionTargetChanged);


	const EFR_MatchPhase CurrentPhase = FR_GameState->GetMatchPhase();
	HandleMatchPhaseChanged(CurrentPhase, CurrentPhase);

	GetWorldTimerManager().SetTimer(TimerLookAtRotation, this, &AFR_PlayerController::LookAtRotation, 0.025, true);
}



void AFR_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (UWorld* World = GetWorld())
	{
		if (AFR_GameState* FRGameState = World->GetGameState<AFR_GameState>())
		{
			FRGameState->MatchPhaseChangedDelegate.RemoveDynamic(this,&AFR_PlayerController::HandleMatchPhaseChanged);
		}

	}
	if (OwnerCharacter)
	{
		OwnerCharacter->GetInteractionComponent()->OnInteractionTargetChangedDelegate.RemoveDynamic(this, &AFR_PlayerController::HandleInteractionTargetChanged);
	}

	GetWorldTimerManager().ClearTimer(TimerLookAtRotation);

	Super::EndPlay(EndPlayReason);
}

void AFR_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedComponent->BindAction(FR_MoveAction, ETriggerEvent::Triggered, this, &AFR_PlayerController::Move);

	EnhancedComponent->BindAction(FR_InteractAction, ETriggerEvent::Started, this, &AFR_PlayerController::StartedInteract);

	EnhancedComponent->BindAction(FR_SprintAction, ETriggerEvent::Started, this, &AFR_PlayerController::StartedSprint);
	EnhancedComponent->BindAction(FR_SprintAction, ETriggerEvent::Completed, this, &AFR_PlayerController::CompletedSprint);

	EnhancedComponent->BindAction(FR_CrouchAction, ETriggerEvent::Started, this, &AFR_PlayerController::StartedCrouch);
	EnhancedComponent->BindAction(FR_CrouchAction, ETriggerEvent::Completed, this, &AFR_PlayerController::CompletedCrouch);
}

void AFR_PlayerController::HandleMatchPhaseChanged(EFR_MatchPhase NewPhase, EFR_MatchPhase OldPhase)
{
	switch (NewPhase)
	{
	case EFR_MatchPhase::Playing:
	case EFR_MatchPhase::EscapeAvailable:
	{
		break;
	}
	case EFR_MatchPhase::Won:
	case EFR_MatchPhase::Lost:
	{
		GetWorldTimerManager().ClearTimer(TimerLookAtRotation);
		DisableInput(this);

		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);

		OwnerCharacter->GetInteractionComponent()->StopInteractionUpdates();
		break;
	}
	default:
		break;
	}
}

void AFR_PlayerController::HandleInteractionTargetChanged(AActor* NewTarget)
{
	AFR_HUD* FR_HUD = Cast<AFR_HUD>(GetHUD());
	if (!FR_HUD) return;
	if (NewTarget)
	{
		if (IFR_Interactable::Execute_CanInteract(NewTarget, OwnerCharacter))
		{
			FText PromptText = IFR_Interactable::Execute_GetInteractionText(NewTarget, OwnerCharacter);
			FR_HUD->ShowInteractionPrompt(PromptText);
		}
		else
		{
			FR_HUD->HideInteractionPrompt();
		}
	}
	else
	{
		FR_HUD->HideInteractionPrompt();
	}

}

bool AFR_PlayerController::IsGameplayInputAllowed() const
{
	const UWorld* World = GetWorld();
	const AFR_GameState* FR_GameState = World ? World->GetGameState<AFR_GameState>() : nullptr;
	if (!FR_GameState) return false;
	const EFR_MatchPhase Phase = FR_GameState->GetMatchPhase();
	return Phase == EFR_MatchPhase::Playing || Phase == EFR_MatchPhase::EscapeAvailable;
}

void AFR_PlayerController::InitializeInputMappings()
{
	check(FR_InputContext);

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!InputSubsystem) return;
	InputSubsystem->AddMappingContext(FR_InputContext,0);

	SetShowMouseCursor(true);
	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);

}

void AFR_PlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (!IsGameplayInputAllowed()) return;
	if (!OwnerCharacter) return;

	const FVector2D InputAxis = InputActionValue.Get<FVector2D>();
	OwnerCharacter->Move(InputAxis.X, InputAxis.Y);
}

void AFR_PlayerController::StartedInteract()
{
	if (!IsGameplayInputAllowed()) return;
	if (!OwnerCharacter) return;

	OwnerCharacter->GetInteractionComponent()->TryInteract();
}

void AFR_PlayerController::StartedSprint()
{
	if (!IsGameplayInputAllowed()) return;
	if (!OwnerCharacter) return;

	OwnerCharacter->Sprint(true);
}

void AFR_PlayerController::CompletedSprint()
{
	if (!OwnerCharacter) return;

	OwnerCharacter->Sprint(false);
}

void AFR_PlayerController::StartedCrouch()
{
	if (!IsGameplayInputAllowed()) return;
	if (!OwnerCharacter) return;

	OwnerCharacter->MovementStateCrouch(true);
}

void AFR_PlayerController::CompletedCrouch()
{
	if (!OwnerCharacter) return;

	OwnerCharacter->MovementStateCrouch(false);
}

void AFR_PlayerController::LookAtRotation()
{
	if (!IsGameplayInputAllowed()) return;
	if (!OwnerCharacter) return;

	FVector RayOrigin;
	FVector RayDirection;

	const bool bDeprojected = DeprojectMousePositionToWorld(RayOrigin, RayDirection);
	if (!bDeprojected) return;

	FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	if (FMath::IsNearlyZero(RayDirection.Z)) return;

	const float DistanceToPlane = (OwnerLocation.Z - RayOrigin.Z) / RayDirection.Z;
	if (DistanceToPlane <= -0.f) return;

	const FVector CursorWorldLocation = RayOrigin + RayDirection * DistanceToPlane;

	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, CursorWorldLocation);
	OwnerCharacter->LookAtRotation(NewRotation);
}


