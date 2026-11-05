// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actor/CircuitOverride/CO_SecurityScanner.h"
#include "Components/BoxComponent.h"
#include "Core/Interface/SS_Combatable.h"
#include "Core/Components/CO_HackableComponent.h"


ACO_SecurityScanner::ACO_SecurityScanner()
{
	PrimaryActorTick.bCanEverTick = true;

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

void ACO_SecurityScanner::Tick(float DeltaTime)
{
	if (GetPoweredState() == EPoweredState::Powered && HackableComponent->GetHackedState() == EHackedState::Active)
	{
		float Step = AlphaSpeed * DeltaTime;
		if (!bUp)
		{
			Step *= -1.f;
		}
		ScanAlpha += Step;
		ScanAlpha = FMath::Clamp(ScanAlpha, 0.0f, 1.0f);
		float CurrentScanAngle = FMath::Lerp(MinAngleZoneScanner,MaxAngleZoneScanner,ScanAlpha);
		ZoneScanner->SetRelativeRotation(FRotator(CurrentScanAngle, 0.f , 0.f));
		if (ScanAlpha >= 1.0f) bUp = false;
		if (ScanAlpha <= 0.0f) bUp = true;
	}
}

void ACO_SecurityScanner::SetMIDs()
{
	MIDs.Empty();
	MIDs.AddUnique(SecurityScanner->CreateDynamicMaterialInstance(1));
	MIDs.AddUnique(ZoneScanner->CreateDynamicMaterialInstance(0));
}

void ACO_SecurityScanner::Execute_Interact(APlayerController* InteractingController)
{
}

void ACO_SecurityScanner::BindCallbacksToDependencies()
{
	ZoneScannerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACO_SecurityScanner::OnOverlapBegin);
	ZoneScannerCollision->OnComponentEndOverlap.AddDynamic(this, &ACO_SecurityScanner::OnOverlapEnd);
}

void ACO_SecurityScanner::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
			if (GetPoweredState() == EPoweredState::Powered && GetHackableComponent()->GetHackedState() != EHackedState::Hacked)
			{
				SwitchingScannerAlarm(false);
			}

		}
	}
}

void ACO_SecurityScanner::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	DetectedActors.Remove(OtherActor);

	if (DetectedActors.Num() == 0)
	{
		if (GetPoweredState() == EPoweredState::Powered && GetHackableComponent()->GetHackedState() != EHackedState::Hacked)
		{
			SwitchingScannerAlarm(true);
		}
	}
}

void ACO_SecurityScanner::SwitchingScannerAlarm(bool bStatus)
{
	if (bCurrentStatus == bStatus) return;
	bCurrentStatus = bStatus;

	const FLinearColor TargetColor = bStatus ? SafeColor : AlarmColor;

	if (MIDs.Num() > 0)
	{
		for (auto& MID : MIDs)
		{
			MID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
		}
	}
}
