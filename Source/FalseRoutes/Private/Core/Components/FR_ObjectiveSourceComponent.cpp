


#include "Core/Components/FR_ObjectiveSourceComponent.h"


UFR_ObjectiveSourceComponent::UFR_ObjectiveSourceComponent()
{

	PrimaryComponentTick.bCanEverTick = false;


}

bool UFR_ObjectiveSourceComponent::BindObjective(FName SourceNodeId, FName RuntimeObjectiveId)
{
	if (SourceNodeId.IsNone() || RuntimeObjectiveId.IsNone()) return false;

	for (const FFR_ObjectiveSourceBinding& Binding : ObjectiveBindings)
	{
		if (Binding.SourceNodeId == SourceNodeId)
		{
			return Binding.RuntimeObjectiveId == RuntimeObjectiveId;
		}
	}

	FFR_ObjectiveSourceBinding NewBinding;
	NewBinding.SourceNodeId = SourceNodeId;
	NewBinding.RuntimeObjectiveId = RuntimeObjectiveId;

	ObjectiveBindings.Add(NewBinding);
	return true;
}

void UFR_ObjectiveSourceComponent::ClearObjectiveBindings()
{
	ObjectiveBindings.Empty();
}

bool UFR_ObjectiveSourceComponent::FindObjectiveId(FName SourceNodeId, FName& OutRuntimeObjectiveId) const
{
	OutRuntimeObjectiveId = NAME_None;
	if (SourceNodeId.IsNone()) return false;

	for (const FFR_ObjectiveSourceBinding& Binding : ObjectiveBindings)
	{
		if (Binding.SourceNodeId == SourceNodeId)
		{
			OutRuntimeObjectiveId = Binding.RuntimeObjectiveId;
			return true;
		}
	}
	return false;
}

bool UFR_ObjectiveSourceComponent::HasBinding(FName SourceNodeId) const
{
	for (const FFR_ObjectiveSourceBinding& Binding : ObjectiveBindings)
	{
		if (Binding.SourceNodeId == SourceNodeId) return true;
	}
	return false;
}



