

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Data/FR_MatchPhase.h"
#include "FR_GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchPhaseChanged, EFR_MatchPhase, NewPhase, EFR_MatchPhase, OldPhase);

/**
 * 
 */
UCLASS()
class FALSEROUTES_API AFR_GameState : public AGameStateBase
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintPure,BlueprintCallable, Category = "MatchPhase") EFR_MatchPhase GetMatchPhase() const { return CurrentMatchPhase; }

	void InternalSetMatchPhase(EFR_MatchPhase NewPhase);

	UPROPERTY(BlueprintAssignable, Category = "MatchPhase") FOnMatchPhaseChanged MatchPhaseChangedDelegate;


protected:



private:

	UPROPERTY(EditInstanceOnly, Category = "Phase") EFR_MatchPhase CurrentMatchPhase = EFR_MatchPhase::Playing;
};
