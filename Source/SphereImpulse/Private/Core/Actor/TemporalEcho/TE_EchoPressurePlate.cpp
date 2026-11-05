


#include "Core/Actor/TemporalEcho/TE_EchoPressurePlate.h"
#include "Components/BoxComponent.h"
#include "Core/Actor/TemporalEcho/TE_EchoActor.h"


ATE_EchoPressurePlate::ATE_EchoPressurePlate()
{

	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	EchoCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	EchoPlate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Echo Plate"));

	EchoPlate->SetupAttachment(RootComponent);
	EchoCollision->SetupAttachment(EchoPlate);

}


void ATE_EchoPressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
	EchoPlateMID = EchoPlate->CreateDynamicMaterialInstance(1);

	EchoCollision->OnComponentBeginOverlap.AddDynamic(this, &ATE_EchoPressurePlate::OnOverlapBegin);
	EchoCollision->OnComponentEndOverlap.AddDynamic(this, &ATE_EchoPressurePlate::OnOverlapEnd);

}


void ATE_EchoPressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATE_EchoPressurePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ATE_EchoActor* EchoActor = Cast<ATE_EchoActor>(OtherActor))
	{
		bIsActivated = true;
		EchoActorsOverlap.AddUnique(OtherActor);
		EchoPlateMID->SetVectorParameterValue(TEXT("AlarmColor"), GreenColor);

	}
}

void ATE_EchoPressurePlate::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EchoActorsOverlap.Remove(OtherActor);
	if (EchoActorsOverlap.Num() <= 0)
	{
		bIsActivated = false;
		EchoPlateMID->SetVectorParameterValue(TEXT("AlarmColor"), RedColor);
		return;
	}

}

