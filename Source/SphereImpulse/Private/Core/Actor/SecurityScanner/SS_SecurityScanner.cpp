
#include "Core/Actor/SecurityScanner/SS_SecurityScanner.h"
#include "Components/BoxComponent.h"
#include "Core/Interface/SS_Combatable.h"


ASS_SecurityScanner::ASS_SecurityScanner()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(GetRootComponent());

	SecurityScanner = CreateDefaultSubobject<UStaticMeshComponent>("SecurityScanner");
	ZoneScanner = CreateDefaultSubobject<UStaticMeshComponent>("ZoneScanner");
	ZoneScannerCollision = CreateDefaultSubobject<UStaticMeshComponent>("ZoneScannerCollision");

	SecurityScanner->SetupAttachment(BoxCollision);
	ZoneScanner->SetupAttachment(BoxCollision);
	ZoneScannerCollision->SetupAttachment(ZoneScanner);

	ZoneScanner->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ZoneScannerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneScannerCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	ZoneScannerCollision->SetVisibility(false);
	

}


void ASS_SecurityScanner::BeginPlay()
{
	Super::BeginPlay();
	
	ZoneMaterialMID = ZoneScanner->CreateDynamicMaterialInstance(0);
	ScannerMaterialMID = SecurityScanner->CreateDynamicMaterialInstance(1);
	SwitchingSecurityScannerAlarm(false);

	ZoneScannerCollision->OnComponentBeginOverlap.AddDynamic(this, &ASS_SecurityScanner::OnOverlapBegin);
	ZoneScannerCollision->OnComponentEndOverlap.AddDynamic(this, &ASS_SecurityScanner::OnOverlapEnd);
}

void ASS_SecurityScanner::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<USS_Combatable>())
	{
		bool bFound = true;
		for (auto& Actor : DetectedActors)
		{
			if (OtherActor == Actor)
			{
				bFound = false;
				break;
			}
		}
		if (bFound)
		{
			DetectedActors.AddUnique(OtherActor);
			SwitchingSecurityScannerAlarm(true);
		}
	}

}

void ASS_SecurityScanner::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	DetectedActors.Remove(OtherActor);

	if (DetectedActors.Num() == 0)
	{
		SwitchingSecurityScannerAlarm(false);

	}
}

void ASS_SecurityScanner::SwitchingSecurityScannerAlarm(bool bAlarmStatus)
{
	if (bCurrentAlarmStatus == bAlarmStatus) return;
	bCurrentAlarmStatus = bAlarmStatus;
	
	const FLinearColor TargetColor = bAlarmStatus ? AlarmColor : SafeColor;

	if (ZoneMaterialMID)
	{
		ZoneMaterialMID->SetVectorParameterValue(TEXT("AlarmColor"),TargetColor);
	}

	if (ScannerMaterialMID)
	{
		ScannerMaterialMID->SetVectorParameterValue(TEXT("AlarmColor"),TargetColor);
	}
	SwitchingSecurityScannerAlarmChange.Broadcast();
}
