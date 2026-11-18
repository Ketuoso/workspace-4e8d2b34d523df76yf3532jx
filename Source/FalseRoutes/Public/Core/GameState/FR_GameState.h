

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Data/FR_MatchPhase.h"
#include "FR_GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchPhaseChanged, EFR_MatchPhase, NewPhase, EFR_MatchPhase, OldPhase);

class UFR_ObjectiveComponent;

/**
 * 
 */
UCLASS()
class FALSEROUTES_API AFR_GameState : public AGameStateBase
{
	GENERATED_BODY()
	

public:

	AFR_GameState();

	UFUNCTION(BlueprintPure,BlueprintCallable, Category = "MatchPhase") EFR_MatchPhase GetMatchPhase() const { return CurrentMatchPhase; }
	UFUNCTION(BlueprintPure, Category = "Objectives") UFR_ObjectiveComponent* GetObjectiveComponent() const { return ObjectiveComponent; }
	void InternalSetMatchPhase(EFR_MatchPhase NewPhase);

	UPROPERTY(BlueprintAssignable, Category = "MatchPhase") FOnMatchPhaseChanged MatchPhaseChangedDelegate;


protected:



private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase", meta = (AllowPrivateAccess = "true")) EFR_MatchPhase CurrentMatchPhase = EFR_MatchPhase::Playing;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Objectives",meta = (AllowPrivateAccess = "true"))TObjectPtr<UFR_ObjectiveComponent> ObjectiveComponent;
};
