


#include "World/Actor/FR_HiddenActor.h"
#include "Core/Character/FR_CharacterRunner.h"
#include "Components/ArrowComponent.h" 


AFR_HiddenActor::AFR_HiddenActor()
{

	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	HidePoint = CreateDefaultSubobject<USceneComponent>(TEXT("HidePoint"));
	ExitPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Exitpoint"));
	ForwardHideSpot = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardSpot"));
	HideSpot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HideSpot"));
	RootComponent = Root;
	HidePoint->SetupAttachment(Root);
	ExitPoint->SetupAttachment(Root);
	ForwardHideSpot->SetupAttachment(Root);
	HideSpot->SetupAttachment(Root);

}

void AFR_HiddenActor::Interact_Implementation(AFR_CharacterBase* Interactor)
{
	if (AFR_CharacterRunner* Runner = Cast<AFR_CharacterRunner>(Interactor))
	{
		if (!Runner->IsRunnerHidden())
		{
			Runner->EnterHiding(this,HidePoint->GetComponentLocation());
		}
		else
		{
			Runner->ExitHiding(ExitPoint->GetComponentLocation());
		}
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



