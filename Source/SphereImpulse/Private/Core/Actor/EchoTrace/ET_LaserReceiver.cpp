
#include "Core/Actor/EchoTrace/ET_LaserReceiver.h"
#include "Components/BoxComponent.h"
#include "Core/Actor/EchoTrace/ET_LaserEmitter.h"

AET_LaserReceiver::AET_LaserReceiver()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	LaserReceiver = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Laser Receiver"));

	RootComponent = BoxCollision;
	LaserReceiver->SetupAttachment(BoxCollision);

	LaserReceiver->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserReceiver->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}

void AET_LaserReceiver::SetLaserReceiverState(ELaserReceiverState NewState)
{
	if (NewState == CurrentLaserReceiverState && !bInizialization) return;
	CurrentLaserReceiverState = NewState;
	bInizialization = false;

	switch (CurrentLaserReceiverState)
	{
	case ELaserReceiverState::Activeted:
	{
		bLaserReceiverActiveted = true;
		SetMaterialLaserReceiver();
		LaserReceiverActivatedChangeDelegate.Broadcast(true);
		break;
	}
	case ELaserReceiverState::Deactiveted:
	{
		bLaserReceiverActiveted = false;
		SetMaterialLaserReceiver();
		LaserReceiverActivatedChangeDelegate.Broadcast(false);
		break;
	}
	default:
		break;
	}
}

void AET_LaserReceiver::BeginPlay()
{
	Super::BeginPlay();
	LaserReceiverMaterialMID = LaserReceiver->CreateDynamicMaterialInstance(1);
	SetLaserReceiverState(CurrentLaserReceiverState);
}


void AET_LaserReceiver::SetLaserActive_Implementation(bool bActive)
{
	if (bActive)
	{
		if (CurrentLaserReceiverState == ELaserReceiverState::Activeted) return;
		SetLaserReceiverState(ELaserReceiverState::Activeted);
	}
	else
	{
		if (CurrentLaserReceiverState == ELaserReceiverState::Deactiveted) return;
		SetLaserReceiverState(ELaserReceiverState::Deactiveted);
	}
}


void AET_LaserReceiver::SetMaterialLaserReceiver()
{
	FLinearColor TargetColor = CurrentLaserReceiverState == ELaserReceiverState::Activeted ? GreenColor : RedColor;
	LaserReceiverMaterialMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
}

