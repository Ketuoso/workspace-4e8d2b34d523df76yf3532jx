

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
class UFR_InteractionComponent;
class UMaterialInterface;

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

	UFR_InteractionComponent* GetInteractionComponent() { return FR_InteractionComponent; }
	USphereComponent* GetInteractionSphere() { return InteractionSphere; }

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction") TObjectPtr<USphereComponent> InteractionSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction") TObjectPtr<UFR_InteractionComponent> FR_InteractionComponent;



private:

	void InizializationRenderTarget();
	void UpdateFOGOrigin();
	void UpdateFrontVisionRays();
	void UpdateNearVisionRays();
	void BuildFrontFOGTriangles();
	void DrawNearFOGMask();
	FVector2D WorldToFOGPixel(FVector WorldLocation);
	void RefreshFOG();

	UPROPERTY(EditAnywhere, Category = "FR_Movement") EFR_MovementState CurrentMovementState = EFR_MovementState::Walking;
	UPROPERTY(EditAnywhere, Category = "FR_Movement") float WalkSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "FR_Movement") float SprintSpeed = 400.f;

	UPROPERTY(Transient) UMaterialInstanceDynamic* FOG_MID;
	FVector2D FOG_OriginXY = FVector2D::ZeroVector;
	UPROPERTY(Transient) UTextureRenderTarget2D* FOG_RT;
	FTimerHandle RefreshFOGTimer;
	TArray<FVector> FrontVisionPoints;
	TArray<FVector> NearVisionPoints;
	TArray<FCanvasUVTri> NearFOGTriangles;
	TArray<FCanvasUVTri> FrontFOGTriangles;


	UPROPERTY(EditDefaultsOnly, Category = "FOG") TObjectPtr<UMaterialInterface> FOG_Material;
	UPROPERTY(EditDefaultsOnly, Category = "FOG") float FOG_WorldSpan = 2048.f;
	UPROPERTY(EditDefaultsOnly, Category = "FOG") int32 FrontRayCount = 40;
	UPROPERTY(EditDefaultsOnly, Category = "FOG") int32 NearRayCount = 72;
	UPROPERTY(EditDefaultsOnly, Category = "FOG") float ViewingAngle = 110.f;
	UPROPERTY(EditDefaultsOnly, Category = "FOG") float ForwardVisiondDistance = 900.f;
	UPROPERTY(EditDefaultsOnly, Category = "FOG") float NearVisionRadius = 150.f;
	UPROPERTY(EditDefaultsOnly, Category = "FOG") float OffsetLocationToZ = 200.f;

	
};
