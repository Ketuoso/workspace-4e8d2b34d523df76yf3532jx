

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Repair", meta = (ClampMin = "0.0")) float RepairDuration = 1.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Repair") EFR_GeneratorState GeneratorState = EFR_GeneratorState::Ready;

private:

	void StartRepair(AFR_CharacterBase* Interactor);
	void CompletedRepair();
	void CanceledRepair();

	FTimerHandle RepairTimerHandle;
	TWeakObjectPtr<AFR_CharacterBase> RepairInstigator;
};
