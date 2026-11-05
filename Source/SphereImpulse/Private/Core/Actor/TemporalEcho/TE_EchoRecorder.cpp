


#include "Core/Actor/TemporalEcho/TE_EchoRecorder.h"
#include "Core/Actor/SignalMemory/SM_SignalButton.h"
#include "GameFramework/Controller.h"
#include "Core/Actor/TemporalEcho/TE_EchoActor.h"


ATE_EchoRecorder::ATE_EchoRecorder()
{

	PrimaryActorTick.bCanEverTick = false;
	EchoRecorder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Terminal"));
	RootComponent = EchoRecorder;
}

void ATE_EchoRecorder::SetEchoRecorderState(EEchoRecorderState NewState)
{
	if (CurrentEchoRecordState == NewState)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Состояние не изменилось"));
		return ;
	}

	CurrentEchoRecordState = NewState;

	switch (CurrentEchoRecordState)
	{
	case EEchoRecorderState::Idle:
	{
		
		//UE_LOG(LogTemp, Warning, TEXT("Поиск записей"));
		//UE_LOG(LogTemp, Warning, TEXT("Найдено %d записей"), CurrentRecordCount);


		SetButtonsState(EButtonState::Activated);

		break;
	}
	case EEchoRecorderState::Recording:
	{
		//UE_LOG(LogTemp, Warning, TEXT("Запуск сохранения кадров."));
		
		
		SetButtonsState(EButtonState::Deactivated);
		StartRecording();

		break;
	}
	case EEchoRecorderState::Recorded:
	{
		// UE_LOG(LogTemp, Warning, TEXT("Запуск сохранения маршрута."));


		SetButtonsState(EButtonState::Deactivated);
		SaveRecord();


		// UE_LOG(LogTemp, Warning, TEXT("Маршрут сохранен."));


		if (bAutoPlayback)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Включен автозапуск  сохраненного маршрута."));


			SetEchoRecorderState(EEchoRecorderState::Playing);
			break;
		}

		SetEchoRecorderState(EEchoRecorderState::Finished);

		break;
	}
	case EEchoRecorderState::Playing:
	{
		//UE_LOG(LogTemp, Warning, TEXT("Запуск проигрывания маршрута."));


		SetButtonsState(EButtonState::Deactivated);

		PlayRecordStart();
		break;
	}
	case EEchoRecorderState::Finished:
	{
		// UE_LOG(LogTemp, Warning, TEXT("Запуск завершения цикла."));


		SetButtonsState(EButtonState::Activated);

		CurrentTimeRecord = 0.f;
		CurrentRecord.Empty();
		CurrenRecordController = nullptr;


		//UE_LOG(LogTemp, Warning, TEXT("Необходимые переменные сброшены"));


		SetEchoRecorderState(EEchoRecorderState::Idle);
		break;
	}
	default:
		break;
	}
}


void ATE_EchoRecorder::BeginPlay()
{
	Super::BeginPlay();
	
	SetEchoRecorderState(EEchoRecorderState::Idle); //Установка в Idle на случай если было другое состояние.

	if (!SignalButtonsInfo.IsEmpty())
	{
		for (auto& ButtonInfo : SignalButtonsInfo)
		{
			if (IsValid(ButtonInfo.Button))
			{
				
				ButtonInfo.Button->SetButtonState(EButtonState::Activated); // На случай когда игра начинается с Idle.
				ButtonInfo.Button->OnButtonInteractionRequestedDelegate.AddDynamic(this, &ATE_EchoRecorder::ButtonInteractionRequested);
			}
		}
	}

}


void ATE_EchoRecorder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATE_EchoRecorder::ButtonInteractionRequested(APlayerController* InteractingController, ASM_SignalButton* ButtonInteraction)
{
	CurrenRecordController = InteractingController;
	for (auto& ButtonInfo : SignalButtonsInfo)
	{

		if (ButtonInteraction == ButtonInfo.Button && ButtonInfo.ButtonType == EEchoButtonType::Play)
		{
			UE_LOG(LogTemp, Warning, TEXT("Нажата клавиша %s"), *StaticEnum<EEchoButtonType>()->GetNameStringByValue(static_cast<int64>(ButtonInfo.ButtonType)));
			SetEchoRecorderState(EEchoRecorderState::Playing);
			break;
		}
		else if (ButtonInteraction == ButtonInfo.Button && ButtonInfo.ButtonType == EEchoButtonType::Record)
		{
			UE_LOG(LogTemp, Warning, TEXT("Нажата клавиша %s"), *StaticEnum<EEchoButtonType>()->GetNameStringByValue(static_cast<int64>(ButtonInfo.ButtonType)));
			SetEchoRecorderState(EEchoRecorderState::Recording);
			break;
		}
	}
}


void ATE_EchoRecorder::SetButtonsState(EButtonState NewState)
{
	for (auto& ButtonInfo : SignalButtonsInfo)
	{
		if (IsValid(ButtonInfo.Button))
		{
			ButtonInfo.Button->SetButtonState(NewState);
		}
	}
}

