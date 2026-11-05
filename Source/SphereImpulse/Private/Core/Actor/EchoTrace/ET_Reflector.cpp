


#include "Core/Actor/EchoTrace/ET_Reflector.h"
#include "Components/BoxComponent.h"
#include "Core/Actor/EchoTrace/ET_LaserEmitter.h"


AET_Reflector::AET_Reflector()
{

	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	LaserReflector = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Laser Reflector"));
	ReflectionSurface = CreateDefaultSubobject<UBoxComponent>(TEXT("Reflection Surface"));

	RootComponent = BoxCollision;
	LaserReflector->SetupAttachment(BoxCollision);
	ReflectionSurface->SetupAttachment(BoxCollision);

	LaserReflector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserReflector->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}

void AET_Reflector::SetLaserReceiverState(ELaserReflectorState NewState)
{
	if (NewState == CurrentLaserReflectorState && !bInizialization) return;
	CurrentLaserReflectorState = NewState;
	bInizialization = false;

	switch (CurrentLaserReflectorState)
	{
	case ELaserReflectorState::Activeted:
	{
		bLaserActiveted = true;
		SetMaterialLaser();
		break;
	}
	case ELaserReflectorState::Deactiveted:
	{
		bLaserActiveted = false;
		SetMaterialLaser();
		break;
	}
	default:
		break;
	}
}

FVector AET_Reflector::GetSurfaceNormal() const
{
	return ReflectionSurface->GetRightVector();
}

void AET_Reflector::BeginPlay()
{
	Super::BeginPlay();

	LaserReflectorMaterialMID = LaserReflector->CreateDynamicMaterialInstance(1);
	SetLaserReceiverState(CurrentLaserReflectorState);
}

void AET_Reflector::SetLaserActive_Implementation(bool bActive)
{
	if (bActive)
	{
		if (CurrentLaserReflectorState == ELaserReflectorState::Activeted) return;
		SetLaserReceiverState(ELaserReflectorState::Activeted);
	}
	else
	{
		if (CurrentLaserReflectorState == ELaserReflectorState::Deactiveted) return;
		SetLaserReceiverState(ELaserReflectorState::Deactiveted);
	}
}

void AET_Reflector::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	AddActorWorldRotation(FRotator(0.f, AddAngle, 0.f));
}

void AET_Reflector::InteractRBM_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
}

bool AET_Reflector::CanInteract_Implementation(APlayerController* InteractingController) const
{
	return true;
}

void AET_Reflector::SetMaterialLaser()
{
	FLinearColor TargetColor = CurrentLaserReflectorState == ELaserReflectorState::Activeted ? GreenColor : RedColor;
	LaserReflectorMaterialMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
}


