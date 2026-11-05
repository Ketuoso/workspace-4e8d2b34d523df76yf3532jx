

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ET_LaserInterface.generated.h"

class AET_LaserEmitter;

UINTERFACE(MinimalAPI)
class UET_LaserInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPHEREIMPULSE_API IET_LaserInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void SetLaserActive (bool bActive);
};
