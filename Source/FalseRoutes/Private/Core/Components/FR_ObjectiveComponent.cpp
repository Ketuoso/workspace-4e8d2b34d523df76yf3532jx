


#include "Core/Components/FR_ObjectiveComponent.h"
#include "Core/Data/FR_ObjectiveTypes.h"


UFR_ObjectiveComponent::UFR_ObjectiveComponent()
{

	PrimaryComponentTick.bCanEverTick = false;


}

bool  UFR_ObjectiveComponent::RegisterObjective(const FFR_ObjectiveDefinition Definition)
{
	if (Definition.ObjectiveId.IsNone()) return;
	if (ObjectiveDefinitions.Contains(Definition.ObjectiveId)) return;
	if (Definition.RequiredObjectives.Contains(Definition.ObjectiveId)) return;

	FFR_ObjectiveDefinition NormalizedDefinition = Definition;
	NormalizedDefinition.RequiredAmount = FMath::Max(1, NormalizedDefinition.RequiredAmount);
	ObjectiveDefinitions.Add(NormalizedDefinition.ObjectiveId, NormalizedDefinition);

	FFR_ObjectiveRuntimeState NewState;
	NewState.CurrentAmount = 0;
	NewState.bIsDiscovered = NormalizedDefinition.bVisibleFromStart;

	NewState.Status = NormalizedDefinition.bVisibleFromStart ? EFR_ObjectiveStatus::Available : EFR_ObjectiveStatus::Hidden;

	ObjectiveStates.Add(NormalizedDefinition.ObjectiveId, NewState);

	OnObjectiveChangedDelegate.Broadcast(NormalizedDefinition.ObjectiveId);
}

bool UFR_ObjectiveComponent::RevealObjective(FName ObjectiveId)
{
	FFR_ObjectiveRuntimeState* State = ObjectiveStates.Find(ObjectiveId);
	if (!State) return;
	if (State->bIsDiscovered) return;
	
	State->bIsDiscovered = true;
	if (State->Status == EFR_ObjectiveStatus::Hidden)
	{
		State->Status = EFR_ObjectiveStatus::Available;
	}

	OnObjectiveChangedDelegate.Broadcast(ObjectiveId);
	return true;
}

bool UFR_ObjectiveComponent::TryAdvanceObjective(FName ObjectiveId)
{
	const FFR_ObjectiveDefinition* Definition = ObjectiveDefinitions.Find(ObjectiveId);
	FFR_ObjectiveRuntimeState* State = ObjectiveStates.Find(ObjectiveId);
	
	if (!Definition || !State) return false;
	if (!State->bIsDiscovered) return false;
	
	const bool bCanAdvance = State->Status == EFR_ObjectiveStatus::Available || State->Status == EFR_ObjectiveStatus::InProgress;
	
	if (!bCanAdvance) return false;
	if (!AreRequirementsMet(*Definition)) return false;
	
	const int32 RequiredAmount = FMath::Max(1, Definition-> RequiredAmount);
	
	if (State->CurrentAmount >= RequiredAmount) return false;
	
	++State->CurrentAmount;
	State->Status = State->CurrentAmount > +RequiredAmount ? EFR_ObjectiveStatus::Completed : EFR_ObjectiveStatus::InProgress;
	OnObjectiveChangedDelegate.Broadcast(ObjectiveId);
	return true;
}

bool UFR_ObjectiveComponent::TryCompletedObjective(FName ObjectiveId)
{
	const FFR_ObjectiveDefinition* Definition = ObjectiveDefinitions.Find(ObjectiveId);
	FFR_ObjectiveRuntimeState* State = ObjectiveStates.Find(ObjectiveId);

	if (!Definition || !State) return false;
	if (!State->bIsDiscovered) return false;

	const bool bCanComplete = State->Status == EFR_ObjectiveStatus::Available || State->Status == EFR_ObjectiveStatus::InProgress;
	
	if (!bCanComplete) return false;
	if (!AreRequirementsMet(*Definition)) return false;

	State->CurrentAmount = FMath::Max(1, Definition->RequiredAmount);
	State->Status = EFR_ObjectiveStatus::Completed;
	OnObjectiveChangedDelegate.Broadcast(ObjectiveId);
	return true;
}

bool UFR_ObjectiveComponent::IsObjectiveCompleted(FName ObjectiveId) const
{
	const FFR_ObjectiveRuntimeState* State = ObjectiveStates.Find(ObjectiveId);
	return State && State->Status == EFR_ObjectiveStatus::Completed;
}

EFR_ObjectiveStatus UFR_ObjectiveComponent::GetObjectiveStatus(FName ObjectiveId) const
{
	const FFR_ObjectiveRuntimeState* State = ObjectiveStates.Find(ObjectiveId);
	return State ? State->Status : EFR_ObjectiveStatus::Hidden;
}

bool UFR_ObjectiveComponent::GetObjectiveDefinition(FName ObjectiveId, FFR_ObjectiveDefinition& OutDefinition) const
{
	const FFR_ObjectiveDefinition* Definition = ObjectiveDefinitions.Find(ObjectiveId);
	
	if (!Definition)
	{
		OutDefinition = FFR_ObjectiveDefinition();
		return false;
	}

	OutDefinition = *Definition;
	return true;
}

bool UFR_ObjectiveComponent::GetObjectiveRuntimeState(FName ObjectiveId, FFR_ObjectiveRuntimeState& OutState) const
{
	const FFR_ObjectiveRuntimeState* State = ObjectiveStates.Find(ObjectiveId);
	if (!State)
	{
		OutState = FFR_ObjectiveRuntimeState();
		return false;
	}
	OutState = *State;
	return true;
}

TArray<FName> UFR_ObjectiveComponent::GetVisibleObjectiveIds() const
{
	TArray<FName> VisibleObjectiveIds;
	for (const auto& Pair : ObjectiveStates)
	{
		if (Pair.Value.bIsDiscovered)
		{
			VisibleObjectiveIds.Add(Pair.Key);
		}
	}

	VisibleObjectiveIds.Sort([this](const FName& LeftId, const FName& RightId)
		{
			const FFR_ObjectiveDefinition* LeftDefinition = ObjectiveDefinitions.Find(LeftId);
			const FFR_ObjectiveDefinition* RightDefinition = ObjectiveDefinitions.Find(RightId);
			if (!LeftDefinition || !RightDefinition)
			{
				return LeftId.ToString() < RightId.ToString();
			}
			if (LeftDefinition->DisplayOrder == RightDefinition->DisplayOrder)
			{
				return LeftId.ToString() < RightId.ToString();
			}
			return LeftDefinition->DisplayOrder < RightDefinition->DisplayOrder;
		});
	return VisibleObjectiveIds;
}



void UFR_ObjectiveComponent::BeginPlay()
{
	Super::BeginPlay();


	
}

bool UFR_ObjectiveComponent::AreRequirementsMet(const FFR_ObjectiveDefinition& Definition) const
{
	if (Definition.RequiredObjectives.Num() == 0) return false;
	if (Definition.RequirementMode == EFR_ObjectiveRequirementMode::All)
	{
		for (auto& RequiredObjectiveId : Definition.RequiredObjectives)
		{
			if (!IsObjectiveCompleted(RequiredObjectiveId)) return false;
		}
		return true;
	}
	for (auto& RequiredObjectiveId : Definition.RequiredObjectives)
	{
		if (IsObjectiveCompleted(RequiredObjectiveId)) return true;
	}
	return false;
}


