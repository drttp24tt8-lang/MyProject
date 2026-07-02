#include "WistoriaSkillComponent.h"

UWistoriaSkillComponent::UWistoriaSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWistoriaSkillComponent::AddSkillPoints(int32 Amount)
{
	if (Amount > 0)
	{
		AvailableSkillPoints += Amount;
		OnSkillChanged.Broadcast(NAME_None);
	}
}

bool UWistoriaSkillComponent::PurchaseSkill(FName SkillId)
{
	FWistoriaSkillNode* Skill = SkillTree.Find(SkillId);
	if (!Skill || Skill->bUnlocked || AvailableSkillPoints < Skill->SkillPointCost)
	{
		return false;
	}

	if (!Skill->PrerequisiteSkillId.IsNone() && !IsSkillUnlocked(Skill->PrerequisiteSkillId))
	{
		return false;
	}

	AvailableSkillPoints -= Skill->SkillPointCost;
	Skill->bUnlocked = true;
	OnSkillChanged.Broadcast(SkillId);
	return true;
}

bool UWistoriaSkillComponent::IsSkillUnlocked(FName SkillId) const
{
	const FWistoriaSkillNode* Skill = SkillTree.Find(SkillId);
	return Skill && Skill->bUnlocked;
}

int32 UWistoriaSkillComponent::GetAvailableSkillPoints() const
{
	return AvailableSkillPoints;
}

void UWistoriaSkillComponent::SetAvailableSkillPoints(int32 NewSkillPoints)
{
	AvailableSkillPoints = FMath::Max(0, NewSkillPoints);
	OnSkillChanged.Broadcast(NAME_None);
}

TMap<FName, FWistoriaSkillNode> UWistoriaSkillComponent::GetSkillTree() const
{
	return SkillTree;
}

void UWistoriaSkillComponent::SetSkillTree(const TMap<FName, FWistoriaSkillNode>& NewSkillTree)
{
	SkillTree = NewSkillTree;
	OnSkillChanged.Broadcast(NAME_None);
}
