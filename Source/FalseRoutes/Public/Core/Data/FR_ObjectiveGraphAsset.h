

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/Data/FR_ObjectiveGraphTypes.h"
#include "FR_ObjectiveGraphAsset.generated.h"

class UCurveFloat;

/**
 * 
 */
UCLASS()
class FALSEROUTES_API UFR_ObjectiveGraphAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/** Корневой узел графа. Обычно это главная задача Escape.Warehouse. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Graph") FName RootNodeid;

	/** Узел, после выполнения которого GameMode должен перевести матч в EscapeAvailable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Graph") FName EscapeAvailableNodeId;

	/** Узлы, из которые генератор может строить граф, NodeId должен быть уникальным */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Graph") TArray<FFR_ObjectiveNodeTemplate> NodeTemplates;


	/** Максимальная глубина зависимостей от корневого узла. Защищает граф от чрезмерно длинных цепочек. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Graph", meta = (ClampMin = "1", ClampMax = "32")) int32 MaxDepth = 6;

	/** Максимальное количество задач в сгенерированном графе. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Graph", meta = (ClampMin = "1", ClampMax = "100")) int32 MaxGeneratedObjectives = 12;

	/** Количество попыток построить проходимый граф до перехода к резервному варианту. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Graph", meta = (ClampMin = "1", ClampMax = "100")) int32 MaxGenerationAttempts = 16;

	/** X - Глубина узла. Y - Базовая вероятность того, что узел выполнен */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Generation|Initial Satisfaction") TObjectPtr<UCurveFloat> InitialSatisfactionChanceCurve = nullptr;

	/** Верхняя граница итоговой вероятности. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Generation|Initial Satisfaction", meta = (ClampMin = "0.0", ClampMax = "1.0")) float MaxSimultaneousRenderTargets = 0.65f;
};
