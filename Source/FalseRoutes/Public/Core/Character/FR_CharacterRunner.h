

#pragma once

#include "CoreMinimal.h"
#include "Core/Character/FR_CharacterBase.h"
#include "FR_CharacterRunner.generated.h"


class AFR_HiddenActor;
/**
 * 
 */
UCLASS()
class FALSEROUTES_API AFR_CharacterRunner : public AFR_CharacterBase
{
	GENERATED_BODY()
	

public:

	bool IsRunnerHidden() const { return bHidden; }
	AActor* GetHiddenActor() { return CurrentHiddenSpot; }

	void EnterHiding(AFR_HiddenActor* HiddenSpot, FVector HidePointLocation);
	void ExitHiding(FVector ExitPointLocation);

private:

	AActor* CurrentHiddenSpot;
	bool bHidden = false;
};
