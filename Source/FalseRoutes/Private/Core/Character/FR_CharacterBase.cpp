


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
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"



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
	
	InizializationRenderTarget();

	GetWorldTimerManager().SetTimer(RefreshFOGTimer, this, &AFR_CharacterBase::RefreshFOG, 0.025f, true);
}


void AFR_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFR_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFR_CharacterBase::InizializationRenderTarget()
{
	if (!IsLocallyControlled()) return;

	FOG_RT = UKismetRenderingLibrary::CreateRenderTarget2D(this, FOG_WorldSpan, FOG_WorldSpan, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::Black, false, false);
	if (!FOG_Material) return;

	FOG_MID = UMaterialInstanceDynamic::Create(FOG_Material, this);
	Camera->AddOrUpdateBlendable(FOG_MID, 1.f);

	FOG_MID->SetTextureParameterValue(TEXT("VisionMask"), FOG_RT);
	FOG_MID->SetVectorParameterValue(TEXT("FogOrigin"), FVector4(FOG_OriginXY.X, FOG_OriginXY.Y, FOG_OriginXY.Y,0.f));
	FOG_MID->SetScalarParameterValue(TEXT("FogWorldSpan"), FOG_WorldSpan);
}

void AFR_CharacterBase::UpdateFOGOrigin()
{
	float HalfSpan = 0.f;

	HalfSpan = FOG_WorldSpan / 2.f;
	FOG_OriginXY.X = GetActorLocation().X - HalfSpan;
	FOG_OriginXY.Y = GetActorLocation().Y - HalfSpan;
}

void AFR_CharacterBase::UpdateFrontVisionRays()
{
	FrontVisionPoints.Empty();

	for (int32 Index = 0; Index < FrontRayCount + 1; Index++)
	{
		float CurrentRayAngle = (-ViewingAngle / 2) + (Index * ViewingAngle / FrontRayCount);
		const FVector Forward2D = GetActorForwardVector().GetSafeNormal2D();
		FVector RayDirection = (Forward2D.RotateAngleAxis(CurrentRayAngle, FVector::UpVector)) * ForwardVisiondDistance;

		FHitResult HitResult;
		FVector StartRay = FVector(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z + OffsetLocationToZ);
		FVector EndRay = StartRay + RayDirection;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(HitResult,StartRay, EndRay, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
		{
			FrontVisionPoints.Add(HitResult.Location);
		}
		else
		{
			FrontVisionPoints.Add(EndRay);
		}
	}
	BuildFrontFOGTriangles();
}

void AFR_CharacterBase::UpdateNearVisionRays()
{
	NearVisionPoints.Empty();

	for (int32 Index = 0; Index < NearRayCount; Index++)
	{
		float CurrentRayAngle = Index * (360.f / NearRayCount);
		FVector RayDirection = (FVector(1.f,0.f,0.f).RotateAngleAxis(CurrentRayAngle, FVector::UpVector)) * NearVisionRadius;
		
		FHitResult HitResult;
		FVector StartRay = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + OffsetLocationToZ);
		FVector EndRay = StartRay + RayDirection;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartRay, EndRay, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
		{
			NearVisionPoints.Add(HitResult.Location);
		}
		else
		{
			NearVisionPoints.Add(EndRay);
		}
	}
	DrawNearFOGMask();
}

void AFR_CharacterBase::BuildFrontFOGTriangles()
{
	FrontFOGTriangles.Empty();

	for (int32 Index = 0; Index < FrontRayCount; Index++)
	{
		FCanvasUVTri CurrentCanvasUVTri;
		CurrentCanvasUVTri.V0_Pos = WorldToFOGPixel(GetActorLocation());
		CurrentCanvasUVTri.V0_Color = FLinearColor::White;
		CurrentCanvasUVTri.V1_Pos = WorldToFOGPixel(FrontVisionPoints[Index]);
		CurrentCanvasUVTri.V1_Color = FLinearColor::White;
		CurrentCanvasUVTri.V2_Pos = WorldToFOGPixel(FrontVisionPoints[(Index + 1)]);
		CurrentCanvasUVTri.V2_Color = FLinearColor::White;

		FrontFOGTriangles.Add(CurrentCanvasUVTri);
	}
}

void AFR_CharacterBase::DrawNearFOGMask()
{
	if (NearVisionPoints.Num() != NearRayCount) return;
	NearFOGTriangles.Empty();

	for (int32 Index = 0; Index < NearRayCount; Index++)
	{
		FCanvasUVTri CurrentCanvasUVTri;
		CurrentCanvasUVTri.V0_Pos = WorldToFOGPixel(GetActorLocation());
		CurrentCanvasUVTri.V0_Color = FLinearColor::White;
		CurrentCanvasUVTri.V1_Pos = WorldToFOGPixel(NearVisionPoints[Index]);
		CurrentCanvasUVTri.V1_Color = FLinearColor::White;
		CurrentCanvasUVTri.V2_Pos = WorldToFOGPixel(NearVisionPoints[(Index + 1) % NearRayCount]);
		CurrentCanvasUVTri.V2_Color = FLinearColor::White;

		NearFOGTriangles.Add(CurrentCanvasUVTri);
	}
	
	
	if (FOG_RT && FOG_MID)
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(this, FOG_RT, FLinearColor::Black);
		UCanvas* OutCanvas;
		FVector2D OutSize;
		FDrawToRenderTargetContext OutContext;
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, FOG_RT, OutCanvas, OutSize, OutContext);
		OutCanvas->K2_DrawTriangle(nullptr, NearFOGTriangles);
		OutCanvas->K2_DrawTriangle(nullptr, FrontFOGTriangles);
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, OutContext);
		FOG_MID->SetVectorParameterValue(TEXT("FogOrigin"), FVector4(FOG_OriginXY.X, FOG_OriginXY.Y, 0.f, 1.f));
	}
	
}

FVector2D AFR_CharacterBase::WorldToFOGPixel(FVector WorldLocation)
{
	FVector2D PixelPosition;
	PixelPosition.X = ((WorldLocation.X - FOG_OriginXY.X) / FOG_WorldSpan) * 2048.f;
	PixelPosition.Y = ((WorldLocation.Y - FOG_OriginXY.Y) / FOG_WorldSpan) * 2048.f;
	return PixelPosition;
}

void AFR_CharacterBase::RefreshFOG()
{
	if (!FOG_RT) return;

	UpdateFOGOrigin();
	UpdateFrontVisionRays();
	UpdateNearVisionRays();
}



