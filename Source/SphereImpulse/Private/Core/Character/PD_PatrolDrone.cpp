


#include "Core/Character/PD_PatrolDrone.h"
#include "Core/Interface/SS_Combatable.h"
#include "Components/CapsuleComponent.h"
#include "Core/AIController/PD_PatrolDroneController.h"

APD_PatrolDrone::APD_PatrolDrone()
{

	PrimaryActorTick.bCanEverTick = true;

	PivotScanner = CreateDefaultSubobject<USceneComponent>(TEXT("PivotScanner"));
	Scanner = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scanner"));
	ZoneScanner = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneScanner"));
	CapsuleCollisionScanner = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollisionScanner"));

	PivotScanner->SetupAttachment(GetMesh());
	CapsuleCollisionScanner->SetupAttachment(PivotScanner);
	Scanner->SetupAttachment(PivotScanner);
	ZoneScanner->SetupAttachment(Scanner);

	Scanner->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Scanner->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	ZoneScanner->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ZoneScanner->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}


void APD_PatrolDrone::BeginPlay()
{
	Super::BeginPlay();
	
	MeshMaterialMID = GetMesh()->CreateDynamicMaterialInstance(1);
	ZoneMaterialMID = ZoneScanner->CreateDynamicMaterialInstance(0);
	ScannerMaterialMID = Scanner->CreateDynamicMaterialInstance(0);
	SwitchingSecurityScannerAlarm(SafeColor);

	CapsuleCollisionScanner->OnComponentBeginOverlap.AddDynamic(this, &APD_PatrolDrone::OnOverlapBegin);
	CapsuleCollisionScanner->OnComponentEndOverlap.AddDynamic(this, &APD_PatrolDrone::OnOverlapEnd);

}

void APD_PatrolDrone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<USS_Combatable>())
	{
		APD_PatrolDroneController* PatrolDroneController = Cast<APD_PatrolDroneController>(GetController());
		if (PatrolDroneController)
		{
			PatrolDroneController->SetPlayerInScanner(OtherActor,true);
		}
	}
}

void APD_PatrolDrone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<USS_Combatable>())
	{
		APD_PatrolDroneController* PatrolDroneController = Cast<APD_PatrolDroneController>(GetController());
		if (PatrolDroneController)
		{
			PatrolDroneController->SetPlayerInScanner(OtherActor, false);
		}
	}
}

void APD_PatrolDrone::SwitchingSecurityScannerAlarm(FLinearColor TargetColor)
{

	if (ZoneMaterialMID)
	{
		ZoneMaterialMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
	}

	if (ScannerMaterialMID)
	{
		ScannerMaterialMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
	}

	if (MeshMaterialMID)
	{
		MeshMaterialMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
	}
}


void APD_PatrolDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APD_PatrolDrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

