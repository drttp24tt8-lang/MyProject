#include "WistoriaUIWidgets.h"

#include "DialogueNPC.h"
#include "ShopVendorNPC.h"
#include "WistoriaBossEnemy.h"
#include "WistoriaCharacter.h"
#include "WistoriaCompanionCharacter.h"
#include "WistoriaHUDLibrary.h"
#include "WistoriaInventoryComponent.h"
#include "WistoriaPlayerController.h"
#include "WistoriaQuestComponent.h"
#include "WistoriaRaidGameMode.h"
#include "WistoriaSaveLibrary.h"
#include "WistoriaSkillComponent.h"
#include "WistoriaGameInstance.h"
#include "Kismet/GameplayStatics.h"

AWistoriaCharacter* UWistoriaUserWidgetBase::GetWistoriaCharacter() const
{
	return Cast<AWistoriaCharacter>(GetOwningPlayerPawn());
}

AWistoriaPlayerController* UWistoriaUserWidgetBase::GetWistoriaController() const
{
	return Cast<AWistoriaPlayerController>(GetOwningPlayer());
}

FWistoriaHUDSnapshot UWistoriaHUDWidget::RefreshHUDSnapshot()
{
	FWistoriaHUDSnapshot Snapshot = UWistoriaHUDLibrary::MakeHUDSnapshot(GetWistoriaCharacter());
	OnHUDSnapshotRefreshed(Snapshot);
	return Snapshot;
}

void UWistoriaPauseMenuWidget::ResumeGame()
{
	if (AWistoriaPlayerController* Controller = GetWistoriaController())
	{
		Controller->HidePauseMenu();
	}
}

bool UWistoriaPauseMenuWidget::SaveGame()
{
	const UWistoriaGameInstance* WistoriaGameInstance = GetGameInstance<UWistoriaGameInstance>();
	return UWistoriaSaveLibrary::SavePlayer(
		GetWistoriaCharacter(),
		WistoriaGameInstance ? WistoriaGameInstance->GetActiveSaveSlot() : UWistoriaSaveLibrary::GetDefaultSaveSlot());
}

bool UWistoriaPauseMenuWidget::LoadGame()
{
	const UWistoriaGameInstance* WistoriaGameInstance = GetGameInstance<UWistoriaGameInstance>();
	return UWistoriaSaveLibrary::LoadPlayer(
		GetWistoriaCharacter(),
		WistoriaGameInstance ? WistoriaGameInstance->GetActiveSaveSlot() : UWistoriaSaveLibrary::GetDefaultSaveSlot());
}

bool UWistoriaPauseMenuWidget::StartNewGamePlus()
{
	const UWistoriaGameInstance* WistoriaGameInstance = GetGameInstance<UWistoriaGameInstance>();
	return UWistoriaSaveLibrary::StartNewGamePlus(
		GetWistoriaCharacter(),
		WistoriaGameInstance ? WistoriaGameInstance->GetActiveSaveSlot() : UWistoriaSaveLibrary::GetDefaultSaveSlot());
}

TArray<FWistoriaInventoryItem> UWistoriaInventoryWidget::GetInventoryItems() const
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		if (const UWistoriaInventoryComponent* Inventory = Character->FindComponentByClass<UWistoriaInventoryComponent>())
		{
			return Inventory->GetItems();
		}
	}

	return TArray<FWistoriaInventoryItem>();
}

TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem> UWistoriaInventoryWidget::GetEquippedItems() const
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		if (const UWistoriaInventoryComponent* Inventory = Character->FindComponentByClass<UWistoriaInventoryComponent>())
		{
			return Inventory->GetEquippedItems();
		}
	}

	return TMap<EWistoriaEquipmentSlot, FWistoriaInventoryItem>();
}

bool UWistoriaInventoryWidget::EquipItem(FName ItemId)
{
	return GetWistoriaCharacter() && GetWistoriaCharacter()->EquipInventoryItem(ItemId);
}

bool UWistoriaInventoryWidget::ReforgeWeaponSlot(int32 ItemIndex, int32 AffixIndexToReplace, int32 CostInGold)
{
	return GetWistoriaCharacter() && GetWistoriaCharacter()->ReforgeWeaponSlot(ItemIndex, AffixIndexToReplace, CostInGold);
}

TArray<FWistoriaQuestRecord> UWistoriaQuestJournalWidget::GetQuestRecords() const
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		if (const UWistoriaQuestComponent* Quests = Character->FindComponentByClass<UWistoriaQuestComponent>())
		{
			return Quests->GetQuests();
		}
	}

	return TArray<FWistoriaQuestRecord>();
}

bool UWistoriaQuestJournalWidget::TrackObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount)
{
	return GetWistoriaCharacter() && GetWistoriaCharacter()->AdvanceQuestObjective(QuestId, ObjectiveId, Amount);
}

