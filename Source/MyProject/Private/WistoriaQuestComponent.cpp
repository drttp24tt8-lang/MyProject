#include "WistoriaQuestComponent.h"

#include "Algo/AllOf.h"

UWistoriaQuestComponent::UWistoriaQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWistoriaQuestComponent::StartQuest(const FWistoriaQuestRecord& Quest)
{
	if (Quest.QuestId.IsNone())
	{
		return;
	}

	const int32 ExistingIndex = FindQuestIndex(Quest.QuestId);
	if (ExistingIndex != INDEX_NONE)
	{
		Quests[ExistingIndex] = Quest;
		Quests[ExistingIndex].State = EWistoriaQuestState::Active;
	}
	else
	{
		FWistoriaQuestRecord ActiveQuest = Quest;
		ActiveQuest.State = EWistoriaQuestState::Active;
		Quests.Add(ActiveQuest);
	}

	OnQuestChanged.Broadcast(Quest.QuestId);
}

bool UWistoriaQuestComponent::AdvanceObjective(FName QuestId, FName ObjectiveId, int32 Amount)
{
	const int32 QuestIndex = FindQuestIndex(QuestId);
	if (QuestIndex == INDEX_NONE || ObjectiveId.IsNone() || Amount <= 0)
	{
		return false;
	}

	FWistoriaQuestRecord& Quest = Quests[QuestIndex];
	if (Quest.State != EWistoriaQuestState::Active)
	{
		return false;
	}

	for (FWistoriaQuestObjective& Objective : Quest.Objectives)
	{
		if (Objective.ObjectiveId == ObjectiveId)
		{
			Objective.CurrentCount = FMath::Clamp(Objective.CurrentCount + Amount, 0, Objective.RequiredCount);
			Objective.bCompleted = Objective.CurrentCount >= Objective.RequiredCount;
			RefreshQuestCompletion(QuestIndex);
			OnQuestChanged.Broadcast(QuestId);
			return true;
		}
	}

	return false;
}

bool UWistoriaQuestComponent::AdvanceAnyActiveObjective(FName ObjectiveId, int32 Amount)
{
	TArray<FWistoriaQuestRecord> IgnoredCompletedQuests;
	return AdvanceAnyActiveObjectiveWithCompletions(ObjectiveId, Amount, IgnoredCompletedQuests);
}

bool UWistoriaQuestComponent::AdvanceAnyActiveObjectiveWithCompletions(FName ObjectiveId, int32 Amount, TArray<FWistoriaQuestRecord>& CompletedQuests)
{
	if (ObjectiveId.IsNone() || Amount <= 0)
	{
		return false;
	}

	bool bAdvanced = false;
	for (int32 QuestIndex = 0; QuestIndex < Quests.Num(); ++QuestIndex)
	{
		FWistoriaQuestRecord& Quest = Quests[QuestIndex];
		if (Quest.State != EWistoriaQuestState::Active)
		{
			continue;
		}

		for (FWistoriaQuestObjective& Objective : Quest.Objectives)
		{
			if (Objective.ObjectiveId == ObjectiveId && !Objective.bCompleted)
			{
				Objective.CurrentCount = FMath::Clamp(Objective.CurrentCount + Amount, 0, Objective.RequiredCount);
				Objective.bCompleted = Objective.CurrentCount >= Objective.RequiredCount;
				const EWistoriaQuestState PreviousState = Quest.State;
				RefreshQuestCompletion(QuestIndex);
				if (PreviousState != EWistoriaQuestState::Completed && Quest.State == EWistoriaQuestState::Completed)
				{
					CompletedQuests.Add(Quest);
				}
				OnQuestChanged.Broadcast(Quest.QuestId);
				bAdvanced = true;
			}
		}
	}

	return bAdvanced;
}

bool UWistoriaQuestComponent::CompleteQuest(FName QuestId)
{
	const int32 QuestIndex = FindQuestIndex(QuestId);
	if (QuestIndex == INDEX_NONE)
	{
		return false;
	}

	Quests[QuestIndex].State = EWistoriaQuestState::Completed;
	for (FWistoriaQuestObjective& Objective : Quests[QuestIndex].Objectives)
	{
		Objective.CurrentCount = Objective.RequiredCount;
		Objective.bCompleted = true;
	}

	OnQuestChanged.Broadcast(QuestId);
	return true;
}

TArray<FWistoriaQuestRecord> UWistoriaQuestComponent::GetQuests() const
{
	return Quests;
}

void UWistoriaQuestComponent::SetQuests(const TArray<FWistoriaQuestRecord>& NewQuests)
{
	Quests = NewQuests;
	OnQuestChanged.Broadcast(NAME_None);
}

int32 UWistoriaQuestComponent::FindQuestIndex(FName QuestId) const
{
	return Quests.IndexOfByPredicate([QuestId](const FWistoriaQuestRecord& Quest)
	{
		return Quest.QuestId == QuestId;
	});
}

void UWistoriaQuestComponent::RefreshQuestCompletion(int32 QuestIndex)
{
	if (!Quests.IsValidIndex(QuestIndex) || Quests[QuestIndex].Objectives.Num() == 0)
	{
		return;
	}

	const bool bAllObjectivesComplete = Algo::AllOf(Quests[QuestIndex].Objectives, [](const FWistoriaQuestObjective& Objective)
	{
		return Objective.bCompleted;
	});

	if (bAllObjectivesComplete)
	{
		Quests[QuestIndex].State = EWistoriaQuestState::Completed;
	}
}
