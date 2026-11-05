


#include "Core/Actor/MomentumLab/AML_PhysicsBall.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


AAML_PhysicsBall::AAML_PhysicsBall()
{

	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	PhysicsBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhysicsBall"));

	RootComponent = SphereCollision;
	PhysicsBall->SetupAttachment(SphereCollision);

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	PhysicsBall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PhysicsBall->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);



}

void AAML_PhysicsBall::AddImpulseBall(FVector VectorImpulse)
{
	SphereCollision->SetSimulatePhysics(true);
	SphereCollision->SetEnableGravity(true);
	SphereCollision->AddImpulse(VectorImpulse * SphereCollision->GetMass());

}


void AAML_PhysicsBall::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->SetMassOverrideInKg(NAME_None, PhysicsBallMass, true);
	StartPositionBall = GetActorLocation();
	
}

void AAML_PhysicsBall::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	SphereCollision->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	SphereCollision->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	SphereCollision->SetSimulatePhysics(false);
	SphereCollision->SetEnableGravity(false);
	SetActorLocation(StartPositionBall);
}

bool AAML_PhysicsBall::CanInteract_Implementation(APlayerController* InteractingController) const
{
	return true;
}


