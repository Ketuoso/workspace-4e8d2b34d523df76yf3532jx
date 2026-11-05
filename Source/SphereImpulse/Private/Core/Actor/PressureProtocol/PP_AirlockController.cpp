


#include "Core/Actor/PressureProtocol/PP_AirlockController.h"
#include "Core/Actor/PressureProtocol/PP_AirlockDoor.h"


APP_AirlockController::APP_AirlockController()
{
 	
	PrimaryActorTick.bCanEverTick = true;


	AirlockController = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AirlockController"));
	RootComponent = AirlockController;
}

void APP_AirlockController::SetAirlockState(EAirlockState NewState)
{
	if (CurrentAirlockState == NewState) return;

	CurrentAirlockState = NewState;

	SwitchingMID();

	switch (CurrentAirlockState)
	{
	case EAirlockState::Idle:
	{
		break;
	}
	case EAirlockState::ClosingDoors:
	{
		InnerDoor->SetBlockedDoor(true);
		OuterDoor->SetBlockedDoor(true);
		switch (CurrentAirlockDoorState)
		{
		case EAirlockDoorState::Inner:
		{
			InnerDoor->SetDoorState(EDoorState::Closing);
			break;
		}
		case EAirlockDoorState::Outer:
		{
			OuterDoor->SetDoorState(EDoorState::Closing);
			break;
		}
		case EAirlockDoorState::BotchClosed:
		{
			SetAirlockState(EAirlockState::EqualizingPressure);
			break;
		}
		default:
			break;
		}

		break;
	}
	case EAirlockState::EqualizingPressure:
	{
		if (CurrentAirlockDoorState == EAirlockDoorState::Outer)
		{
			TargetPressure = RoomPressure;
			bStartEqualizingPressure = true;

		}
		if (CurrentAirlockDoorState == EAirlockDoorState::Inner)
		{
			TargetPressure = AtmospheresPressure;
			bStartEqualizingPressure = true;

		}
		if (CurrentAirlockDoorState == EAirlockDoorState::BotchClosed)
		{
			if (CurrentDoorInteraction == InnerDoor)
			{
				TargetPressure = RoomPressure;
				bStartEqualizingPressure = true;
			}
			if (CurrentDoorInteraction == OuterDoor)
			{
				TargetPressure = AtmospheresPressure;
				bStartEqualizingPressure = true;
			}
		}

		break;
	}
	case EAirlockState::OpeningDoors:
	{
		bStartEqualizingPressure = false;

		switch (CurrentAirlockDoorState)
		{
		case EAirlockDoorState::Inner:
		{
			OuterDoor->SetDoorState(EDoorState::Opening);
			CurrentAirlockDoorState = EAirlockDoorState::Outer;
			break;
		}
		case EAirlockDoorState::Outer:
		{
			InnerDoor->SetDoorState(EDoorState::Opening);
			CurrentAirlockDoorState = EAirlockDoorState::Inner;
			break;
		}
		case EAirlockDoorState::BotchClosed:
		{
			CurrentDoorInteraction->SetDoorState(EDoorState::Opening);
			CurrentAirlockDoorState = CurrentDoorInteraction == InnerDoor ? EAirlockDoorState::Inner : EAirlockDoorState::Outer;
			break;
		}
		default:
			break;
		}

		break;
	}
	case EAirlockState::Ready:
	{
		InnerDoor->SetBlockedDoor(false);
		OuterDoor->SetBlockedDoor(false);

		SetAirlockState(EAirlockState::Idle);
		break;
	}
	case EAirlockState::Error:
	{
		break;
	}
	default:
		break;
	}
}


void APP_AirlockController::BeginPlay()
{
	Super::BeginPlay();
	
	AirlockControllerMID = AirlockController->CreateDynamicMaterialInstance(1);
	
	Initialize();
}

