


#include "Core/Actor/PressureProtocol/PP_AirlockDoor.h"
#include "TimerManager.h"

APP_AirlockDoor::APP_AirlockDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	Doorway = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Doorway"));
	Door_Left = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorLeft"));
	Door_Right = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorRight"));
	DoorPanel_Front = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorPanelFront"));
	DoorPanel_Back = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorPanelBack"));

	RootComponent = Doorway;
	Door_Left->SetupAttachment(Doorway);
	Door_Right->SetupAttachment(Doorway);
	DoorPanel_Front->SetupAttachment(Doorway);
	DoorPanel_Back->SetupAttachment(Doorway);

}

void APP_AirlockDoor::SetBlockedDoor(bool NewbBlocked)
{
	bBlocked = NewbBlocked;
	UpdateDoorVisual();
}

void APP_AirlockDoor::SetDoorState(EDoorState NewState)
{
	if (CurrentDoorState == NewState) return;

	CurrentDoorState = NewState;
	
	UpdateDoorVisual();

	switch (CurrentDoorState)
	{
	case EDoorState::Closed:
	{
		OnDoorStateChangedDelegate.Broadcast(CurrentDoorState);
		bMoveDoor = false;
		break;
	}
	case EDoorState::Opening:
	{
		bMoveDoor = true;
		break;
	}
	case EDoorState::Opened:
	{
		OnDoorStateChangedDelegate.Broadcast(CurrentDoorState);
		bMoveDoor = false;
		break;
	}
	case EDoorState::Closing:
	{
		bMoveDoor = true;
		break;
	}
	default:
		break;
	}

}

void APP_AirlockDoor::BeginPlay()
{
	Super::BeginPlay();
	
	DoorwayMID = Doorway->CreateDynamicMaterialInstance(1);
	DoorPanel_BackMID = DoorPanel_Back->CreateDynamicMaterialInstance(1);
	DoorPanel_FrontMID = DoorPanel_Front->CreateDynamicMaterialInstance(1);

	DoorwayMID->SetVectorParameterValue(TEXT("AlarmColor"), GreenColor);

	UpdateDoorVisual();
	UpdateDoorStartPosition();
}

void APP_AirlockDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMoveDoor)
	{
		MoveDoor(DeltaTime);
	}
}


void APP_AirlockDoor::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	if(!CanInteract_Implementation(InteractingController)) return;
	if (bAirController)
	{
		OnDoorInteractionRequestedDelegate.Broadcast(this);
		return;
	}
	CurrentDoorState == EDoorState::Closed ? SetDoorState(EDoorState::Opening) : SetDoorState(EDoorState::Closing);

}

bool APP_AirlockDoor::CanInteract_Implementation(APlayerController* InteractingController) const
{
	if (bBlocked) return false;
	if (CurrentDoorState == EDoorState::Closing || CurrentDoorState == EDoorState::Opening) return false;
	return true;
}

void APP_AirlockDoor::MoveDoor(float DeltaTime)
{
	FVector DoorLeftLocation = Door_Left->GetRelativeLocation();
	FVector DoorRightLocation = Door_Right->GetRelativeLocation();
	
	float DoorTargetLocation = CurrentDoorState == EDoorState::Opening ? 130.f : 0.f;

	DoorLeftLocation.Y = FMath::FInterpTo(DoorLeftLocation.Y, -DoorTargetLocation, DeltaTime, DoorMoveSpeed);
	DoorRightLocation.Y = FMath::FInterpTo(DoorRightLocation.Y, DoorTargetLocation, DeltaTime, DoorMoveSpeed);

	Door_Left->SetRelativeLocation(DoorLeftLocation);
	Door_Right->SetRelativeLocation(DoorRightLocation);

	float DoorTargetEndLocation = CurrentDoorState == EDoorState::Opening ? 125.f : 5.f;
	bool bOpening = CurrentDoorState == EDoorState::Opening ? true : false;
	if (bOpening)
	{
		if (Door_Left->GetRelativeLocation().Y <= -DoorTargetEndLocation || Door_Right->GetRelativeLocation().Y >= DoorTargetEndLocation)
		{
			Door_Left->SetRelativeLocation(FVector(0.f, -DoorTargetLocation, 0.f));
			Door_Right->SetRelativeLocation(FVector(0.f, DoorTargetLocation, 0.f));
			SetDoorState(EDoorState::Opened);
		}
	}
	else
	{
		if (Door_Left->GetRelativeLocation().Y >= -DoorTargetEndLocation || Door_Right->GetRelativeLocation().Y <= DoorTargetEndLocation)
		{
			Door_Left->SetRelativeLocation(FVector(0.f, -DoorTargetLocation, 0.f));
			Door_Right->SetRelativeLocation(FVector(0.f, DoorTargetLocation, 0.f));
			SetDoorState(EDoorState::Closed);
		}
	}
	

}

void APP_AirlockDoor::SwitchingMID()
{

	bool bStatus = CurrentDoorState == EDoorState::Opened || CurrentDoorState == EDoorState::Closed ? true : false;
	const FLinearColor TargetColor = bStatus && !bBlocked ? GreenColor : RedColor;
	
	if (!IsValid(DoorPanel_BackMID) || !IsValid(DoorPanel_FrontMID)) return;

	DoorPanel_BackMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
	DoorPanel_FrontMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);

}

void APP_AirlockDoor::UpdateDoorStartPosition()
{
	float DoorTargetLocation = CurrentDoorState == EDoorState::Opened ? 130.f : 0.f;
	Door_Left->SetRelativeLocation(FVector(0.f, -DoorTargetLocation, 0.f));
	Door_Right->SetRelativeLocation(FVector(0.f, DoorTargetLocation, 0.f));
}

void APP_AirlockDoor::UpdateDoorVisual()
{
	SwitchingMID();
}

void APP_AirlockDoor::OpenDoor()
{
	SetDoorState(EDoorState::Opening);
}

void APP_AirlockDoor::CloseDoor()
{
	SetDoorState(EDoorState::Closing);
}

