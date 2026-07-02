#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WistoriaTypes.h"
#include "WistoriaInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChangedSignature);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UWistoriaInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWistoriaInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(const FWistoriaInventoryItem& Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemId, int32 Quantity = 1);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(FName ItemId, int32 Quantity = 1) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FWistoriaInventoryItem> GetItems() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetItems(const TArray<FWistoriaInventoryItem>& NewItems);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
	bool EquipItem(FName ItemId);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
	bool ReforgeWeaponAffix(int32 ItemIndex, int32 AffixIndexToReplace, int32 CostInGold);

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
	TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem> GetEquippedItems() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
	void SetEquippedItems(const TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem>& NewEquippedItems);

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
	float GetTotalPhysicalBonus() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
	float GetTotalMagicBonus() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
	float GetTotalCourageBonus() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
	float GetTotalCritChanceBonus() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
	float GetTotalSpellChantSpeedBonus() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
	float GetTotalCourageGainMultiplierBonus() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Gold")
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Gold")
	bool SpendGold(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Inventory|Gold")
	int32 GetGold() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Gold")
	void SetGold(int32 NewGold);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryChangedSignature OnInventoryChanged;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FWistoriaInventoryItem> Items;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem> EquippedItems;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 Gold = 0;

	int32 FindItemIndex(FName ItemId) const;
	FWistoriaItemAffix GenerateRandomAffix(FWistoriaInventoryItem& Item, int32 AffixIndex) const;
	void RemoveAffixBonus(FWistoriaInventoryItem& Item, const FWistoriaItemAffix& Affix) const;
	void ApplyAffixBonus(FWistoriaInventoryItem& Item, FWistoriaItemAffix& Affix) const;
};
