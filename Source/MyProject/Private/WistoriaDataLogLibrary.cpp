#include "WistoriaDataLogLibrary.h"

#include "Engine/DataTable.h"
#include "WistoriaRPGDataAssets.h"

bool UWistoriaDataLogLibrary::GetWeaponRow(UDataTable* WeaponTable, FName WeaponId, FWistoriaWeaponDataRow& OutWeaponRow)
{
	if (!WeaponTable || WeaponId.IsNone())
	{
		return false;
	}

	if (const FWistoriaWeaponDataRow* Row = WeaponTable->FindRow<FWistoriaWeaponDataRow>(WeaponId, TEXT("WistoriaWeaponLookup")))
	{
		OutWeaponRow = *Row;
		return true;
	}

	return false;
}

bool UWistoriaDataLogLibrary::GetSpellRow(UDataTable* SpellTable, FName SpellId, FWistoriaSpellDataRow& OutSpellRow)
{
	if (!SpellTable || SpellId.IsNone())
	{
		return false;
	}

	if (const FWistoriaSpellDataRow* Row = SpellTable->FindRow<FWistoriaSpellDataRow>(SpellId, TEXT("WistoriaSpellLookup")))
	{
		OutSpellRow = *Row;
		return true;
	}

	return false;
}

bool UWistoriaDataLogLibrary::GetCraftingRecipeRow(UDataTable* CraftingRecipeTable, FName RecipeId, FWistoriaCraftingRecipe& OutRecipeRow)
{
	if (!CraftingRecipeTable || RecipeId.IsNone())
	{
		return false;
	}

	if (const FWistoriaCraftingRecipe* Row = CraftingRecipeTable->FindRow<FWistoriaCraftingRecipe>(RecipeId, TEXT("WistoriaCraftingRecipeLookup")))
	{
		OutRecipeRow = *Row;
		return true;
	}

	return false;
}

FWistoriaInventoryItem UWistoriaDataLogLibrary::MakeInventoryItemFromWeaponRow(const FWistoriaWeaponDataRow& WeaponRow)
{
	FWistoriaInventoryItem Item;
	Item.ItemId = WeaponRow.WeaponId;
	Item.DisplayName = WeaponRow.WeaponName;
	Item.ItemType = EWistoriaItemType::Weapon;
	Item.EquipmentSlot = WeaponRow.EquipmentSlot;
	Item.Quantity = 1;
	Item.PhysicalAttackBonus = WeaponRow.BasePhysicalDamage;
	Item.MagicPowerBonus = WeaponRow.BaseMagicalMultiplier - 1.0f;
	return Item;
}

FWistoriaInventoryItem UWistoriaDataLogLibrary::MakeInventoryItemFromAsset(const UWistoriaItemDataAsset* ItemAsset, int32 QuantityOverride)
{
	return ItemAsset ? ItemAsset->MakeInventoryItem(QuantityOverride) : FWistoriaInventoryItem();
}

FWistoriaQuestRecord UWistoriaDataLogLibrary::MakeQuestRecordFromAsset(const UWistoriaQuestDataAsset* QuestAsset, bool bStartActive)
{
	return QuestAsset ? QuestAsset->MakeQuestRecord(bStartActive) : FWistoriaQuestRecord();
}

TArray<FWistoriaInventoryItem> UWistoriaDataLogLibrary::MakeInventoryFromLoadout(const UWistoriaStarterLoadoutDataAsset* LoadoutAsset)
{
	return LoadoutAsset ? LoadoutAsset->MakeStartingInventory() : TArray<FWistoriaInventoryItem>();
}

TArray<FWistoriaQuestRecord> UWistoriaDataLogLibrary::MakeQuestsFromLoadout(const UWistoriaStarterLoadoutDataAsset* LoadoutAsset)
{
	return LoadoutAsset ? LoadoutAsset->MakeStartingQuests() : TArray<FWistoriaQuestRecord>();
}
