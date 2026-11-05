// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PP_AirlockController.generated.h"

UENUM()
enum class EAirlockState : uint8
{
	Idle, //Готово к использованию
	ClosingDoors, // Закрытие дверей
	EqualizingPressure, // Выравнивание давления
	OpeningDoors, // Открытие дверей
	Ready, // Завершение цикла
	Error // Ошибка
};

UENUM()
enum class EAirlockDoorState
{
	Inner, // Шлюз внутренний
	Outer, // Шлюз наружний
	BotchClosed, // Закрыты обе двери
	Error // Открыты обе двери
};

class APP_AirlockDoor;

UCLASS()
class SPHEREIMPULSE_API APP_AirlockController : public AActor
{
	GENERATED_BODY()
	
public:	

	APP_AirlockController();

	void SetAirlockState(EAirlockState NewState);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Airlock") TObjectPtr<UStaticMeshComponent> AirlockController;

private:

	UPROPERTY(EditAnywhere, Category = "Airlock") TObjectPtr<APP_AirlockDoor> InnerDoor;
	UPROPERTY(EditAnywhere, Category = "Airlock") TObjectPtr<APP_AirlockDoor> OuterDoor;
	UPROPERTY(EditAnywhere, Category = "Airlock") float AtmospheresPressure = 0.f;
	UPROPERTY(EditAnywhere, Category = "Airlock") float RoomPressure = 100.f;
	UPROPERTY(EditAnywhere, Category = "Airlock") float RatePressureChange = 50.f;

	UPROPERTY(EditAnywhere, Category = "Airlock") float CurrentPressure = 50.f;
	float TargetPressure = 0.f;

	UPROPERTY(EditAnywhere, Category = "Airlock") EAirlockState CurrentAirlockState = EAirlockState::Idle;
	EAirlockDoorState CurrentAirlockDoorState = EAirlockDoorState::Inner;



	UPROPERTY() UMaterialInstanceDynamic* AirlockControllerMID;
	FLinearColor GreenColor = FLinearColor::Green;
	FLinearColor RedColor = FLinearColor::Red;

	UFUNCTION() void DoorInteractionRequested(APP_AirlockDoor* DoorInteraction);
	UFUNCTION() void DoorStateChanged(EDoorState NewState);

	APP_AirlockDoor* CurrentDoorInteraction = nullptr;
	bool bStartEqualizingPressure = false;

	void SwitchingMID();
	void Initialize();
};


