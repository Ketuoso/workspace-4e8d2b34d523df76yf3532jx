

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/Data/FR_ObjectiveTypes.h"
#include "FR_ObjectiveComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveChanged,FName, ObjectiveId);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FALSEROUTES_API UFR_ObjectiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UFR_ObjectiveComponent();

	UFUNCTION(BlueprintCallable, Category = "Objectives") bool  RegisterObjective(const FFR_ObjectiveDefinition Definition);

	UFUNCTION(BlueprintCallable, Category = "Objectives") bool RevealObjective(FName ObjectiveId);
	UFUNCTION(BlueprintCallable, Category = "Objectives") bool TryAdvanceObjective(FName ObjectiveId);
	UFUNCTION(BlueprintCallable, Category = "Objectives") bool TryCompletedObjective(FName ObjectiveId);
	UFUNCTION(BlueprintPure, Category = "Objectives") bool IsObjectiveCompleted(FName ObjectiveId) const;
	UFUNCTION(BlueprintPure, Category = "Objectives") EFR_ObjectiveStatus GetObjectiveStatus(FName ObjectiveId) const;

	UFUNCTION(BlueprintPure, Category = "Objectives") bool GetObjectiveDefinition( FName ObjectiveId,FFR_ObjectiveDefinition& OutDefinition) const;
	UFUNCTION(BlueprintPure, Category = "Objectives") bool GetObjectiveRuntimeState(FName ObjectiveId,FFR_ObjectiveRuntimeState& OutState) const;

	// Для построения списка в UI.
	UFUNCTION(BlueprintPure, Category = "Objectives")
	TArray<FName> GetVisibleObjectiveIds() const;


	UPROPERTY(BlueprintAssignable, Category = "Objectives") FOnObjectiveChanged OnObjectiveChangedDelegate;

protected:
	virtual void BeginPlay() override;


private:

	bool AreRequirementsMet(const FFR_ObjectiveDefinition& Definition) const;

	UPROPERTY() TMap<FName, FFR_ObjectiveDefinition> ObjectiveDefinitions;
	UPROPERTY() TMap<FName, FFR_ObjectiveRuntimeState> ObjectiveStates;
};
