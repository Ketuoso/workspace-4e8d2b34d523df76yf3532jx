


#include "Core/Components/FR_InteractionComponent.h"
#include "Core/Interface/FR_Interactable.h"
#include "Core/Character/FR_CharacterBase.h"
#include "Core/Data/FR_MatchPhase.h"
#include "Components/SphereComponent.h"
#include "Core/GameState/FR_GameState.h"
#include "Engine/World.h"


UFR_InteractionComponent::UFR_InteractionComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

void UFR_InteractionComponent::TryInteract()
{
	if (!OwnerCharacter) return;
	if (IsValid(CurrentInteractableActor))
	{
		if (!CurrentInteractableActor->Implements<UFR_Interactable>()) return;
		if (!IFR_Interactable::Execute_CanInteract(CurrentInteractableActor, OwnerCharacter)) return;
		IFR_Interactable::Execute_Interact(CurrentInteractableActor, OwnerCharacter);
	}
}



void UFR_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AFR_CharacterBase>(GetOwner());
	if (!OwnerCharacter) return;
	InteractionSphere = OwnerCharacter->GetInteractionSphere();

	StartInteractionUpdates();
}

void UFR_InteractionComponent::SetCurrentInteractableActor(AActor* NewActor)
{
	if (!OwnerCharacter) return;
	if (CurrentInteractableActor == NewActor) return;
	PreviousActor = CurrentInteractableActor;

	if (PreviousActor)
	{
		if (PreviousActor->Implements<UFR_Interactable>())
		{
			IFR_Interactable::Execute_SetInteractionHighlighted(PreviousActor, OwnerCharacter, false);
		}
	}
	CurrentInteractableActor = NewActor;
	if (CurrentInteractableActor)
	{
		if (CurrentInteractableActor->Implements<UFR_Interactable>())
		{
			IFR_Interactable::Execute_SetInteractionHighlighted(CurrentInteractableActor, OwnerCharacter, true);
			OnInteractionTargetChangedDelegate.Broadcast(CurrentInteractableActor);
		}
	}
	else
	{
		OnInteractionTargetChangedDelegate.Broadcast(CurrentInteractableActor);
	}

}

void UFR_InteractionComponent::StartInteractionUpdates()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(RefreshInteractionTimer, this, &UFR_InteractionComponent::RefreshInteraction, InteractionRefreshInterval, true);
	}
}

void UFR_InteractionComponent::StopInteractionUpdates()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(RefreshInteractionTimer);
	}
	SetCurrentInteractableActor(nullptr);
}

void UFR_InteractionComponent::RefreshInteraction()
{
	if (!CanUpdateInteraction())
	{
		ClearInteractionTarget();
		return;
	}
	
	FindBestInteractable();
}

void UFR_InteractionComponent::FindBestInteractable()
{
	AActor* BestActor = nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	TArray<AActor*> OverlappingActors;
	OwnerCharacter->GetInteractionSphere()->GetOverlappingActors(OverlappingActors, nullptr);

	for (auto& Actor : OverlappingActors)
	{
		if (Actor->Implements<UFR_Interactable>())
		{
			if (IFR_Interactable::Execute_CanInteract(Actor, OwnerCharacter))
			{
				float BestDistanceSquared = OwnerCharacter->GetSquaredDistanceTo(Actor);
				if (BestDistanceSquared < BestDistance)
				{
					BestActor = Actor;
					BestDistance = BestDistanceSquared;
				}
			}
			
		}
	}
	SetCurrentInteractableActor(BestActor);
}

bool UFR_InteractionComponent::CanUpdateInteraction()
{
	if (!OwnerCharacter->IsLocallyControlled()) return false;
	UWorld* World = GetWorld();
	if (!World) return false;
	AFR_GameState* FR_GameState = World->GetGameState<AFR_GameState>();
	if (!FR_GameState) return false;
	if (FR_GameState->GetMatchPhase() == EFR_MatchPhase::Won || FR_GameState->GetMatchPhase() == EFR_MatchPhase::Lost) return false;
	return true;
}

void UFR_InteractionComponent::ClearInteractionTarget()
{
	if (CurrentInteractableActor)
	{
		if (CurrentInteractableActor->Implements<UFR_Interactable>())
		{
			IFR_Interactable::Execute_SetInteractionHighlighted(CurrentInteractableActor, OwnerCharacter, false);
		}
	}
	SetCurrentInteractableActor(nullptr);
}

