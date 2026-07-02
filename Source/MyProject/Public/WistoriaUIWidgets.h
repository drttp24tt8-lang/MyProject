#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WistoriaTypes.h"
#include "WistoriaUIWidgets.generated.h"

class ADialogueNPC;
class AShopVendorNPC;
class AWistoriaBossEnemy;
class AWistoriaCharacter;
class AWistoriaCompanionCharacter;
class AWistoriaPlayerController;
class AWistoriaRaidGameMode;
class UWistoriaGameInstance;

UCLASS()
class MYPROJECT_API UWistoriaUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Wistoria|UI")
	AWistoriaCharacter* GetWistoriaCharacter() const;

	UFUNCTION(BlueprintPure, Category = "Wistoria|UI")
	AWistoriaPlayerController* GetWistoriaController() const;
};

UCLASS()
class MYPROJECT_API UWistoriaHUDWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|HUD")
	FWistoriaHUDSnapshot RefreshHUDSnapshot();

	UFUNCTION(BlueprintImplementableEvent, Category = "Wistoria|HUD")
	void OnHUDSnapshotRefreshed(const FWistoriaHUDSnapshot& Snapshot);
};

UCLASS()
class MYPROJECT_API UWistoriaPauseMenuWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Pause")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Pause")
	bool SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Pause")
	bool LoadGame();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Pause")
	bool StartNewGamePlus();
};

UCLASS()
class MYPROJECT_API UWistoriaInventoryWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Wistoria|Inventory")
	TArray<FWistoriaInventoryItem> GetInventoryItems() const;

	UFUNCTION(BlueprintPure, Category = "Wistoria|Inventory")
	TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem> GetEquippedItems() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Inventory")
	bool EquipItem(FName ItemId);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Inventory")
	bool ReforgeWeaponSlot(int32 ItemIndex, int32 AffixIndexToReplace, int32 CostInGold);
};

UCLASS()
class MYPROJECT_API UWistoriaQuestJournalWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Wistoria|Quest")
	TArray<FWistoriaQuestRecord> GetQuestRecords() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Quest")
	bool TrackObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount = 1);
};

UCLASS()
class MYPROJECT_API UWistoriaSkillTreeWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Wistoria|Skills")
	int32 GetAvailableSkillPoints() const;

	UFUNCTION(BlueprintPure, Category = "Wistoria|Skills")
	TMap<FName, FWistoriaSkillNode> GetSkillTree() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Skills")
	bool PurchaseSkill(FName SkillId);
};

UCLASS()
class MYPROJECT_API UWistoriaBossHUDWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Boss")
	void SetBoss(AWistoriaBossEnemy* NewBoss);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Boss")
	float GetBossHealthPercent() const;

	UFUNCTION()
	void HandleBossHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "Wistoria|Boss")
	void OnBossHealthPercentChanged(float NewPercent);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Boss")
	TObjectPtr<AWistoriaBossEnemy> Boss;
};

UCLASS()
class MYPROJECT_API UWistoriaDialogueWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Dialogue")
	void OpenDialogue(ADialogueNPC* NewNPC, const TArray<FText>& NewDialogueLines);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Dialogue")
	TArray<FText> GetDialogueLines() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Dialogue")
	TObjectPtr<ADialogueNPC> NPC;

	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Dialogue")
	TArray<FText> DialogueLines;
};

UCLASS()
class MYPROJECT_API UWistoriaShopWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Shop")
	void SetVendor(AShopVendorNPC* NewVendor);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Shop")
	TArray<FWistoriaShopStockItem> GetMerchantInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Shop")
	bool BuyStockItem(FName ItemId);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Shop")
	bool BuyItem(const FWistoriaInventoryItem& Item, int32 Price);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Shop")
	bool SellItem(FName ItemId, int32 Quantity, int32 PricePerItem);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Shop")
	bool SellItemToVendor(FName ItemId, int32 Quantity);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Shop")
	TObjectPtr<AShopVendorNPC> Vendor;
};

UCLASS()
class MYPROJECT_API UWistoriaFastTravelWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Fast Travel")
	void SetAvailableGates(const TArray<FWistoriaTeleportPoint>& NewAvailableGates);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Fast Travel")
	TArray<FWistoriaTeleportPoint> GetAvailableGates() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Fast Travel")
	bool TravelToGate(FName GateId);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Fast Travel")
	TArray<FWistoriaTeleportPoint> AvailableGates;
};

UCLASS()
class MYPROJECT_API UWistoriaCompanionCommandWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Wistoria|Companion")
	TArray<AWistoriaCompanionCharacter*> GetCompanions() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Companion")
	int32 CommandFollow();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Companion")
	int32 CommandHoldPosition();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Companion")
	int32 CommandAttackLockedTarget();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Companion")
	int32 CommandDefendPlayer();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Companion")
	int32 CommandInteractWithTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Companion")
	bool ActivateTeamUltimate(FName CompanionId);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Companion")
	TMap<FName, FWistoriaCompanionBond> GetCompanionBonds() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Companion")
	int32 IssueCommand(EWistoriaCompanionCommand Command, AActor* CommandTarget);
};

UCLASS()
class MYPROJECT_API UWistoriaCharacterCreatorWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Customization")
	void SetHairColor(const FLinearColor& NewHairColor);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Customization")
	void SetRobeColor(const FLinearColor& NewRobeColor);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Customization")
	FLinearColor GetHairColor() const;

	UFUNCTION(BlueprintPure, Category = "Wistoria|Customization")
	FLinearColor GetRobeColor() const;
};

UCLASS()
class MYPROJECT_API UWistoriaSaveSlotManagerWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Wistoria|Save")
	TArray<FWistoriaSaveSlotInfo> GetSaveSlots(int32 SlotCount = 3) const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	bool SelectSlot(int32 SlotIndex, bool bCreateIfEmpty = false);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	bool StartNewGameInSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	bool LoadGameInSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	bool DeleteSaveSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	void OpenGameplayLevel();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Save")
	FName GameplayLevelName = TEXT("LV1_TestArena");

	UWistoriaGameInstance* GetWistoriaGameInstance() const;
};

UCLASS()
class MYPROJECT_API UWistoriaRaidHUDWidget : public UWistoriaUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Raid")
	FWistoriaRaidSnapshot RefreshRaidSnapshot();

	UFUNCTION(BlueprintPure, Category = "Wistoria|Raid")
	AWistoriaRaidGameMode* GetRaidGameMode() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Wistoria|Raid")
	void OnRaidSnapshotRefreshed(const FWistoriaRaidSnapshot& Snapshot);
};
