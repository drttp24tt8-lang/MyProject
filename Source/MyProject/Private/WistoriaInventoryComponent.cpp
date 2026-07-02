#include "WistoriaInventoryComponent.h"

#include "Engine/Engine.h"

UWistoriaInventoryComponent::UWistoriaInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWistoriaInventoryComponent::AddItem(const FWistoriaInventoryItem& Item)
{
	if (Item.ItemId.IsNone() || Item.Quantity <= 0)
	{
		return;
	}

	const bool bStackable = Item.ItemType == EWistoriaItemType::Consumable || Item.ItemType == EWistoriaItemType::Material || Item.ItemType == EWistoriaItemType::KeyItem;
	const int32 ExistingIndex = FindItemIndex(Item.ItemId);
	if (bStackable && ExistingIndex != INDEX_NONE)
	{
		Items[ExistingIndex].Quantity += Item.Quantity;
	}
	else
	{
		Items.Add(Item);
	}

	OnInventoryChanged.Broadcast();
}

bool UWistoriaInventoryComponent::RemoveItem(FName ItemId, int32 Quantity)
{
	if (ItemId.IsNone() || Quantity <= 0)
	{
		return false;
	}

	const int32 ExistingIndex = FindItemIndex(ItemId);
	if (ExistingIndex == INDEX_NONE || Items[ExistingIndex].Quantity < Quantity)
	{
		return false;
	}

	Items[ExistingIndex].Quantity -= Quantity;
	if (Items[ExistingIndex].Quantity <= 0)
	{
		Items.RemoveAt(ExistingIndex);
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UWistoriaInventoryComponent::HasItem(FName ItemId, int32 Quantity) const
{
	const int32 ExistingIndex = FindItemIndex(ItemId);
	return ExistingIndex != INDEX_NONE && Items[ExistingIndex].Quantity >= Quantity;
}

TArray<FWistoriaInventoryItem> UWistoriaInventoryComponent::GetItems() const
{
	return Items;
}

void UWistoriaInventoryComponent::SetItems(const TArray<FWistoriaInventoryItem>& NewItems)
{
	Items = NewItems;
	OnInventoryChanged.Broadcast();
}

bool UWistoriaInventoryComponent::EquipItem(FName ItemId)
{
	const int32 ExistingIndex = FindItemIndex(ItemId);
	if (ExistingIndex == INDEX_NONE)
	{
		return false;
	}

	const FWistoriaInventoryItem ItemToEquip = Items[ExistingIndex];
	if (ItemToEquip.ItemType != EWistoriaItemType::Weapon && ItemToEquip.ItemType != EWistoriaItemType::Armor)
	{
		return false;
	}

	Items.RemoveAt(ExistingIndex);

	if (FWistoriaInventoryItem* PreviouslyEquipped = EquippedItems.Find(ItemToEquip.EquipmentSlot))
	{
		Items.Add(*PreviouslyEquipped);
	}

	EquippedItems.Add(ItemToEquip.EquipmentSlot, ItemToEquip);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UWistoriaInventoryComponent::ReforgeWeaponAffix(int32 ItemIndex, int32 AffixIndexToReplace, int32 CostInGold)
{
	if (!Items.IsValidIndex(ItemIndex) || CostInGold < 0)
	{
		return false;
	}

	if (Gold < CostInGold)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Insufficient gold for stat reforging!"));
		}
		return false;
	}

	FWistoriaInventoryItem& Item = Items[ItemIndex];
	if (Item.ItemType != EWistoriaItemType::Weapon || !Item.Affixes.IsValidIndex(AffixIndexToReplace))
	{
		return false;
	}

	Gold -= CostInGold;
	RemoveAffixBonus(Item, Item.Affixes[AffixIndexToReplace]);
	FWistoriaItemAffix NewAffix = GenerateRandomAffix(Item, AffixIndexToReplace);
	ApplyAffixBonus(Item, NewAffix);
	Item.Affixes[AffixIndexToReplace] = NewAffix;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Cyan,
			FString::Printf(TEXT("REFORGE SUCCESS: %s. Spent %d Gold."), *NewAffix.Description.ToString(), CostInGold));
	}
	OnInventoryChanged.Broadcast();
	return true;
}

TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem> UWistoriaInventoryComponent::GetEquippedItems() const
{
	return EquippedItems;
}

void UWistoriaInventoryComponent::SetEquippedItems(const TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem>& NewEquippedItems)
{
	EquippedItems = NewEquippedItems;
	OnInventoryChanged.Broadcast();
}

float UWistoriaInventoryComponent::GetTotalPhysicalBonus() const
{
	float Total = 0.0f;
	for (const TPair<EWistoriaEquipmentSlot, FWistoriaInventoryItem>& Pair : EquippedItems)
	{
		Total += Pair.Value.PhysicalAttackBonus;
	}
	return Total;
}

float UWistoriaInventoryComponent::GetTotalMagicBonus() const
{
	float Total = 0.0f;
	for (const TPair<EWistoriaEquipmentSlot, FWistoriaInventoryItem>& Pair : EquippedItems)
	{
		Total += Pair.Value.MagicPowerBonus;
	}
	return Total;
}

float UWistoriaInventoryComponent::GetTotalCourageBonus() const
{
	float Total = 0.0f;
	for (const TPair<EWistoriaEquipmentSlot, FWistoriaInventoryItem>& Pair : EquippedItems)
	{
		Total += Pair.Value.CourageBonus;
	}
	return Total;
}

float UWistoriaInventoryComponent::GetTotalCritChanceBonus() const
{
	float Total = 0.0f;
	for (const TPair<EWistoriaEquipmentSlot, FWistoriaInventoryItem>& Pair : EquippedItems)
	{
		Total += Pair.Value.CritChanceBonus;
	}
	return Total;
}

float UWistoriaInventoryComponent::GetTotalSpellChantSpeedBonus() const
{
	float Total = 0.0f;
	for (const TPair<EWistoriaEquipmentSlot, FWistoriaInventoryItem>& Pair : EquippedItems)
	{
		Total += Pair.Value.SpellChantSpeedBonus;
	}
	return Total;
}

float UWistoriaInventoryComponent::GetTotalCourageGainMultiplierBonus() const
{
	float Total = 0.0f;
	for (const TPair<EWistoriaEquipmentSlot, FWistoriaInventoryItem>& Pair : EquippedItems)
	{
		Total += Pair.Value.CourageGainMultiplierBonus;
	}
	return Total;
}

void UWistoriaInventoryComponent::AddGold(int32 Amount)
{
	if (Amount > 0)
	{
		Gold += Amount;
		OnInventoryChanged.Broadcast();
	}
}

bool UWistoriaInventoryComponent::SpendGold(int32 Amount)
{
	if (Amount < 0 || Gold < Amount)
	{
		return false;
	}

	Gold -= Amount;
	OnInventoryChanged.Broadcast();
	return true;
}

int32 UWistoriaInventoryComponent::GetGold() const
{
	return Gold;
}

void UWistoriaInventoryComponent::SetGold(int32 NewGold)
{
	Gold = FMath::Max(0, NewGold);
	OnInventoryChanged.Broadcast();
}

int32 UWistoriaInventoryComponent::FindItemIndex(FName ItemId) const
{
	return Items.IndexOfByPredicate([ItemId](const FWistoriaInventoryItem& Item)
	{
		return Item.ItemId == ItemId;
	});
}

