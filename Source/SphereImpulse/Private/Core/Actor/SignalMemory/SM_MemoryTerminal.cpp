


#include "Core/Actor/SignalMemory/SM_MemoryTerminal.h"
#include "Core/Actor/SignalMemory/SM_SignalButton.h"


ASM_MemoryTerminal::ASM_MemoryTerminal()
{

	PrimaryActorTick.bCanEverTick = false;

	Terminal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Terminal"));
	RootComponent = Terminal;

}

void ASM_MemoryTerminal::SetTerminalState(ETerminalState NewState)
{
	if (CurrentTerminalState == NewState) return;
	CurrentTerminalState = NewState;
	switch (CurrentTerminalState)
	{
	case ETerminalState::Idle:
	{
		UE_LOG(LogTemp, Warning, TEXT("Терминал отключен."));
		for (auto& ButtonInfo : TerminalButtonsInfo)
		{
			ButtonInfo.Button->SetButtonState(EButtonState::Deactivated);
		}
		break;
	}
	case ETerminalState::ShowingSequence:
	{
		UE_LOG(LogTemp, Warning, TEXT("Запущена демонстрация последовательности"));
		
		if (TerminalButtonsInfo.IsEmpty())
		{
			SetTerminalState(ETerminalState::Idle);
			return;
		}

		for (auto& ButtonInfo : TerminalButtonsInfo)
		{
			ButtonInfo.Button->SetButtonState(EButtonState::WaitInterract);
		}
		GetWorldTimerManager().ClearTimer(ShowSignalTimer_Start);
		CreateSequence();
		CurrentShowIndex = 0;
		GetWorldTimerManager().SetTimer(ShowSignalTimer_Start, this, &ASM_MemoryTerminal::StartShowSignalSequence, WaitToStartShowSignal, false);
		break;
	}
	case ETerminalState::WaitingForInput:
	{
		UE_LOG(LogTemp, Warning, TEXT("Ожидается ввод игрока"));

		for (auto& ButtonInfo : TerminalButtonsInfo)
		{
			ButtonInfo.Button->SetButtonState(EButtonState::Activated);
		}
		break;
	}
	case ETerminalState::RoundCompleted:
	{
		UE_LOG(LogTemp, Warning, TEXT("Раунд завершен успешно"));

		CurrentRound += 1;
		if (CurrentRound >= MaxRound)
		{
			SetTerminalState(ETerminalState::Completed);
			break;
		}
		
		SetTerminalState(ETerminalState::ShowingSequence);
		
		break;
	}
	case ETerminalState::Failed:
	{
		UE_LOG(LogTemp, Warning, TEXT("Была нажата не правильная последовательность"));
		for (auto& ButtonInfo : TerminalButtonsInfo)
		{
			ButtonInfo.Button->SetButtonState(EButtonState::Deactivated);
		}
		Sequence.Empty();
		CurrentRound = 0;
		SetTerminalState(ETerminalState::Idle);
		break;
	}
	case ETerminalState::Completed:
	{
		UE_LOG(LogTemp, Warning, TEXT("Все раунды пройдены."));

		for (auto& ButtonInfo : TerminalButtonsInfo)
		{
			ButtonInfo.Button->SetButtonState(EButtonState::Deactivated);
		}
		Sequence.Empty();
		CurrentRound = 0;
		SetTerminalState(ETerminalState::Idle);
		break;
	}
	default:
		break;
	}
}


void ASM_MemoryTerminal::BeginPlay()
{
	Super::BeginPlay();
	
	

	if (!SignalButtons.IsEmpty())
	{
		TerminalButtonsInfo.Empty();

		for (int32 Index = 0; Index < SignalButtons.Num(); Index++)
		{
			FButtonInfo ButtonInfo;
			ButtonInfo.Button = SignalButtons[Index];
			ButtonInfo.ButtonIndex = Index;
			if (IsValid(SignalButtons[Index]))
			{
				TerminalButtonsInfo.Add(ButtonInfo);
				SignalButtons[Index]->OnButtonInteractionRequestedFinishDelegate.AddDynamic(this, &ASM_MemoryTerminal::ButtonInteractionRequestedFinish);
				SignalButtons[Index]->OnButtonInteractionRequestedStartDelegate.AddDynamic(this, &ASM_MemoryTerminal::ButtonInteractionRequestedStart);
			}

			//UE_LOG(LogTemp, Warning, TEXT("Кнопка %s добавлена в массив"), *SignalButtons[Index]->GetName());
		}
	}
}


