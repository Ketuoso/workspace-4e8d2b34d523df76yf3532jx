


#include "Core/GameMode/FR_LevelGameMode.h"
#include "Core/GameState/FR_GameState.h"
#include "Core/Controller/FR_PlayerController.h"

AFR_LevelGameMode::AFR_LevelGameMode()
{
	GameStateClass = AFR_GameState::StaticClass();
	PlayerControllerClass = AFR_PlayerController::StaticClass();
}

bool AFR_LevelGameMode::CanTransition(EFR_MatchPhase CurrentPhase, EFR_MatchPhase NewPhase) const
{
	if (CurrentPhase == NewPhase)
	{
		return false;
	}
	switch (CurrentPhase)
	{
	case EFR_MatchPhase::Playing:
	{
		if (NewPhase == EFR_MatchPhase::Won) return false;
		return true;
		break;
	}
	case EFR_MatchPhase::EscapeAvailable:
	{
		if (NewPhase == EFR_MatchPhase::Playing) return false;
		return true;
		break;
	}
	case EFR_MatchPhase::Won:
	{
		return false;
		break;
	}
	case EFR_MatchPhase::Lost:
	{
		return false;
		break;
	}
	default:
		break;
	}
	return false;
}

bool AFR_LevelGameMode::TryChangeMatchPhase(EFR_MatchPhase NewPhase)
{
	AFR_GameState* FR_GameState = GetGameState<AFR_GameState>();
	if (!IsValid(FR_GameState)) return false;
	const EFR_MatchPhase CurrentPhase = FR_GameState->GetMatchPhase();
	const bool bCanTransition = CanTransition(CurrentPhase, NewPhase);
	if (!bCanTransition)
	{
		return false;
	}
	FR_GameState->InternalSetMatchPhase(NewPhase);
	return true;
}

void AFR_LevelGameMode::RequestEscapeAvailable()
{
	UE_LOG(LogTemp, Warning, TEXT("Переход в фазу EscapeAvailable"));
	bool bSuccesses = TryChangeMatchPhase(EFR_MatchPhase::EscapeAvailable);
	if (!bSuccesses)
	{
		UE_LOG(LogTemp, Warning, TEXT("Переход в фазу EscapeAvailableне удалось"));
	}
}

void AFR_LevelGameMode::RequestWin()
{
	UE_LOG(LogTemp, Warning, TEXT("Переход в фазу Won"));
	bool bSuccesses = TryChangeMatchPhase(EFR_MatchPhase::Won);
	if (!bSuccesses)
	{
		UE_LOG(LogTemp, Warning, TEXT("Переход в фазу Won не удалось"));
	}
}

void AFR_LevelGameMode::RequestLose()
{
	UE_LOG(LogTemp, Warning, TEXT("Переход в фазу Lost"));
	bool bSuccesses = TryChangeMatchPhase(EFR_MatchPhase::Lost);
	if (!bSuccesses)
	{
		UE_LOG(LogTemp, Warning, TEXT("Переход в фазу Lost не удалось"));
	}
}
