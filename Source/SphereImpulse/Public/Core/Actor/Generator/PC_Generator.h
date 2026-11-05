// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PC_Generator.generated.h"



UCLASS()
class SPHEREIMPULSE_API APC_Generator : public AActor
{
	GENERATED_BODY()
	
public:	

	APC_Generator();

	UFUNCTION(BlueprintCallable, BlueprintPure) float GetMaxEnergy() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) float GetCurrentEnergy() const;

	UFUNCTION(BlueprintCallable) void UpdateEnergy(float EnergyAmount, bool bConsumeEnergy);

	UFUNCTION(BlueprintCallable) bool CanProvideEnergy(float RequiredEnergy);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generator") TObjectPtr<UStaticMeshComponent> Generator;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generator") TObjectPtr<UInstancedStaticMeshComponent> CellsMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cell") int32 Rows = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cell") int32 Colums = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cell") int32 CellSpacing = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cell") int32 CellsHeight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy") float CurrentEnergy = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy") float MaxEnergy = 100.f;

private:

	struct FCellMovementData
	{
		FVector InitialLocation = FVector::ZeroVector;
		float TargetZ = 0.f;
		float InterpSpeed = 1.f;
	};

	TArray<FCellMovementData> CellsMovementData;

};
