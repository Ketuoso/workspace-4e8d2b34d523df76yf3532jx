

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

	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestEscapeAvailable();
	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestWin();
	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestLose();

private:

	bool TryChangeMatchPhase(EFR_MatchPhase NewPhase);
	bool CanTransition(EFR_MatchPhase CurrentPhase, EFR_MatchPhase NewPhase) const;
};
