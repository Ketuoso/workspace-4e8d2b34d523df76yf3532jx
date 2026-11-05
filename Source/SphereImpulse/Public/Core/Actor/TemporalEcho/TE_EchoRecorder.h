

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TE_EchoRecorder.generated.h"

class APlayerController;

UENUM()
enum class EEchoRecorderState
{
	Idle, //Нет активной записи
	Recording, //Сохранение кадров
	Recorded, //Маршрут сохранён
	Playing, //Echo воспроизводит маршрут
	Finished //Воспроизведение завершено
};

UENUM(BlueprintType)
enum class EEchoButtonType : uint8
{
	Record,
	Play
};

USTRUCT()
struct FTE_RecordedFrame
{
	GENERATED_BODY()

	FVector EchoPosition = FVector::ZeroVector;
	FRotator EchoRotation = FRotator::ZeroRotator;
	float RecordTime;

};

USTRUCT()
struct FTE_Record
{
	GENERATED_BODY()

	int32 RecordID = 1;
	TArray<FTE_RecordedFrame> RecordFrames;

};

class ASM_SignalButton;

enum class EButtonState;

USTRUCT(BlueprintType)
struct FButtonEchoInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ASM_SignalButton> Button = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEchoButtonType ButtonType = EEchoButtonType::Record;
	
};

UCLASS()
class SPHEREIMPULSE_API ATE_EchoRecorder : public AActor
{
	GENERATED_BODY()
	
public:	

	ATE_EchoRecorder();

	void SetEchoRecorderState(EEchoRecorderState NewState);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EchoRecorder") TObjectPtr<UStaticMeshComponent> EchoRecorder;



private:

	UPROPERTY(EditAnywhere, Category = "EchoRecorder") TArray<FButtonEchoInfo> SignalButtonsInfo;
	EEchoRecorderState CurrentEchoRecordState = EEchoRecorderState::Idle;

	int32 CurrentRecordCount = 0;
	int32 CurrentRecordID = 1;
	APlayerController* CurrenRecordController;

	UPROPERTY(EditAnywhere, Category = "EchoRecorder") float PlaybackSpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "EchoRecorder") float MaxRecordDuration = 3.f;
	UPROPERTY(EditAnywhere, Category = "EchoRecorder") float RecordInterval = 0.1f;

	float CurrentTimeRecord = 0.f;

	UPROPERTY(EditAnywhere, Category = "EchoRecorder") bool bPlayback = false;
	UPROPERTY(EditAnywhere, Category = "EchoRecorder") bool bMultipleEchoes = false;
	UPROPERTY(EditAnywhere, Category = "EchoRecorder") bool bAutoPlayback = false;


	UFUNCTION() void ButtonInteractionRequested(APlayerController* InteractingController, ASM_SignalButton* ButtonInteraction);

	void SetButtonsState(EButtonState NewState);

	void StartRecording();
	void RecordFrame();

	void PlayRecordStart();
	UFUNCTION() void PlayRecordEnd();

	void SaveRecord();

	TArray<FTE_Record> Records;
	TArray<FTE_RecordedFrame> CurrentRecord;
	FTimerHandle TimerRecordFrame;

};
