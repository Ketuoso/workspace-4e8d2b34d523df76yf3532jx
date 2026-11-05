


#include "Core/Actor/Generator/PC_Generator.h"
#include "Components/InstancedStaticMeshComponent.h"


APC_Generator::APC_Generator()
{

	PrimaryActorTick.bCanEverTick = true;

	Generator = CreateDefaultSubobject<UStaticMeshComponent>("Generator");
	RootComponent = Generator;

	CellsMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("CellsMesh");
	CellsMesh->SetupAttachment(Generator);

}

float APC_Generator::GetMaxEnergy() const
{
    return MaxEnergy;
}

float APC_Generator::GetCurrentEnergy() const
{
    return CurrentEnergy;
}

void APC_Generator::UpdateEnergy(float EnergyAmount, bool bConsumeEnergy)
{
    CurrentEnergy = bConsumeEnergy ? CurrentEnergy - EnergyAmount : CurrentEnergy + EnergyAmount;
}



bool APC_Generator::CanProvideEnergy(float RequiredEnergy)
{
    if (CurrentEnergy - RequiredEnergy >= 0) return true;
    return false;
}


void APC_Generator::BeginPlay()
{
	Super::BeginPlay();
	
    CellsMovementData.Empty();
    const int32 CellsCount = CellsMesh->GetInstanceCount();
    
    for (int32 Index = 0; Index < CellsCount; Index++)
    {
        FTransform CellTransform;

        if (!CellsMesh->GetInstanceTransform(Index, CellTransform, false)) return;

        FCellMovementData Data;

        Data.InitialLocation = CellTransform.GetLocation();
        Data.TargetZ = Data.InitialLocation.Z + FMath::FRandRange(0.f, 50.f);
        Data.InterpSpeed = FMath::FRandRange(1.f, 3.f);
        CellsMovementData.Add(Data);
    }

}

void APC_Generator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    const float EnergyPercent = FMath::Clamp(CurrentEnergy / FMath::Max(MaxEnergy, 1.f),0.f,1.f);
    const float SpeedMultiplier = (1.f - EnergyPercent) * 5.f;

    if (SpeedMultiplier <= 0.f)
    {
        return;
    }

    for (int32 Index = 0; Index < CellsMovementData.Num(); Index++)
    {
        FCellMovementData& Data = CellsMovementData[Index];
        FTransform CellTransform;

        if (!CellsMesh->GetInstanceTransform(Index, CellTransform, false)) continue;

        FVector CurrentLocation = CellTransform.GetLocation();

        const float ActualSpeed = Data.InterpSpeed * SpeedMultiplier;

        CurrentLocation.Z = FMath::FInterpTo(CurrentLocation.Z,Data.TargetZ,DeltaTime,ActualSpeed);

        CellTransform.SetLocation(CurrentLocation);
        CellsMesh->UpdateInstanceTransform(Index,CellTransform,false,true,true);

        if (FMath::Abs(CurrentLocation.Z - Data.TargetZ) < 1.f)
        {
            Data.TargetZ = Data.InitialLocation.Z + FMath::FRandRange(0.f, 50.f);
        }
    }
}



