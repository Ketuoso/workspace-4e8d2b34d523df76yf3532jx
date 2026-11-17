


#include "Core/GameState/FR_GameState.h"

void AFR_GameState::InternalSetMatchPhase(EFR_MatchPhase NewPhase)
{
	if (CurrentMatchPhase == NewPhase) return;

	EFR_MatchPhase OldPhase = CurrentMatchPhase;
	CurrentMatchPhase = NewPhase;

	MatchPhaseChangedDelegate.Broadcast(NewPhase, OldPhase);
}