FWistoriaItemAffix UWistoriaInventoryComponent::GenerateRandomAffix(FWistoriaInventoryItem& Item, int32 AffixIndex) const
{
	static const TArray<FName> AvailableStats =
	{
		TEXT("CritChance"),
		TEXT("SpellChantSpeed"),
		TEXT("CourageGainScale"),
		TEXT("PhysicalAttack"),
		TEXT("MagicPower")
	};

	float RarityMultiplier = 1.0f;
	switch (Item.Rarity)
	{
	case EWistoriaItemRarity::Legendary:
		RarityMultiplier = 2.0f;
		break;
	case EWistoriaItemRarity::Epic:
		RarityMultiplier = 1.5f;
		break;
	case EWistoriaItemRarity::Rare:
		RarityMultiplier = 1.2f;
		break;
	case EWistoriaItemRarity::Standard:
	default:
		RarityMultiplier = 1.0f;
		break;
	}

	FWistoriaItemAffix Affix;
	Affix.StatAffected = AvailableStats[FMath::RandRange(0, AvailableStats.Num() - 1)];
	Affix.StatValueBonus = FMath::FRandRange(0.05f, 0.15f) * RarityMultiplier;
	Affix.AffixId = FName(*FString::Printf(TEXT("%s_Reforged_%d"), *Affix.StatAffected.ToString(), AffixIndex));
	return Affix;
}

void UWistoriaInventoryComponent::RemoveAffixBonus(FWistoriaInventoryItem& Item, const FWistoriaItemAffix& Affix) const
{
	if (Affix.StatAffected == FName(TEXT("CritChance")))
	{
		Item.CritChanceBonus -= Affix.StatValueBonus;
	}
	else if (Affix.StatAffected == FName(TEXT("SpellChantSpeed")))
	{
		Item.SpellChantSpeedBonus -= Affix.StatValueBonus;
	}
	else if (Affix.StatAffected == FName(TEXT("CourageGainScale")))
	{
		Item.CourageGainMultiplierBonus -= Affix.StatValueBonus;
	}
	else if (Affix.StatAffected == FName(TEXT("PhysicalAttack")))
	{
		Item.PhysicalAttackBonus -= Affix.StatValueBonus * 100.0f;
	}
	else if (Affix.StatAffected == FName(TEXT("MagicPower")))
	{
		Item.MagicPowerBonus -= Affix.StatValueBonus;
	}
}

void UWistoriaInventoryComponent::ApplyAffixBonus(FWistoriaInventoryItem& Item, FWistoriaItemAffix& Affix) const
{
	if (Affix.StatAffected == FName(TEXT("CritChance")))
	{
		Item.CritChanceBonus += Affix.StatValueBonus;
		Affix.Description = FText::FromString(FString::Printf(TEXT("+%.0f%% critical strike chance"), Affix.StatValueBonus * 100.0f));
	}
	else if (Affix.StatAffected == FName(TEXT("SpellChantSpeed")))
	{
		Item.SpellChantSpeedBonus += Affix.StatValueBonus;
		Affix.Description = FText::FromString(FString::Printf(TEXT("+%.0f%% spell chant output"), Affix.StatValueBonus * 100.0f));
	}
	else if (Affix.StatAffected == FName(TEXT("CourageGainScale")))
	{
		Item.CourageGainMultiplierBonus += Affix.StatValueBonus;
		Affix.Description = FText::FromString(FString::Printf(TEXT("+%.0f%% courage gain"), Affix.StatValueBonus * 100.0f));
	}
	else if (Affix.StatAffected == FName(TEXT("PhysicalAttack")))
	{
		Item.PhysicalAttackBonus += Affix.StatValueBonus * 100.0f;
		Affix.Description = FText::FromString(FString::Printf(TEXT("+%.1f physical attack"), Affix.StatValueBonus * 100.0f));
	}
	else if (Affix.StatAffected == FName(TEXT("MagicPower")))
	{
		Item.MagicPowerBonus += Affix.StatValueBonus;
		Affix.Description = FText::FromString(FString::Printf(TEXT("+%.0f%% magic power"), Affix.StatValueBonus * 100.0f));
	}
}
