


#include "Core/Actor/EchoTrace/ET_LaserEmitter.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Core/Interface/ET_LaserInterface.h"
#include "Core/Actor/EchoTrace/ET_Reflector.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"


AET_LaserEmitter::AET_LaserEmitter()
{

	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	LaserEmitter = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Laser Imitter"));
	LaserPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Laser Point"));
	ArrowLaser = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Laser"));

	RootComponent = BoxCollision;
	LaserEmitter->SetupAttachment(BoxCollision);
	LaserPoint->SetupAttachment(BoxCollision);
	ArrowLaser->SetupAttachment(LaserPoint);

	LaserEmitter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserEmitter->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	LaserSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LaserSpline"));
	LaserSpline->SetupAttachment(ArrowLaser);

	TestLaserMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("TestLaserMesh"));
	TestLaserMesh->SetupAttachment(LaserSpline);

	TestLaserMesh->SetForwardAxis(ESplineMeshAxis::X, false);
	TestLaserMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AET_LaserEmitter::SetLaserEmitterState(ELaserEmitterState NewState)
{
	if (NewState == CurrentLaserEmitterState && !bInizialization) return;
	CurrentLaserEmitterState = NewState;
	bInizialization = false;

	switch (CurrentLaserEmitterState)
	{
	case ELaserEmitterState::Activeted:
	{
		bLaserActiveted = true;
		SetMaterialLaser();
		break;
	}
	case ELaserEmitterState::Deactiveted:
	{
		bLaserActiveted = false;
		SetMaterialLaser();
		break;
	}
	default:
		break;
	}
}


void AET_LaserEmitter::BeginPlay()
{
	Super::BeginPlay();

	LaserEmitterMaterialMID = LaserEmitter->CreateDynamicMaterialInstance(1);

	LaserMeshSegments.Add(TestLaserMesh);
	TestLaserMesh->SetVisibility(false);

	// Устанавливаем начальное состояние излучателя
	SetLaserEmitterState(CurrentLaserEmitterState);

}

void AET_LaserEmitter::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	ELaserEmitterState NewState = CurrentLaserEmitterState == ELaserEmitterState::Activeted ? ELaserEmitterState::Deactiveted : ELaserEmitterState::Activeted;
	SetLaserEmitterState(NewState);
}

void AET_LaserEmitter::InteractRBM_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
}

bool AET_LaserEmitter::CanInteract_Implementation(APlayerController* InteractingController) const
{
	return true;
}

void AET_LaserEmitter::SetMaterialLaser()
{
	FLinearColor TargetColor = CurrentLaserEmitterState == ELaserEmitterState::Activeted ? GreenColor : RedColor;
	LaserEmitterMaterialMID->SetVectorParameterValue(TEXT("AlarmColor"), TargetColor);
}

