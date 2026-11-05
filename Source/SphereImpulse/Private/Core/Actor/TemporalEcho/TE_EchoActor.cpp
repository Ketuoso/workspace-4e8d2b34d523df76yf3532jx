


#include "Core/Actor/TemporalEcho/TE_EchoActor.h"
#include "Core/Actor/TemporalEcho/TE_EchoRecorder.h"
#include "Components/BoxComponent.h"


ATE_EchoActor::ATE_EchoActor()
{

	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    EchoMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EchoMesh"));
    EchoMesh->SetupAttachment(RootComponent);

    EchoCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
    EchoCollision->SetupAttachment(EchoMesh);


}

void ATE_EchoActor::InitializePlayback(const TArray<FTE_RecordedFrame>& InFrames)
{
    PlaybackFrames = InFrames;

    CurrentFrameIndex = 0;
    PlaybackTime = 0.f;
    bIsPlaying = true;

}

void ATE_EchoActor::SetEchoAppearance(USkeletalMeshComponent* SourceMesh)
{
    if (!IsValid(SourceMesh)) return;

    EchoMesh->SetSkeletalMeshAsset(SourceMesh->GetSkeletalMeshAsset());

    EchoMesh->SetRelativeTransform(SourceMesh->GetRelativeTransform());

    for (int32 Index = 0; Index < SourceMesh->GetNumMaterials(); ++Index)
    {
        EchoMesh->SetMaterial(Index,SourceMesh->GetMaterial(Index));
    }
}


void ATE_EchoActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATE_EchoActor::Tick(float DeltaTime)
{
	if (bIsPlaying)
	{
        // Для интерполяции нужны минимум два кадра
        if (PlaybackFrames.Num() < 2)
        {
            bIsPlaying = false;
            DestroyActorAfterWait();
            return;
        }

        // Увеличиваем время воспроизведения
        PlaybackTime += DeltaTime;

        // Ищем два кадра, между которыми находится PlaybackTime
        while (CurrentFrameIndex + 1 < PlaybackFrames.Num() && PlaybackTime >= PlaybackFrames[CurrentFrameIndex + 1].RecordTime)
        {
            CurrentFrameIndex++;
        }

        // Достигли последнего кадра
        if (CurrentFrameIndex + 1 >= PlaybackFrames.Num())
        {
            const FTE_RecordedFrame& LastFrame = PlaybackFrames.Last();

            SetActorLocationAndRotation(LastFrame.EchoPosition,LastFrame.EchoRotation);
            bIsPlaying = false;
            OnEchoActorFinishRoutesDelegate.Broadcast();
            GetWorldTimerManager().SetTimer(WaitToDestroy, this, &ATE_EchoActor::DestroyActorAfterWait, 5.f, false);
            return;
        }

        // Получаем два соседних кадра
        const FTE_RecordedFrame& StartFrame = PlaybackFrames[CurrentFrameIndex];
        const FTE_RecordedFrame& EndFrame = PlaybackFrames[CurrentFrameIndex + 1];

        // Вычисляем длительность интервала
        float FrameDuration = EndFrame.RecordTime - StartFrame.RecordTime;

        if (FrameDuration <= 0.f)
        {
            bIsPlaying = false;
            DestroyActorAfterWait();
            return;
        }

        // Вычисляем коэффициент интерполяции
        float Alpha = (PlaybackTime - StartFrame.RecordTime) / FrameDuration;

        Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

        // Интерполируем позицию
        FVector NewLocation = FMath::Lerp(StartFrame.EchoPosition,EndFrame.EchoPosition,Alpha);

        // Интерполируем поворот
        FQuat NewRotation = FQuat::Slerp(StartFrame.EchoRotation.Quaternion(),EndFrame.EchoRotation.Quaternion(),Alpha);
        // Применяем результат
        SetActorLocationAndRotation(NewLocation,NewRotation.Rotator());
	}
}

void ATE_EchoActor::DestroyActorAfterWait()
{
    Destroy();
}


