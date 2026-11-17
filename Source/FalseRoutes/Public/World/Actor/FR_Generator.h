

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/FR_Interactable.h"
#include "FR_Generator.generated.h"


UENUM()
enum class EFR_GeneratorState : uint8
{
	Ready,
	Repairing,
	Completed
};

class AFR_CharacterBase;

UCLASS()
class FALSEROUTES_API AFR_Generator : public AActor, public IFR_Interactable
{
	GENERATED_BODY()
	
public:	

	AFR_Generator();

	virtual void Interact_Implementation(AFR_CharacterBase* Interactor) override;
	virtual bool CanInteract_Implementation(AFR_CharacterBase* Interactor) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Repair") int32 RepairCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Repair", meta = (ClampMin = "1")) int32 RequiredRepairs = 3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Repair", meta = (ClampMin = "0.0")) float RepairDuration = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Repair") EFR_GeneratorState GeneratorState = EFR_GeneratorState::Ready;

private:

	void StartRepair();
	void CompletedRepair();
	void CanceledRepair();

	FTimerHandle RepairTimerHandle;

};
