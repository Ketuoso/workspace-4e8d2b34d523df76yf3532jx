

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/WP_Interactable.h"
#include "SM_MemoryTerminal.generated.h"

UENUM()
enum class ETerminalState
{
	Idle, //Ожидание запуска
	ShowingSequence, //Демонстрация последовательности
	WaitingForInput, //Ожидание действий игрока
	RoundCompleted, //Раунд успешно пройден
	Failed, //Игрок допустил ошибку
	Completed //Все раунды пройдены
};

class ASM_SignalButton;

USTRUCT()
struct FButtonInfo
{
	GENERATED_BODY()

	ASM_SignalButton* Button = nullptr;
	int32 ButtonIndex = INDEX_NONE;

};

UCLASS()
class SPHEREIMPULSE_API ASM_MemoryTerminal : public AActor, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	ASM_MemoryTerminal();

	void SetTerminalState(ETerminalState NewState);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Terminal") TObjectPtr<UStaticMeshComponent> Terminal;


private:


	UPROPERTY(EditAnywhere, Category = "Terminal") TArray<ASM_SignalButton*> SignalButtons;
	UPROPERTY(EditAnywhere, Category = "Terminal") int32 MaxRound = 3;
	UPROPERTY(EditAnywhere, Category = "Terminal") float ShowSignalDuration = 1.f;
	UPROPERTY(EditAnywhere, Category = "Terminal") float WaitToStartShowSignal = 1.f;

	ETerminalState CurrentTerminalState = ETerminalState::Idle;
	TArray<FButtonInfo> TerminalButtonsInfo;
	TArray<FButtonInfo> Sequence;

	int32 CurrentRound = 0;
	int32 CurrentInputIndex = 0;
	int32 CurrentShowIndex = 0;

	FTimerHandle ShowSignalTimer_Start;
	FTimerHandle ShowSignalTimer_Finish;


	void StartShowSignalSequence();
	void FinishShowSignalSequence();

	UFUNCTION() void ButtonInteractionRequestedFinish(ASM_SignalButton* ButtonInteraction);
	UFUNCTION() void ButtonInteractionRequestedStart();
	void CreateSequence();
};