int32 UWistoriaSkillTreeWidget::GetAvailableSkillPoints() const
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		if (const UWistoriaSkillComponent* Skills = Character->FindComponentByClass<UWistoriaSkillComponent>())
		{
			return Skills->GetAvailableSkillPoints();
		}
	}

	return 0;
}

TMap<FName, FWistoriaSkillNode> UWistoriaSkillTreeWidget::GetSkillTree() const
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		if (const UWistoriaSkillComponent* Skills = Character->FindComponentByClass<UWistoriaSkillComponent>())
		{
			return Skills->GetSkillTree();
		}
	}

	return TMap<FName, FWistoriaSkillNode>();
}

bool UWistoriaSkillTreeWidget::PurchaseSkill(FName SkillId)
{
	return GetWistoriaCharacter() && GetWistoriaCharacter()->PurchaseSkill(SkillId);
}

void UWistoriaBossHUDWidget::SetBoss(AWistoriaBossEnemy* NewBoss)
{
	if (Boss)
	{
		Boss->OnBossHealthChanged.RemoveDynamic(this, &UWistoriaBossHUDWidget::HandleBossHealthChanged);
	}

	Boss = NewBoss;
	if (Boss)
	{
		Boss->OnBossHealthChanged.AddDynamic(this, &UWistoriaBossHUDWidget::HandleBossHealthChanged);
	}

	OnBossHealthPercentChanged(GetBossHealthPercent());
}

float UWistoriaBossHUDWidget::GetBossHealthPercent() const
{
	if (!Boss || Boss->GetMaxHealthValue() <= 0.0f)
	{
		return 0.0f;
	}

	return Boss->GetCurrentHealthValue() / Boss->GetMaxHealthValue();
}

void UWistoriaBossHUDWidget::HandleBossHealthChanged(float CurrentHealth, float MaxHealth)
{
	OnBossHealthPercentChanged(MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f);
}

void UWistoriaDialogueWidget::OpenDialogue(ADialogueNPC* NewNPC, const TArray<FText>& NewDialogueLines)
{
	NPC = NewNPC;
	DialogueLines = NewDialogueLines;
}

TArray<FText> UWistoriaDialogueWidget::GetDialogueLines() const
{
	return DialogueLines;
}

void UWistoriaShopWidget::SetVendor(AShopVendorNPC* NewVendor)
{
	Vendor = NewVendor;
}

TArray<FWistoriaShopStockItem> UWistoriaShopWidget::GetMerchantInventory() const
{
	return Vendor ? Vendor->GetMerchantInventory() : TArray<FWistoriaShopStockItem>();
}

bool UWistoriaShopWidget::BuyStockItem(FName ItemId)
{
	return Vendor && Vendor->BuyStockItem(GetWistoriaCharacter(), ItemId);
}

bool UWistoriaShopWidget::BuyItem(const FWistoriaInventoryItem& Item, int32 Price)
{
	return Vendor && Vendor->BuyItem(GetWistoriaCharacter(), Item, Price);
}

bool UWistoriaShopWidget::SellItem(FName ItemId, int32 Quantity, int32 PricePerItem)
{
	return Vendor && Vendor->SellItem(GetWistoriaCharacter(), ItemId, Quantity, PricePerItem);
}

bool UWistoriaShopWidget::SellItemToVendor(FName ItemId, int32 Quantity)
{
	return Vendor && Vendor->SellItemToVendor(GetWistoriaCharacter(), ItemId, Quantity);
}

void UWistoriaFastTravelWidget::SetAvailableGates(const TArray<FWistoriaTeleportPoint>& NewAvailableGates)
{
	AvailableGates = NewAvailableGates;
}

TArray<FWistoriaTeleportPoint> UWistoriaFastTravelWidget::GetAvailableGates() const
{
	return AvailableGates;
}

bool UWistoriaFastTravelWidget::TravelToGate(FName GateId)
{
	AWistoriaCharacter* Character = GetWistoriaCharacter();
	if (!Character || GateId.IsNone())
	{
		return false;
	}

	for (const FWistoriaTeleportPoint& Gate : AvailableGates)
	{
		if (Gate.GateId == GateId)
		{
			return Character->TeleportToGate(Gate);
		}
	}

	return false;
}

TArray<AWistoriaCompanionCharacter*> UWistoriaCompanionCommandWidget::GetCompanions() const
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->GetActiveCompanions();
	}

	return TArray<AWistoriaCompanionCharacter*>();
}

int32 UWistoriaCompanionCommandWidget::CommandFollow()
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->CommandCompanionsToFollow();
	}

	return 0;
}

int32 UWistoriaCompanionCommandWidget::CommandHoldPosition()
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->CommandCompanionsToHoldPosition();
	}

	return 0;
}

int32 UWistoriaCompanionCommandWidget::CommandAttackLockedTarget()
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->CommandCompanionsToAttackLockedTarget();
	}

	return 0;
}

