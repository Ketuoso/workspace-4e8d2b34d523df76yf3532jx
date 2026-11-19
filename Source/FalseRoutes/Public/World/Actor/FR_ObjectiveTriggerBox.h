

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FR_ObjectiveTriggerBox.generated.h"

class UFR_ObjectiveSourceComponent;
class UBoxComponent;
class AFR_CharacterBase;

UCLASS()
class FALSEROUTES_API AFR_ObjectiveTriggerBox : public AActor
{
	GENERATED_BODY()
	
public:	

	AFR_ObjectiveTriggerBox();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<UFR_ObjectiveSourceComponent> ObjectiveSource;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objectives") FName SourceNodeId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objectives") bool bRevealWhenEntered = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objectives") bool bCompleteWhenEntered = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objectives") bool bTriggerOnce = true;

private:

	UFUNCTION() void HandleActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool TryActivateObjective(AFR_CharacterBase* Character);

	bool bHasTriggered = false;
};
