#include "WistoriaRPGDataAssets.h"

FWistoriaInventoryItem UWistoriaItemDataAsset::MakeInventoryItem(int32 QuantityOverride) const
{
	FWistoriaInventoryItem Result = Item;
	if (QuantityOverride > 0)
	{
		Result.Quantity = QuantityOverride;
	}
	return Result;
}

FWistoriaQuestRecord UWistoriaQuestDataAsset::MakeQuestRecord(bool bStartActive) const
{
	FWistoriaQuestRecord Result = Quest;
	Result.State = bStartActive ? EWistoriaQuestState::Active : EWistoriaQuestState::Inactive;
	return Result;
}

TArray<FWistoriaInventoryItem> UWistoriaStarterLoadoutDataAsset::MakeStartingInventory() const
{
	TArray<FWistoriaInventoryItem> Result;
	for (const UWistoriaItemDataAsset* ItemAsset : StartingItems)
	{
		if (ItemAsset)
		{
			Result.Add(ItemAsset->MakeInventoryItem());
		}
	}
	return Result;
}

TArray<FWistoriaQuestRecord> UWistoriaStarterLoadoutDataAsset::MakeStartingQuests() const
{
	TArray<FWistoriaQuestRecord> Result;
	for (const UWistoriaQuestDataAsset* QuestAsset : StartingQuests)
	{
		if (QuestAsset)
		{
			Result.Add(QuestAsset->MakeQuestRecord(true));
		}
	}
	return Result;
}
