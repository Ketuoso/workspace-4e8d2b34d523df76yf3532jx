


#include "World/Actor/FR_HiddenActor.h"
#include "Core/Character/FR_CharacterRunner.h"


AFR_HiddenActor::AFR_HiddenActor()
{

	PrimaryActorTick.bCanEverTick = false;

}

void AFR_HiddenActor::Interact_Implementation(AFR_CharacterBase* Interactor)
{
	if (AFR_CharacterRunner* Runner = Cast<AFR_CharacterRunner>(Interactor))
	{
//
	}
}

bool AFR_HiddenActor::CanInteract_Implementation(AFR_CharacterBase* Interactor)
{
	return false;
}


void AFR_HiddenActor::BeginPlay()
{
	Super::BeginPlay();
	
}



