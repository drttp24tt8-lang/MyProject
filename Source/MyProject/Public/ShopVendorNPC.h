#pragma once

#include "CoreMinimal.h"
#include "DialogueNPC.h"
#include "ShopVendorNPC.generated.h"

UCLASS()
class MYPROJECT_API AShopVendorNPC : public ADialogueNPC
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shop")
	TArray<FWistoriaShopStockItem> GetMerchantInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool BuyItem(AActor* Buyer, const FWistoriaInventoryItem& Item, int32 Price);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool BuyStockItem(AActor* Buyer, FName ItemId);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool SellItem(AActor* Seller, FName ItemId, int32 Quantity, int32 PricePerItem);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool SellItemToVendor(AActor* Seller, FName ItemId, int32 Quantity);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TArray<FWistoriaShopStockItem> MerchantInventory;
};
