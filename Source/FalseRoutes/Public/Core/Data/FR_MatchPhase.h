#pragma once

#include "CoreMinimal.h"
#include "FR_MatchPhase.generated.h"

UENUM(BlueprintType)
enum class EFR_MatchPhase : uint8
{
	Playing,
	EscapeAvailable,
	Won,
	Lost
};