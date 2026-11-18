

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FR_ObjectiveGraphGenerator.generated.h"

class UFR_ObjectiveGraphAsset;
struct FFR_ObjectiveGraphContext;
struct FFR_GeneratedObjectiveGraph;

/**
 * Строит runtime-граф задач по DataAsset и начальному сиду.
 *
 * Не обращается к миру, GameMode, GameState и акторам.
 */
UCLASS()
class FALSEROUTES_API UFR_ObjectiveGraphGenerator : public UObject
{
	GENERATED_BODY()

public:

	static bool GenerateGraph(const UFR_ObjectiveGraphAsset* GraphAsset, const FFR_ObjectiveGraphContext& Context, int32 Seed, FFR_GeneratedObjectiveGraph& OutGraph, FText& OutError);

	
};
