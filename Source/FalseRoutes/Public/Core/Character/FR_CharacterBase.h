

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FR_CharacterBase.generated.h"

UENUM()
enum class EFR_MovementState
{
	Walking,
	Sprinting,
	Crouching
};

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;

UCLASS()
class FALSEROUTES_API AFR_CharacterBase : public ACharacter
{
	GENERATED_BODY()

public:

	AFR_CharacterBase();

	void Move(float InX, float InY);
	void LookAtRotation(FRotator& NewRotation);
	void Sprint(bool bIsSprint);
	void MovementStateCrouch(bool bIsCrounch);

	void SetMovementState(EFR_MovementState NewState);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction") TObjectPtr<USphereComponent> InteractionSphere;


private:

	UPROPERTY(EditAnywhere, Category = "FR_Movement") EFR_MovementState CurrentMovementState = EFR_MovementState::Walking;
	UPROPERTY(EditAnywhere, Category = "FR_Movement") float WalkSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "FR_Movement") float SprintSpeed = 400.f;

};
