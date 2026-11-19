

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/FR_Interactable.h"
#include "FR_Generator.generated.h"

class UFR_ObjectiveSourceComponent;

UENUM()
enum class EFR_GeneratorState : uint8
{
	Offline,
	Repairing,
	Repaired,
	Running
};

enum class EFR_GeneratorInteractionAction : uint8
{
	None,
	Inspect,
	Repair,
	Start
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Repair") EFR_GeneratorState GeneratorState = EFR_GeneratorState::Offline;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TObjectPtr<UFR_ObjectiveSourceComponent> ObjectiveSource;
private:

	EFR_GeneratorInteractionAction ResolveInteractionAction(AFR_CharacterBase* Interactor) const;

	void InspectGenerator(AFR_CharacterBase* Interactor);
	void StartRepair(AFR_CharacterBase* Interactor);
	void CompletedRepair();
	void CanceledRepair();

	bool TryStartGenerator(AFR_CharacterBase* Interactor);

	bool ResolveObjectiveId(FName SourceNodeId,FName& OutObjectiveId) const;

	FTimerHandle RepairTimerHandle;
	TWeakObjectPtr<AFR_CharacterBase> RepairInstigator;
};
