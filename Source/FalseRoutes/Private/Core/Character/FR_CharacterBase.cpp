


#include "Core/Character/FR_CharacterBase.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Interface/FR_Interactable.h"
#include "Core/Components/FR_InteractionComponent.h"


AFR_CharacterBase::AFR_CharacterBase()
{

	PrimaryActorTick.bCanEverTick = false;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(GetArrowComponent());

	FR_InteractionComponent = CreateDefaultSubobject<UFR_InteractionComponent>(TEXT("InteractionComponent"));


	// Персонаж не должен автоматически копировать поворот контроллера.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Движение не должно перезаписывать поворот к курсору.
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// CameraBoom не должен поворачиваться вместе с персонажем.
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bUsePawnControlRotation = false;

	// Используй текущий угол своей камеры.
	// Это пример для типичной top-down камеры.
	CameraBoom->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	Camera->bUsePawnControlRotation = false;

}

void AFR_CharacterBase::Move(float InX, float InY)
{
	AddMovementInput(FVector(0.f, 1.f, 0.f), InX);
	AddMovementInput(FVector(1.f, 0.f, 0.f), InY);
}

void AFR_CharacterBase::LookAtRotation(FRotator& NewRotation)
{
	SetActorRotation(FRotator(0.f,NewRotation.Yaw,0.f));
}

void AFR_CharacterBase::Sprint(bool bIsSprint)
{
	EFR_MovementState NewState = bIsSprint ? EFR_MovementState::Sprinting : EFR_MovementState::Walking;
	SetMovementState(NewState);
}

void AFR_CharacterBase::MovementStateCrouch(bool bIsCrounch)
{
	EFR_MovementState NewState = bIsCrounch ? EFR_MovementState::Crouching : EFR_MovementState::Walking;
	if (!bIsCrounch)
	{
		UnCrouch();
	}
	SetMovementState(NewState);
}


void AFR_CharacterBase::SetMovementState(EFR_MovementState NewState)
{
	if (CurrentMovementState == NewState) return;
	CurrentMovementState = NewState;

	switch (CurrentMovementState)
	{
	case EFR_MovementState::Walking:
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;
	}
	case EFR_MovementState::Sprinting:
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		break;
	}
	case EFR_MovementState::Crouching:
	{
		Crouch();
		break;
	}
	default:
		break;
	}
}


void AFR_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void AFR_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFR_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



