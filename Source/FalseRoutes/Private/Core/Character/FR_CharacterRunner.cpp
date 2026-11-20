


#include "Core/Character/FR_CharacterRunner.h"
#include "Core/GameState/FR_GameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "World/Actor/FR_HiddenActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Core/Character/FR_CharacterHunter.h"

void AFR_CharacterRunner::EnterHiding(AFR_HiddenActor* HiddenSpot, FVector HidePointLocation)
{
	UWorld* World =GetWorld();
	if (!World) return;

	AFR_GameState* FR_GameState = World->GetGameState<AFR_GameState>();
	if (!FR_GameState) return;

	if (bHidden || FR_GameState->GetMatchPhase() != EFR_MatchPhase::Playing) return;

	bHidden = true;
	CurrentHiddenSpot = HiddenSpot;
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SetActorLocation(HidePointLocation);
	SetActorRotation(CurrentHiddenSpot->GetActorRotation());

	SetMovementState(EFR_MovementState::Walking);
	GetMesh()->SetVisibility(false);
}

void AFR_CharacterRunner::ExitHiding(FVector ExitPointLocation)
{
	UWorld* World = GetWorld();
	if (!World) return;

	AFR_GameState* FR_GameState = World->GetGameState<AFR_GameState>();
	if (!FR_GameState) return;

	if (!bHidden || FR_GameState->GetMatchPhase() != EFR_MatchPhase::Playing) return;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<AActor*> OutActors;

	const bool bFound = UKismetSystemLibrary::SphereOverlapActors(this, ExitPointLocation,50.f,ObjectTypes,nullptr, ActorsToIgnore, OutActors);
	if (bFound) return; //Позже вывод сообщения что место занято

	bHidden = false;
	SetActorLocation(ExitPointLocation);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetMesh()->SetVisibility(true);

	CurrentHiddenSpot = nullptr;
}