void APP_AirlockController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bStartEqualizingPressure)
	{
		if (TargetPressure == RoomPressure)
		{
			// Повышаем давление
			CurrentPressure += RatePressureChange * DeltaTime;
			CurrentPressure = FMath::Clamp(CurrentPressure, AtmospheresPressure, RoomPressure);
			UE_LOG(LogTemp,Warning,TEXT("Pressure = %f"),CurrentPressure)
			if (CurrentPressure == TargetPressure)
			{
				SetAirlockState(EAirlockState::OpeningDoors);
			}
		}
		if (TargetPressure == AtmospheresPressure)
		{
			// Понижаем давление
			CurrentPressure -= RatePressureChange * DeltaTime;
			CurrentPressure = FMath::Clamp(CurrentPressure, AtmospheresPressure, RoomPressure);
			UE_LOG(LogTemp, Warning, TEXT("Pressure = %f"), CurrentPressure)
			if (CurrentPressure == TargetPressure)
			{
				SetAirlockState(EAirlockState::OpeningDoors);
			}
		}
	}
}

void APP_AirlockController::DoorInteractionRequested(APP_AirlockDoor* DoorInteraction)
{
	if (CurrentAirlockState == EAirlockState::Idle)
	{
		CurrentDoorInteraction = DoorInteraction;
		SetAirlockState(EAirlockState::ClosingDoors);
	}
}

void APP_AirlockController::DoorStateChanged(EDoorState NewState)
{
	switch (CurrentAirlockState)
	{
	case EAirlockState::ClosingDoors:
	{
		if (NewState != EDoorState::Closed) break;
			
		SetAirlockState(EAirlockState::EqualizingPressure);

		break;
	}

	case EAirlockState::OpeningDoors:
	{
		if (NewState != EDoorState::Opened) break;

		SetAirlockState(EAirlockState::Ready);

		break;
	}

	default:
		break;
	}
}


void APP_AirlockController::SwitchingMID()
{
	const FLinearColor TargetColor = CurrentAirlockState != EAirlockState::Error ? GreenColor : RedColor;
	
	if (AirlockControllerMID)
	{
		AirlockControllerMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
	}
}

void APP_AirlockController::Initialize()
{
	if (!IsValid(InnerDoor) || !IsValid(OuterDoor)) return;

	bool bBlocked = CurrentAirlockState == EAirlockState::Error ? true : false;

	//Определить текущее положение дверей

	bool bInnerDoorOpened = InnerDoor->GetDoorState() == EDoorState::Opened;
	bool bOuterDoorOpened = OuterDoor->GetDoorState() == EDoorState::Opened;

	if (bInnerDoorOpened && !bOuterDoorOpened)
	{
		CurrentAirlockDoorState = EAirlockDoorState::Inner;
		CurrentPressure = RoomPressure;
	}
	if (!bInnerDoorOpened && bOuterDoorOpened)
	{
		CurrentAirlockDoorState = EAirlockDoorState::Outer;
		CurrentPressure = AtmospheresPressure;
	}
	if (!bInnerDoorOpened && !bOuterDoorOpened)
	{
		CurrentAirlockDoorState = EAirlockDoorState::BotchClosed;
	}
	if (bInnerDoorOpened && bOuterDoorOpened)
	{
		CurrentAirlockDoorState = EAirlockDoorState::Error;
		SetAirlockState(EAirlockState::Error);
		bBlocked = true;
	}


	InnerDoor->SetBlockedDoor(bBlocked);
	OuterDoor->SetBlockedDoor(bBlocked);
	SwitchingMID();
	InnerDoor->SetAirController(true);
	OuterDoor->SetAirController(true);



	InnerDoor->OnDoorInteractionRequestedDelegate.AddDynamic(this, &APP_AirlockController::DoorInteractionRequested);
	OuterDoor->OnDoorInteractionRequestedDelegate.AddDynamic(this, &APP_AirlockController::DoorInteractionRequested);
	InnerDoor->OnDoorStateChangedDelegate.AddDynamic(this, &APP_AirlockController::DoorStateChanged);
	OuterDoor->OnDoorStateChangedDelegate.AddDynamic(this, &APP_AirlockController::DoorStateChanged);
}


