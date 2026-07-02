#include "ShopVendorNPC.h"

#include "WistoriaCharacter.h"
#include "WistoriaInventoryComponent.h"

TArray<FWistoriaShopStockItem> AShopVendorNPC::GetMerchantInventory() const
{
	return MerchantInventory;
}

bool AShopVendorNPC::BuyItem(AActor* Buyer, const FWistoriaInventoryItem& Item, int32 Price)
{
	if (Item.ItemId.IsNone() || Item.Quantity <= 0 || Price < 0)
	{
		return false;
	}

	AWistoriaCharacter* Player = Cast<AWistoriaCharacter>(Buyer);
	UWistoriaInventoryComponent* Inventory = Player ? Player->FindComponentByClass<UWistoriaInventoryComponent>() : nullptr;
	if (!Inventory || !Inventory->SpendGold(Price))
	{
		return false;
	}

	Player->AddInventoryItem(Item);
	return true;
}

bool AShopVendorNPC::BuyStockItem(AActor* Buyer, FName ItemId)
{
	if (ItemId.IsNone())
	{
		return false;
	}

	for (FWistoriaShopStockItem& StockItem : MerchantInventory)
	{
		if (StockItem.Item.ItemId != ItemId)
		{
			continue;
		}

		if (!StockItem.bUnlimitedStock && StockItem.StockQuantity <= 0)
		{
			return false;
		}

		if (!BuyItem(Buyer, StockItem.Item, StockItem.BuyPrice))
		{
			return false;
		}

		if (!StockItem.bUnlimitedStock)
		{
			StockItem.StockQuantity = FMath::Max(0, StockItem.StockQuantity - StockItem.Item.Quantity);
		}
		return true;
	}

	return false;
}

bool AShopVendorNPC::SellItem(AActor* Seller, FName ItemId, int32 Quantity, int32 PricePerItem)
{
	if (ItemId.IsNone() || Quantity <= 0 || PricePerItem < 0)
	{
		return false;
	}

	AWistoriaCharacter* Player = Cast<AWistoriaCharacter>(Seller);
	UWistoriaInventoryComponent* Inventory = Player ? Player->FindComponentByClass<UWistoriaInventoryComponent>() : nullptr;
	if (!Inventory || !Inventory->RemoveItem(ItemId, Quantity))
	{
		return false;
	}

	Inventory->AddGold(Quantity * PricePerItem);
	return true;
}

bool AShopVendorNPC::SellItemToVendor(AActor* Seller, FName ItemId, int32 Quantity)
{
	if (ItemId.IsNone())
	{
		return false;
	}

	int32 PricePerItem = 1;
	for (const FWistoriaShopStockItem& StockItem : MerchantInventory)
	{
		if (StockItem.Item.ItemId == ItemId)
		{
			PricePerItem = StockItem.SellPrice;
			break;
		}
	}

	return SellItem(Seller, ItemId, Quantity, PricePerItem);
}