void ASM_MemoryTerminal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASM_MemoryTerminal::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	if (!CanInteract_Implementation(InteractingController)) return;

	SetTerminalState(ETerminalState::ShowingSequence);
}

bool ASM_MemoryTerminal::CanInteract_Implementation(APlayerController* InteractingController) const
{
	if (CurrentTerminalState != ETerminalState::Idle) return false;
	return true;
}

void ASM_MemoryTerminal::StartShowSignalSequence()
{
	GetWorldTimerManager().ClearTimer(ShowSignalTimer_Finish);
	if (CurrentTerminalState != ETerminalState::ShowingSequence)
	{
		Sequence.Empty();
		SetTerminalState(ETerminalState::Idle);
		return;
	}

	Sequence[CurrentShowIndex].Button->SwitchingButtonMID(true);
	GetWorldTimerManager().SetTimer(ShowSignalTimer_Finish, this, &ASM_MemoryTerminal::FinishShowSignalSequence, ShowSignalDuration, false);

}

void ASM_MemoryTerminal::FinishShowSignalSequence()
{
	GetWorldTimerManager().ClearTimer(ShowSignalTimer_Start);
	if (CurrentTerminalState != ETerminalState::ShowingSequence)
	{
		Sequence.Empty();
		SetTerminalState(ETerminalState::Idle);
		return;
	}
	Sequence[CurrentShowIndex].Button->SwitchingButtonMID(false);
	if (CurrentShowIndex == Sequence.Num() - 1)
	{
		SetTerminalState(ETerminalState::WaitingForInput);
	}
	else
	{
		CurrentShowIndex += 1;
		GetWorldTimerManager().SetTimer(ShowSignalTimer_Start, this, &ASM_MemoryTerminal::StartShowSignalSequence, WaitToStartShowSignal, false);
	}
}

void ASM_MemoryTerminal::ButtonInteractionRequestedFinish(ASM_SignalButton* ButtonInteraction)
{
	if (CurrentTerminalState != ETerminalState::WaitingForInput)
	{
		Sequence.Empty();
		SetTerminalState(ETerminalState::Idle);
		return;
	}

	FButtonInfo CurrentButtonInfo;
	for (auto& ButtonInfo : TerminalButtonsInfo)
	{
		ButtonInfo.Button->SetButtonState(EButtonState::Activated);
		if (ButtonInfo.Button == ButtonInteraction)
		{
			CurrentButtonInfo = ButtonInfo;
		}
	}


	if (Sequence[CurrentInputIndex].ButtonIndex == CurrentButtonInfo.ButtonIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("Нажата кнопка %d"), CurrentButtonInfo.ButtonIndex);
		if (CurrentInputIndex >= Sequence.Num() - 1)
		{
			SetTerminalState(ETerminalState::RoundCompleted);
		}
		else
		{
			CurrentInputIndex += 1;
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Нажата не правильная кнопка"));
		SetTerminalState(ETerminalState::Failed);
	}

}

void ASM_MemoryTerminal::ButtonInteractionRequestedStart()
{
	if (CurrentTerminalState != ETerminalState::WaitingForInput)
	{
		Sequence.Empty();
		SetTerminalState(ETerminalState::Idle);
		return;
	}
	for (auto& ButtonInfo : TerminalButtonsInfo)
	{
		ButtonInfo.Button->SetButtonState(EButtonState::WaitInterract);
	}
}

void ASM_MemoryTerminal::CreateSequence()
{
	int32 Index = FMath::RandRange(0, TerminalButtonsInfo.Num() - 1);
	FButtonInfo ButtonInfoSequence = TerminalButtonsInfo[Index];
	Sequence.Add(ButtonInfoSequence);

	for (auto& SequenceInfo : Sequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("В последовательности  находится кнопка ID %d"), SequenceInfo.ButtonIndex);
	}

	CurrentInputIndex = 0;
}