void ATE_EchoRecorder::StartRecording()
{
	if (CurrenRecordController == nullptr)
	{
		// Контроллер не был найден, запуск отменен, возвращение в Idle.
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;
	}

	if (CurrentEchoRecordState != EEchoRecorderState::Recording)
	{
		// Во время запуска изменилось состояния, возвращение в Idle.
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Запуск записи."));

	// Запись первого кадра после запуска.
	RecordFrame();
	GetWorldTimerManager().SetTimer(TimerRecordFrame, this, &ATE_EchoRecorder::RecordFrame, RecordInterval, true);

}

void ATE_EchoRecorder::RecordFrame()
{

	// UE_LOG(LogTemp, Warning, TEXT("Текущий время записи %f"), CurrentTimeRecord);

	if (!IsValid(CurrenRecordController)) 
	{
		// Контроллер не был найден, запуск отменен, возвращение в Idle.
		GetWorldTimerManager().ClearTimer(TimerRecordFrame);
		SetEchoRecorderState(EEchoRecorderState::Idle);
	}

	APawn* PlayerPawn = CurrenRecordController->GetPawn();

	if (!IsValid(PlayerPawn))
	{
		// Игрок не был найден, запуск отменен, возвращение в Idle.
		GetWorldTimerManager().ClearTimer(TimerRecordFrame);
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;
	}

	// Создание временной структуры

	FTE_RecordedFrame CurrentRecordFrameInfo;
	CurrentRecordFrameInfo.EchoPosition = PlayerPawn->GetActorLocation();
	CurrentRecordFrameInfo.EchoRotation = PlayerPawn->GetActorRotation();
	CurrentRecordFrameInfo.RecordTime = CurrentTimeRecord;

	CurrentRecord.Add(CurrentRecordFrameInfo);

	// Увеличение интервала на 0.1

	CurrentTimeRecord += RecordInterval;

	if (CurrentTimeRecord > MaxRecordDuration)
	{
		// Текущий интервал > Максимального времени записи, запись завершена.

		GetWorldTimerManager().ClearTimer(TimerRecordFrame);
		SetEchoRecorderState(EEchoRecorderState::Recorded);
	}
}

void ATE_EchoRecorder::PlayRecordStart()
{
	// Берем первую запись и запускаем ее, после увеличиваем CurrentRecord на 1 ищем по ID

	if (Records.IsEmpty())
	{
		// если массив с записями пуст вовзращаемся в Idle
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;

	}

	if (CurrentRecordID > Records.Num())
	{
		// Если ID > Числа записей, позволяет вернуться к первой записи.
		CurrentRecordID = 1;
	}


	
	if (!IsValid(CurrenRecordController))
	{
		// Контроллер не был найден, запуск отменен, возвращение в Idle.
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;
	}
	APawn* PlayerPawn = CurrenRecordController->GetPawn();
	if (!IsValid(PlayerPawn))
	{
		// Игрок не был найден, запуск отменен, возвращение в Idle.
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;
	}
	USkeletalMeshComponent* PlayerMesh = PlayerPawn->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(PlayerMesh))
	{
		// Меш не был найден, запуск отменен, возвращение в Idle.
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;
	}

	// Получение записи 
	TArray<FTE_RecordedFrame> Frames = Records[CurrentRecordID - 1].RecordFrames;
	for (auto& Record : Records)
	{
		if (Record.RecordID == CurrentRecordID)
		{
			Frames = Record.RecordFrames;
		}
	}

	if (Frames.IsEmpty()) 
	{
		// Пустая запись, запуск отменен, возвращение в Idle.
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;
	}

	FVector Location = Frames[0].EchoPosition;
	FRotator Rotation = Frames[0].EchoRotation;
	FActorSpawnParameters SpawnParameters;

	ATE_EchoActor* SpawnEhcoActor = GetWorld()->SpawnActor<ATE_EchoActor>(Location,Rotation,SpawnParameters);
	if (!IsValid(SpawnEhcoActor))
	{
		// Не удалось создать SpawnEhcoActor, возвращениев Idle.
		SetEchoRecorderState(EEchoRecorderState::Idle);
		return;
	}
	SpawnEhcoActor->SetEchoAppearance(PlayerMesh);
	SpawnEhcoActor->InitializePlayback(Records[CurrentRecordID - 1].RecordFrames);
	SpawnEhcoActor->OnEchoActorFinishRoutesDelegate.AddDynamic(this, &ATE_EchoRecorder::PlayRecordEnd);
}

void ATE_EchoRecorder::PlayRecordEnd()
{

	// UE_LOG(LogTemp, Warning, TEXT("Маршрут завершен."));

	SetEchoRecorderState(EEchoRecorderState::Finished);
}

void ATE_EchoRecorder::SaveRecord()
{
	// Создание временой структуры записи
	
	if (CurrentRecord.IsEmpty())
	{
		// Продолжение кода в Recorder направит в Finish
		return;
	}

	FTE_Record CurrentRecordInfo;

	CurrentRecordInfo.RecordID = CurrentRecordCount + 1;
	CurrentRecordInfo.RecordFrames = CurrentRecord;
	Records.Add(CurrentRecordInfo);

	CurrentRecordCount = Records.Num();

	CurrentRecord.Empty();
}

