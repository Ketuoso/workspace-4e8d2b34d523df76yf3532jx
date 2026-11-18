

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FR_ObjectiveSourceComponent.generated.h"

USTRUCT(BlueprintType)
struct FFR_ObjectiveSourceBinding
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") FName SourceNodeId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") FName RuntimeObjectiveId;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FALSEROUTES_API UFR_ObjectiveSourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFR_ObjectiveSourceComponent();

	UFUNCTION(BlueprintPure, Category = "Objectives") FName GetSourceSlot() const {return SourceSlot;}

	bool BindObjective(FName SourceNodeId,FName RuntimeObjectiveId);
	void ClearObjectiveBindings();

	UFUNCTION(BlueprintPure, Category = "Objectives")bool FindObjectiveId(FName SourceNodeId,FName& OutRuntimeObjectiveId) const;
	UFUNCTION(BlueprintPure, Category = "Objectives")bool HasBinding(FName SourceNodeId) const;

	const TArray<FFR_ObjectiveSourceBinding>& GetObjectiveBindings() const {return ObjectiveBindings;}

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objectives") FName SourceSlot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives") TArray<FFR_ObjectiveSourceBinding> ObjectiveBindings;


};
