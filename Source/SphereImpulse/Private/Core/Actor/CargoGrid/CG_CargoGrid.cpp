


#include "Core/Actor/CargoGrid/CG_CargoGrid.h"
#include "Components/InstancedStaticMeshComponent.h"


ACG_CargoGrid::ACG_CargoGrid()
{

	PrimaryActorTick.bCanEverTick = false;

	Storage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Storage"));
	Cargo = CreateDefaultSubobject< UInstancedStaticMeshComponent>(TEXT("Cargo"));

	RootComponent = Storage;
	Cargo->SetupAttachment(Storage);

}


void ACG_CargoGrid::BeginPlay()
{
	Super::BeginPlay();
	
	Cargo->ClearInstances();
	CellsInfo.Empty();

	for (int32 Row = 0; Row < StorageRow; Row++)
	{
		for (int32 Column = 0; Column < StorageColumn; Column++)
		{
			FCG_CargoGridInfo CellInfo;
			CellInfo.Row = Row;
			CellInfo.Column = Column;
			CellsInfo.Add(CellInfo);
		}
	}
	AvailableWeight = MaxWeight;
}

void ACG_CargoGrid::Interact_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	FVector HitLocation = GetActorTransform().InverseTransformPosition(HitResult.ImpactPoint);
	int32 Row = FMath::RoundToInt(((HitLocation.X + (Spacing * (StorageRow - 1)*0.5f)) / Spacing));
	int32 Column = FMath::RoundToInt(((HitLocation.Y + (Spacing * (StorageColumn - 1) * 0.5f)) / Spacing));
	int32 Index = Row * StorageColumn + Column;
	if (Row < 0 || Row >= StorageRow) return;
	if (Column < 0  || Column >= StorageColumn) return;
	bool bOccupied = CellsInfo[Index].bOccupied;

	if (bCargoSelected && SelectedCargoIndex == Index) return;
	if (bCargoSelected && SelectedCargoIndex != Index)
	{
		if (!MoveCargo(CellsInfo[SelectedCargoIndex].Row, CellsInfo[SelectedCargoIndex].Column, CellsInfo[Index].Row, CellsInfo[Index].Column)) return;

		bCargoSelected = false;
		SelectedCargoIndex = INDEX_NONE;
		return;
	}

	!bOccupied ? PlaceCargo(Row, Column) : RemoveCargo(Row, Column);

}

void ACG_CargoGrid::InteractRBM_Implementation(APlayerController* InteractingController, FHitResult HitResult)
{
	
	FVector HitLocation = GetActorTransform().InverseTransformPosition(HitResult.ImpactPoint);
	int32 Row = FMath::RoundToInt(((HitLocation.X + (Spacing * (StorageRow - 1) * 0.5f)) / Spacing));
	int32 Column = FMath::RoundToInt(((HitLocation.Y + (Spacing * (StorageColumn - 1) * 0.5f)) / Spacing));
	int32 Index = Row * StorageColumn + Column;

	if (bCargoSelected && Index == SelectedCargoIndex)
	{
		bCargoSelected = false;
		SelectedCargoIndex = INDEX_NONE;
		return;
	}
	if (Row < 0 || Row >= StorageRow) return;
	if (Column < 0 || Column >= StorageColumn) return;
	bool bOccupied = CellsInfo[Index].bOccupied;

	if (bCargoSelected && Index != SelectedCargoIndex && bOccupied)
	{
		SelectedCargoIndex = Index;
		bCargoSelected = true;
		return;
	}
	if (bCargoSelected && Index != SelectedCargoIndex && !bOccupied)
	{
		bCargoSelected = false;
		SelectedCargoIndex = INDEX_NONE;
		return;
	}
	if (!bOccupied) return;

	SelectedCargoIndex = Index;
	bCargoSelected = true;
}

bool ACG_CargoGrid::CanInteract_Implementation(APlayerController* InteractingController) const
{
	return true;
}

bool ACG_CargoGrid::PlaceCargo(int32 inRow, int32 inColumn)
{
	if (inRow < 0 || inRow >= StorageRow) return false;
	if (inColumn < 0 || inColumn >= StorageColumn) return false;
	
	int32 Index = inRow * StorageColumn + inColumn;
	
	if (CellsInfo[Index].bOccupied) return false;
	if (AvailableWeight - CargoWeight < 0) return false;

	
	CellsInfo[Index].bOccupied = true;
	CellsInfo[Index].CargoWeight = CargoWeight;
	AvailableWeight -= CellsInfo[Index].CargoWeight;
	UE_LOG(LogTemp, Warning, TEXT("Occupied = %s"),CellsInfo[Index].bOccupied ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("AvailableWeight = %d"), AvailableWeight);
	RebuildCargoInstances();
	return true;
}

bool ACG_CargoGrid::RemoveCargo(int32 inRow, int32 inColumn)
{
	if (inRow < 0 || inRow >= StorageRow) return false;
	if (inColumn < 0 || inColumn >= StorageColumn) return false;

	int32 Index = inRow * StorageColumn + inColumn;

	if (!CellsInfo[Index].bOccupied) return false;
	
	AvailableWeight += CellsInfo[Index].CargoWeight;

	CellsInfo[Index].bOccupied = false;
	CellsInfo[Index].CargoWeight = 0;

	RebuildCargoInstances();
	UE_LOG(LogTemp, Warning, TEXT("Occupied = %s"), CellsInfo[Index].bOccupied ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("AvailableWeight = %d"), AvailableWeight);
	return true;
}

bool ACG_CargoGrid::MoveCargo(int32 FromRow, int32 FromColumn, int32 ToRow, int32 ToColumn)
{
	if (FromRow < 0 || FromRow >= StorageRow) return false;
	if (FromColumn < 0 || FromColumn >= StorageColumn) return false;
	if (ToRow < 0 || ToRow >= StorageRow) return false;
	if (ToColumn < 0 || ToColumn >= StorageColumn) return false;

	int32 FromIndex = FromRow * StorageColumn + FromColumn;
	int32 ToIndex = ToRow * StorageColumn + ToColumn;

	if (!CellsInfo[FromIndex].bOccupied) return false;
	if (CellsInfo[ToIndex].bOccupied) return false;

	CellsInfo[FromIndex].bOccupied = false;
	CellsInfo[ToIndex].bOccupied = true;

	CellsInfo[ToIndex].CargoWeight = CellsInfo[FromIndex].CargoWeight;
	CellsInfo[FromIndex].CargoWeight = 0;

	RebuildCargoInstances();

	return true;
}

void ACG_CargoGrid::RebuildCargoInstances()
{
	Cargo->ClearInstances();
	for (const auto& Cell : CellsInfo)
	{
		if (Cell.bOccupied)
		{
			FTransform InstanceTransorm = FTransform::Identity;
			FVector Location = InstanceTransorm.GetLocation();
			Location.X = (Cell.Row * Spacing) - (Spacing * (StorageRow - 1) * 0.5f);
			Location.Y = (Cell.Column * Spacing) - (Spacing * (StorageColumn - 1) * 0.5f);
			InstanceTransorm.SetLocation(Location);
			Cargo->AddInstance(InstanceTransorm);
		}
	}
}