int32 UWistoriaCompanionCommandWidget::CommandDefendPlayer()
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->CommandCompanionsToDefendPlayer();
	}

	return 0;
}

int32 UWistoriaCompanionCommandWidget::CommandInteractWithTarget(AActor* TargetActor)
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->IssueCompanionCommand(EWistoriaCompanionCommand::InteractWithTarget, TargetActor);
	}

	return 0;
}

bool UWistoriaCompanionCommandWidget::ActivateTeamUltimate(FName CompanionId)
{
	return GetWistoriaCharacter() && GetWistoriaCharacter()->TriggerTeamUltimateCombo(CompanionId);
}

TMap<FName, FWistoriaCompanionBond> UWistoriaCompanionCommandWidget::GetCompanionBonds() const
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->GetCompanionBonds();
	}

	return TMap<FName, FWistoriaCompanionBond>();
}

int32 UWistoriaCompanionCommandWidget::IssueCommand(EWistoriaCompanionCommand Command, AActor* CommandTarget)
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->IssueCompanionCommand(Command, CommandTarget);
	}

	return 0;
}

void UWistoriaCharacterCreatorWidget::SetHairColor(const FLinearColor& NewHairColor)
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		Character->SetHairColor(NewHairColor);
	}
}

void UWistoriaCharacterCreatorWidget::SetRobeColor(const FLinearColor& NewRobeColor)
{
	if (AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		Character->SetRobeColor(NewRobeColor);
	}
}

FLinearColor UWistoriaCharacterCreatorWidget::GetHairColor() const
{
	if (const AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->GetHairColor();
	}

	return FLinearColor::White;
}

FLinearColor UWistoriaCharacterCreatorWidget::GetRobeColor() const
{
	if (const AWistoriaCharacter* Character = GetWistoriaCharacter())
	{
		return Character->GetRobeColor();
	}

	return FLinearColor::White;
}

TArray<FWistoriaSaveSlotInfo> UWistoriaSaveSlotManagerWidget::GetSaveSlots(int32 SlotCount) const
{
	return UWistoriaSaveLibrary::GetSaveSlotInfos(SlotCount);
}

bool UWistoriaSaveSlotManagerWidget::SelectSlot(int32 SlotIndex, bool bCreateIfEmpty)
{
	const FString SlotName = UWistoriaSaveLibrary::GetIndexedSaveSlot(SlotIndex);
	if (bCreateIfEmpty && !UWistoriaSaveLibrary::DoesSaveExist(SlotName) && !UWistoriaSaveLibrary::CreateNewSave(SlotName))
	{
		return false;
	}

	if (UWistoriaGameInstance* WistoriaGameInstance = GetWistoriaGameInstance())
	{
		WistoriaGameInstance->SetActiveSaveSlot(SlotName);
		return true;
	}

	return false;
}

bool UWistoriaSaveSlotManagerWidget::StartNewGameInSlot(int32 SlotIndex)
{
	const FString SlotName = UWistoriaSaveLibrary::GetIndexedSaveSlot(SlotIndex);
	if (!UWistoriaSaveLibrary::CreateNewSave(SlotName))
	{
		return false;
	}

	return SelectSlot(SlotIndex, false);
}

bool UWistoriaSaveSlotManagerWidget::LoadGameInSlot(int32 SlotIndex)
{
	const FString SlotName = UWistoriaSaveLibrary::GetIndexedSaveSlot(SlotIndex);
	return UWistoriaSaveLibrary::DoesSaveExist(SlotName) && SelectSlot(SlotIndex, false);
}

bool UWistoriaSaveSlotManagerWidget::DeleteSaveSlot(int32 SlotIndex)
{
	return UWistoriaSaveLibrary::DeleteSave(UWistoriaSaveLibrary::GetIndexedSaveSlot(SlotIndex));
}

void UWistoriaSaveSlotManagerWidget::OpenGameplayLevel()
{
	if (GameplayLevelName != NAME_None)
	{
		UGameplayStatics::OpenLevel(this, GameplayLevelName);
	}
}

UWistoriaGameInstance* UWistoriaSaveSlotManagerWidget::GetWistoriaGameInstance() const
{
	return Cast<UWistoriaGameInstance>(GetGameInstance());
}

FWistoriaRaidSnapshot UWistoriaRaidHUDWidget::RefreshRaidSnapshot()
{
	FWistoriaRaidSnapshot Snapshot;
	if (AWistoriaRaidGameMode* RaidGameMode = GetRaidGameMode())
	{
		Snapshot = RaidGameMode->GetRaidSnapshot();
	}

	OnRaidSnapshotRefreshed(Snapshot);
	return Snapshot;
}

AWistoriaRaidGameMode* UWistoriaRaidHUDWidget::GetRaidGameMode() const
{
	return GetWorld() ? GetWorld()->GetAuthGameMode<AWistoriaRaidGameMode>() : nullptr;
}
