

#include "Core/Actor/MomentumLab/AML_KineticLauncher.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Actor/MomentumLab/AML_PhysicsBall.h"
#include "Components/SphereComponent.h"


AAML_KineticLauncher::AAML_KineticLauncher()
{

	PrimaryActorTick.bCanEverTick = false;

	KineticLauncherMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KineticLauncher"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));

	RootComponent = KineticLauncherMesh;
	BoxCollision->SetupAttachment(KineticLauncherMesh);

}


void AAML_KineticLauncher::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AAML_KineticLauncher::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AAML_KineticLauncher::OnOverlapEnd);
}

void AAML_KineticLauncher::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	LaunchBall();
}

bool AAML_KineticLauncher::CanInteract_Implementation(APlayerController* InteractingController) const
{
	if (CurrentBall == nullptr) return false;

	return true;
}

void AAML_KineticLauncher::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAML_PhysicsBall* Actor = Cast<AAML_PhysicsBall>(OtherActor))
	{
		CurrentBall = Actor;
		if (CurrentBall->GetSphereCollision()->GetPhysicsLinearVelocity().Length() > 10.f)
		{
			LaunchBall();
		}
	}
}

void AAML_KineticLauncher::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AAML_PhysicsBall* Actor = Cast<AAML_PhysicsBall>(OtherActor))
	{
		if (Actor != CurrentBall) return;
		CurrentBall = nullptr;
	}
}

void AAML_KineticLauncher::LaunchBall()
{
	if (!IsValid(CurrentBall)) return;
	FVector CurrentVelosity = CurrentBall->GetSphereCollision()->GetPhysicsLinearVelocity();
	FVector TargetVelosity = GetActorUpVector() * LaunchPower;

	FVector DeltaVelosity = TargetVelosity - CurrentVelosity;

	UE_LOG(LogTemp, Warning, TEXT("Target Velosity: %s"), *TargetVelosity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Current Velosity: %s"), *CurrentVelosity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Delta Velosity: %s"), *DeltaVelosity.ToString());
	CurrentBall->AddImpulseBall(DeltaVelosity);
}



