


#include "Core/Character/WP_ServiceRobot.h"
#include "Core/AIController/WP_RobotController.h"


AWP_ServiceRobot::AWP_ServiceRobot()
{

	PrimaryActorTick.bCanEverTick = false;

	PivotRoller = CreateDefaultSubobject<USceneComponent>(TEXT("PivotRoller"));
	Roller = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Roller"));
	
	PivotRoller->SetupAttachment(GetMesh());
	Roller->SetupAttachment(PivotRoller);

}


void AWP_ServiceRobot::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWP_ServiceRobot::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	AWP_RobotController* RobotController = Cast<AWP_RobotController>(GetController());

	if (!RobotController) return;
	RobotController->TryInteract();
}

bool AWP_ServiceRobot::CanInteract_Implementation(APlayerController* InteractingController) const
{
	return true;
}




