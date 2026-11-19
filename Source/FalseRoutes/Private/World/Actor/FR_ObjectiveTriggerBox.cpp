


#include "World/Actor/FR_ObjectiveTriggerBox.h"
#include "Core/Components/FR_ObjectiveSourceComponent.h"
#include "Components/BoxComponent.h"
#include "Core/Character/FR_CharacterBase.h"
#include "Core/GameMode/FR_LevelGameMode.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Components/FR_ObjectiveComponent.h"



AFR_ObjectiveTriggerBox::AFR_ObjectiveTriggerBox()
{

	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	RootComponent = TriggerBox;

	ObjectiveSource = CreateDefaultSubobject<UFR_ObjectiveSourceComponent>(TEXT("ObjectiveSource"));

}

void AFR_ObjectiveTriggerBox::BeginPlay()
{
	Super::BeginPlay();
	
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AFR_ObjectiveTriggerBox::HandleActorBeginOverlap);
	}
	
}

void AFR_ObjectiveTriggerBox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &AFR_ObjectiveTriggerBox::HandleActorBeginOverlap);
	}
	

	Super::EndPlay(EndPlayReason);
}

void AFR_ObjectiveTriggerBox::HandleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggerOnce && bHasTriggered) return;
	AFR_CharacterBase* Character = Cast<AFR_CharacterBase>(OtherActor);
	if (!IsValid(Character) || !Character->IsPlayerControlled()) return;
	if (!TryActivateObjective(Character)) return;
	if (bTriggerOnce) bHasTriggered = true;

}

bool AFR_ObjectiveTriggerBox::TryActivateObjective(AFR_CharacterBase* Character)
{
	if (!IsValid(Character) || !ObjectiveSource || SourceNodeId.IsNone()) return false;
	
	UWorld* World = GetWorld();
	if (!World) return false;

	AFR_LevelGameMode* GameMode = World->GetAuthGameMode<AFR_LevelGameMode>();
	if (!GameMode) return false;

	FName RuntimeObjectiveId;
	const bool bHasBinding = ObjectiveSource->FindObjectiveId(SourceNodeId,RuntimeObjectiveId);

	if (!bHasBinding)
	{
		UE_LOG(LogTemp,Verbose,TEXT("Objective trigger has no binding. Actor=%s | Node=%s | SourceSlot=%s"),
			*GetName(),
			*SourceNodeId.ToString(),
			*ObjectiveSource->GetSourceSlot().ToString());
		return false;
	}

	bool bActionSucceeded = false;

	if (bRevealWhenEntered)
	{
		// false здесь может означать, что задача уже раскрыта.
		// Это не мешает последующему завершению.
		GameMode->RevealObjective(RuntimeObjectiveId);
		bActionSucceeded = true;
	}

	if (bCompleteWhenEntered)
	{
		const bool bCompleted = GameMode->TryCompletedObjective(RuntimeObjectiveId,Character);
		bool bAlreadyCompleted = false;

		if (!bCompleted)
		{
			AFR_GameState* FRGameState = World->GetGameState<AFR_GameState>();
			const UFR_ObjectiveComponent* Objectives = FRGameState ? FRGameState->GetObjectiveComponent() : nullptr;
			bAlreadyCompleted = Objectives && Objectives->IsObjectiveCompleted(RuntimeObjectiveId);
		}

		if (!bCompleted && !bAlreadyCompleted)
		{
			UE_LOG(LogTemp,Verbose,TEXT("Objective trigger could not complete objective. Objective=%s"),*RuntimeObjectiveId.ToString());
			// Например, задача скрыта или её зависимости ещё не выполнены.
			// Разрешаем повторное срабатывание при следующем входе.
			return false;
		}

		bActionSucceeded = true;
	}

	if (bActionSucceeded)
	{
		UE_LOG(LogTemp,Display,TEXT("Objective trigger activated. Actor=%s | Node=%s | Objective=%s"),
			*GetName(),
			*SourceNodeId.ToString(),
			*RuntimeObjectiveId.ToString());
	}

	return bActionSucceeded;
}



