

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TE_EchoActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEchoActorFinishRoutes);

struct FTE_RecordedFrame;
class UBoxComponent;

UCLASS()
class SPHEREIMPULSE_API ATE_EchoActor : public AActor
{
	GENERATED_BODY()
	
public:	

	ATE_EchoActor();

	TArray<FTE_RecordedFrame> PlaybackFrames;

	void InitializePlayback(const TArray<FTE_RecordedFrame>& InFrames);
	void SetEchoAppearance(USkeletalMeshComponent* SourceMesh);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* EchoMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent* EchoCollision;

	FOnEchoActorFinishRoutes OnEchoActorFinishRoutesDelegate;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:


	int32 CurrentFrameIndex = 0;
	float PlaybackTime = 0.f;
	bool bIsPlaying = false;

	UFUNCTION() void DestroyActorAfterWait();

	FTimerHandle WaitToDestroy;



};
