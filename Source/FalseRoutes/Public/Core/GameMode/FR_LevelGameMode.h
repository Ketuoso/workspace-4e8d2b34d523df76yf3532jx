

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/Data/FR_MatchPhase.h"
#include "FR_LevelGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FALSEROUTES_API AFR_LevelGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:

	AFR_LevelGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Objectives") bool RevealObjective(FName ObjectiveId);
	UFUNCTION(BlueprintCallable, Category = "Objectives") bool TryAdvanceObjective(FName ObjectiveId, AActor* SourceActor);
	UFUNCTION(BlueprintCallable, Category = "Objectives") bool TryCompletedObjective(FName ObjectiveId, AActor* SourceActor);

	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestEscapeAvailable();
	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestWin();
	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestLose();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Objectives") FName EscapeAvailableObjectiveId = FName(TEXT("Generator.Start"));

private:

	void RegisterTestObjectives();

	UFUNCTION() void HandleObjectiveChanged(FName ObjectiveId);

	bool TryChangeMatchPhase(EFR_MatchPhase NewPhase);
	bool CanTransition(EFR_MatchPhase CurrentPhase, EFR_MatchPhase NewPhase) const;
};
