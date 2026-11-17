

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/Data/FR_MatchPhase.h"
#include "FR_PlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class AFR_CharacterBase;

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class FALSEROUTES_API AFR_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;

	// INPUT ACTION

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input") TObjectPtr<UInputAction> FR_MoveAction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input") TObjectPtr<UInputAction> FR_InteractAction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input") TObjectPtr<UInputAction> FR_SprintAction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input") TObjectPtr<UInputAction> FR_CrouchAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input") TObjectPtr<UInputMappingContext> FR_InputContext;



private:

	UFUNCTION() void HandleMatchPhaseChanged(EFR_MatchPhase NewPhase, EFR_MatchPhase OldPhase);
	UFUNCTION() void HandleInteractionTargetChanged(AActor* NewTarget);

	bool IsGameplayInputAllowed() const;

	//BEGIN PLAY

	void InitializeInputMappings();

	//INPUT BIND

	void Move(const FInputActionValue& InputActionValue);
	void StartedInteract();

	void StartedSprint();
	void CompletedSprint();

	void StartedCrouch();
	void CompletedCrouch();

	void LookAtRotation();


	AFR_CharacterBase* OwnerCharacter = nullptr;
	FTimerHandle TimerLookAtRotation;

};
