#include "WistoriaHUDLibrary.h"

#include "WistoriaCharacter.h"
#include "WistoriaInventoryComponent.h"
#include "WistoriaQuestComponent.h"

FWistoriaHUDSnapshot UWistoriaHUDLibrary::MakeHUDSnapshot(AWistoriaCharacter* Character)
{
	FWistoriaHUDSnapshot Snapshot;
	if (!Character)
	{
		return Snapshot;
	}

	const float MaxHealth = Character->GetMaxHealthValue();
	Snapshot.HealthPercent = MaxHealth > 0.0f ? Character->GetCurrentHealth() / MaxHealth : 0.0f;
	Snapshot.ManaPercent = Character->GetMaxMana() > 0.0f ? Character->GetCurrentMana() / Character->GetMaxMana() : 0.0f;
	Snapshot.CouragePercent = Character->GetMaxCourage() > 0.0f ? Character->GetCurrentCourage() / Character->GetMaxCourage() : 0.0f;
	Snapshot.Level = Character->GetCharacterLevel();
	Snapshot.CurrentXP = Character->GetCurrentXP();
	Snapshot.Coins = Character->GetCoins();
	Snapshot.bIsTargeting = Character->IsTargeting();

	if (const UWistoriaInventoryComponent* Inventory = Character->FindComponentByClass<UWistoriaInventoryComponent>())
	{
		Snapshot.Gold = Inventory->GetGold();
	}

	if (const UWistoriaQuestComponent* Quests = Character->FindComponentByClass<UWistoriaQuestComponent>())
	{
		const TArray<FWistoriaQuestRecord> QuestRecords = Quests->GetQuests();
		for (const FWistoriaQuestRecord& Quest : QuestRecords)
		{
			if (Quest.State == EWistoriaQuestState::Active)
			{
				Snapshot.PrimaryQuestText = FormatQuestObjective(Quest);
				break;
			}
		}
	}

	return Snapshot;
}

FText UWistoriaHUDLibrary::FormatQuestObjective(const FWistoriaQuestRecord& Quest)
{
	if (Quest.Objectives.Num() == 0)
	{
		return Quest.QuestName;
	}

	const FWistoriaQuestObjective& Objective = Quest.Objectives[0];
	return FText::Format(
		FText::FromString(TEXT("{0}: {1} / {2}")),
		Quest.QuestName,
		FText::AsNumber(Objective.CurrentCount),
		FText::AsNumber(Objective.RequiredCount));
}