void AET_LaserEmitter::UpdateLaser()
{
	// Создаем коллекцию акторов
	TSet<TWeakObjectPtr<AActor>> CurrentHitActors;
	LaserPoints.Reset();
	if (bLaserActiveted)
	{
		//Cтартовая позиция лазера
		FVector StartPoint = ArrowLaser->GetComponentLocation();
		LaserPoints.Add(StartPoint);
		//Текущее направление лазера
		FVector CurrentDirection = ArrowLaser->GetForwardVector().GetSafeNormal();
		//Дистанция лазера
		float RemainingDistance = MaxLaserDistance;
		//Количество лучей
		int32 ReflectionCount = 0;
		//Обьект который нужно игнорировать
		AActor* IgnoredActor = this;
		//Фиксированный отступ от плоскости
		constexpr float SurfaceOffset = 1.f;

		const int32 MaxTraceSteps = FMath::Max(64, MaxReflections + 16);

		for (int32 Step = 0; Step < MaxTraceSteps && RemainingDistance > KINDA_SMALL_NUMBER; ++Step)
		{
			// Конец текущего сегмента
			const FVector EndPoint = StartPoint + CurrentDirection * RemainingDistance;
			FHitResult HitResult;
			FCollisionQueryParams TraceParams;

			if (IsValid(IgnoredActor))
			{
				TraceParams.AddIgnoredActor(IgnoredActor);
			}
			const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, TraceParams);
			const FVector DrawEndPoint = bHit ? HitResult.ImpactPoint : EndPoint;
			LaserPoints.Add(DrawEndPoint);
			//DrawDebugLine(GetWorld(), StartPoint, DrawEndPoint, FColor::Blue, false, 0.2f);

			if (!bHit) break;

			AActor* HitActor = HitResult.GetActor();

			if (!IsValid(HitActor)) break;

			if (HitActor->Implements<UET_LaserInterface>())
			{
				CurrentHitActors.Add(HitActor);
			}

			const float TraveledDistance = FVector::Distance(StartPoint, HitResult.ImpactPoint);

			RemainingDistance = FMath::Max(0.f, RemainingDistance - TraveledDistance);

			AET_Reflector* Reflector = Cast<AET_Reflector>(HitActor);

			if (!Reflector) break;

			const FVector SurfaceNormal = Reflector->GetSurfaceNormal().GetSafeNormal();
			const float Dot = FMath::Abs(FVector::DotProduct(CurrentDirection, SurfaceNormal));

			// Луч параллелен отражающей плоскости
			if (Dot < 0.01f)
			{
				const float Offset = FMath::Min(SurfaceOffset, RemainingDistance);

				StartPoint = HitResult.ImpactPoint + CurrentDirection * Offset;
				RemainingDistance -= Offset;
				IgnoredActor = HitActor;

				continue;
			}

			// Достигли лимита отражений
			if (ReflectionCount >= MaxReflections) break;

			// Вычисляем новое направление
			const FVector ReflectedDirection = FMath::GetReflectionVector(CurrentDirection, SurfaceNormal).GetSafeNormal();
			const float Offset = FMath::Min(SurfaceOffset, RemainingDistance);

			StartPoint = HitResult.ImpactPoint + ReflectedDirection * Offset;
			RemainingDistance -= Offset;
			CurrentDirection = ReflectedDirection;
			IgnoredActor = HitActor;
			++ReflectionCount;
		}
	}

	for (const TWeakObjectPtr<AActor>& ActorPtr : PreviousHitActors)
	{
		if (!CurrentHitActors.Contains(ActorPtr))
		{
			if (AActor* Actor = ActorPtr.Get())
			{
				IET_LaserInterface::Execute_SetLaserActive(Actor, false);
			}
		}
	}

	for (const TWeakObjectPtr<AActor>& ActorPtr : CurrentHitActors)
	{
		if (!PreviousHitActors.Contains(ActorPtr))
		{
			if (AActor* Actor = ActorPtr.Get())
			{
				IET_LaserInterface::Execute_SetLaserActive(Actor, true);
			}
		}
	}

	PreviousHitActors = MoveTemp(CurrentHitActors);
	UpdateLaserVisual();
}

void AET_LaserEmitter::UpdateLaserVisual()
{
	LaserSpline->ClearSplinePoints(false);

	for (int32 i = 0; i < LaserPoints.Num(); ++i)
	{
		LaserSpline->AddSplinePoint(LaserPoints[i],ESplineCoordinateSpace::World,false);

		LaserSpline->SetSplinePointType(i,ESplinePointType::Linear,false);
	}

	LaserSpline->UpdateSpline();

	const int32 RequiredSegments = FMath::Max(0, LaserSpline->GetNumberOfSplinePoints() - 1);

	while (LaserMeshSegments.Num() < RequiredSegments)
	{
		USplineMeshComponent* NewSegment = NewObject<USplineMeshComponent>(this);

		NewSegment->SetupAttachment(LaserSpline);

		NewSegment->SetStaticMesh(TestLaserMesh->GetStaticMesh());
		NewSegment->SetMaterial(0, TestLaserMesh->GetMaterial(0));

		NewSegment->SetForwardAxis(ESplineMeshAxis::X, false);
		NewSegment->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		NewSegment->RegisterComponent();

		LaserMeshSegments.Add(NewSegment);
	}

	for (int32 i = 0; i < RequiredSegments; ++i)
	{
		USplineMeshComponent* Segment = LaserMeshSegments[i];

		const FVector Start = LaserSpline->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::Local);
		const FVector End = LaserSpline->GetLocationAtSplinePoint(i + 1,ESplineCoordinateSpace::Local);
		const FVector Tangent = End - Start;

		Segment->SetStartScale(FVector2D(0.05f, 0.05f),false);
		Segment->SetEndScale(FVector2D(0.05f, 0.05f),false);
		Segment->SetStartAndEnd(Start,Tangent,End,Tangent,true);
		Segment->SetVisibility(true);
	}

	for (int32 i = RequiredSegments; i < LaserMeshSegments.Num(); ++i)
	{
		LaserMeshSegments[i]->SetVisibility(false);
	}


}


void AET_LaserEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLaser();
}

