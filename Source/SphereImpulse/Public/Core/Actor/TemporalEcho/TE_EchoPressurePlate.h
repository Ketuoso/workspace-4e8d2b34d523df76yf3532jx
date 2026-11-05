

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TE_EchoPressurePlate.generated.h"

class UBoxComponent;

UCLASS()
class SPHEREIMPULSE_API ATE_EchoPressurePlate : public AActor
{
	GENERATED_BODY()
	
public:	

	ATE_EchoPressurePlate();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* EchoPlate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent* EchoCollision;

	UFUNCTION() void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor GreenColor = FLinearColor::Green;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Materials|Visual")FLinearColor RedColor = FLinearColor::Red;

private:

	TArray<ATE_EchoActor*> EchoActorsOverlap;

	UPROPERTY() UMaterialInstanceDynamic* EchoPlateMID;
	bool bIsActivated = false;
};
