

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/Data/FR_MatchPhase.h"
#include "Core/Data/FR_ObjectiveGraphTypes.h"
#include "FR_LevelGameMode.generated.h"

class UFR_ObjectiveGraphAsset;


/**
 * 
 */
UCLASS()
class FALSEROUTES_API AFR_LevelGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:

	AFR_LevelGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Objectives") bool RevealObjective(FName ObjectiveId);
	UFUNCTION(BlueprintCallable, Category = "Objectives") bool TryAdvanceObjective(FName ObjectiveId, AActor* SourceActor);
	UFUNCTION(BlueprintCallable, Category = "Objectives") bool TryCompletedObjective(FName ObjectiveId, AActor* SourceActor);

	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestEscapeAvailable();
	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestWin();
	UFUNCTION(BlueprintCallable, Category = "MatchPhase") void RequestLose();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Objectives") FName EscapeAvailableObjectiveId = FName(TEXT("Generator.Start"));


	UPROPERTY(EditDefaultsOnly, Category = "Objectives|Generation") TObjectPtr<UFR_ObjectiveGraphAsset> ObjectiveGraphAsset = nullptr;
	// Первый сид диагностической серии.
	UPROPERTY(EditDefaultsOnly, Category = "Objectives|Generation") int32 ObjectiveSeed = 1000;
	// Сколько последовательных сидов проверить при запуске.
	UPROPERTY(EditDefaultsOnly,Category = "Objectives|Generation",meta = (ClampMin = "1", ClampMax = "100")) int32 DiagnosticSeedCount = 10;
	UPROPERTY(EditDefaultsOnly, Category = "Objectives|Generation") bool bUseProceduralObjectiveGraph = false;
	UPROPERTY(VisibleAnywhere, Category = "Objectives|Generation") FFR_GeneratedObjectiveGraph ActiveObjectiveGraph;

private:

	void RegisterTestObjectives();

	UFUNCTION() void HandleObjectiveChanged(FName ObjectiveId);

	bool TryChangeMatchPhase(EFR_MatchPhase NewPhase);
	bool CanTransition(EFR_MatchPhase CurrentPhase, EFR_MatchPhase NewPhase) const;


	bool InitializeProceduralObjectives();
	bool ApplyGeneratedObjectiveGraph(const FFR_GeneratedObjectiveGraph& Graph);

	void TestGenerateObjectiveGraph();
	void BuildObjectiveGraphContext(FFR_ObjectiveGraphContext& OutContext) const;
	void LogGeneratedObjectiveGraph(const FFR_GeneratedObjectiveGraph& Graph) const;
};
