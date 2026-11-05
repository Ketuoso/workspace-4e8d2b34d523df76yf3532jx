

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Interface/WP_Interactable.h"
#include "CG_CargoGrid.generated.h"

USTRUCT()
struct FCG_CargoGridInfo
{
	GENERATED_BODY()

	bool bOccupied = false;
	int32 CargoWeight = 0;
	int32 Row = 0;
	int32 Column = 0;

};

UCLASS()
class SPHEREIMPULSE_API ACG_CargoGrid : public AActor, public IWP_Interactable
{
	GENERATED_BODY()
	
public:	

	ACG_CargoGrid();

protected:

	virtual void BeginPlay() override;
	virtual void Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual void InteractRBM_Implementation(APlayerController* InteractingController, FHitResult HitResult) override;
	virtual bool CanInteract_Implementation(APlayerController* InteractingController) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generator") TObjectPtr<UStaticMeshComponent> Storage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generator") TObjectPtr<UInstancedStaticMeshComponent> Cargo;

private:

	UPROPERTY(EditAnywhere, Category = "Storage") int32 StorageRow = 4;
	UPROPERTY(EditAnywhere, Category = "Storage") int32 StorageColumn = 4;
	UPROPERTY(EditAnywhere, Category = "Storage") int32 Spacing = 190; // Обьект + расстояние между обьектам
	UPROPERTY(EditAnywhere, Category = "Storage") float OffsetCargo = 80.f; // Обьек /2 необходимо чтобы точки были по центру ячейки

	bool PlaceCargo(int32 inRow, int32 inColumn);
	bool RemoveCargo(int32 inRow, int32 inColumn);
	bool MoveCargo(int32 FromRow, int32 FromColumn, int32 ToRow, int32 ToColumn);
	void RebuildCargoInstances();

	int32 MaxWeight = 500;
	int32 AvailableWeight = 0;
	TArray<FCG_CargoGridInfo> CellsInfo;
	int32 CargoWeight = 50;
	bool bCargoSelected = false;
	int32 SelectedCargoIndex = INDEX_NONE;
};
