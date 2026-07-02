#include "WistoriaSaveLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "WistoriaCharacter.h"
#include "WistoriaInventoryComponent.h"
#include "WistoriaQuestComponent.h"
#include "WistoriaSaveGame.h"

FString UWistoriaSaveLibrary::GetDefaultSaveSlot()
{
	return TEXT("WistoriaSlot0");
}

FString UWistoriaSaveLibrary::GetIndexedSaveSlot(int32 SlotIndex)
{
	const int32 ClampedIndex = FMath::Clamp(SlotIndex, 1, 3);
	return FString::Printf(TEXT("WistoriaSlot_%d"), ClampedIndex);
}

FWistoriaSaveSlotInfo UWistoriaSaveLibrary::GetSaveSlotInfo(int32 SlotIndex)
{
	FWistoriaSaveSlotInfo SlotInfo;
	SlotInfo.SlotIndex = FMath::Clamp(SlotIndex, 1, 3);
	SlotInfo.SlotName = GetIndexedSaveSlot(SlotInfo.SlotIndex);
	SlotInfo.bHasSave = DoesSaveExist(SlotInfo.SlotName);

	if (!SlotInfo.bHasSave)
	{
		SlotInfo.SlotLabel = FText::FromString(FString::Printf(TEXT("Slot %d: New Game"), SlotInfo.SlotIndex));
		return SlotInfo;
	}

	const UWistoriaSaveGame* SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotInfo.SlotName, 0));
	if (!SaveGame)
	{
		SlotInfo.bHasSave = false;
		SlotInfo.SlotLabel = FText::FromString(FString::Printf(TEXT("Slot %d: Unreadable Save"), SlotInfo.SlotIndex));
		return SlotInfo;
	}
	if (!SaveGame->bSaveStarted)
	{
		SlotInfo.bHasSave = false;
		SlotInfo.SlotLabel = FText::FromString(FString::Printf(TEXT("Slot %d: New Game"), SlotInfo.SlotIndex));
		return SlotInfo;
	}

	SlotInfo.CharacterLevel = SaveGame->CharacterLevel;
	SlotInfo.CurrentXP = SaveGame->CurrentXP;
	SlotInfo.Gold = SaveGame->Gold;
	SlotInfo.NewGamePlusCount = SaveGame->NewGamePlusCount;
	SlotInfo.TotalRaidClearsCount = SaveGame->TotalRaidClearsCount;
	SlotInfo.CurrentAcademyRankId = SaveGame->CurrentAcademyRankId;

	for (const FWistoriaAcademyRank& Rank : SaveGame->AcademyRanks)
	{
		if (Rank.RankId == SlotInfo.CurrentAcademyRankId)
		{
			SlotInfo.CurrentAcademyRankTitle = Rank.RankTitle;
			break;
		}
	}

	const FString RankText = SlotInfo.CurrentAcademyRankTitle.IsEmpty()
		? TEXT("Scholar")
		: SlotInfo.CurrentAcademyRankTitle.ToString();
	SlotInfo.SlotLabel = FText::FromString(FString::Printf(
		TEXT("Slot %d: Level %d %s"),
		SlotInfo.SlotIndex,
		SlotInfo.CharacterLevel,
		*RankText));
	return SlotInfo;
}

TArray<FWistoriaSaveSlotInfo> UWistoriaSaveLibrary::GetSaveSlotInfos(int32 SlotCount)
{
	TArray<FWistoriaSaveSlotInfo> SlotInfos;
	const int32 ClampedCount = FMath::Clamp(SlotCount, 1, 3);
	SlotInfos.Reserve(ClampedCount);

	for (int32 SlotIndex = 1; SlotIndex <= ClampedCount; ++SlotIndex)
	{
		SlotInfos.Add(GetSaveSlotInfo(SlotIndex));
	}

	return SlotInfos;
}

bool UWistoriaSaveLibrary::DoesSaveExist(const FString& SlotName)
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

bool UWistoriaSaveLibrary::CreateNewSave(const FString& SlotName)
{
	if (SlotName.IsEmpty())
	{
		return false;
	}

	UWistoriaSaveGame* NewSave = Cast<UWistoriaSaveGame>(UGameplayStatics::CreateSaveGameObject(UWistoriaSaveGame::StaticClass()));
	if (NewSave)
	{
		NewSave->bSaveStarted = true;
	}
	return NewSave && UGameplayStatics::SaveGameToSlot(NewSave, SlotName, 0);
}

bool UWistoriaSaveLibrary::SavePlayer(AWistoriaCharacter* Player, const FString& SlotName)
{
	if (!Player)
	{
		return false;
	}

	Player->SaveProgress(SlotName);
	return true;
}

bool UWistoriaSaveLibrary::LoadPlayer(AWistoriaCharacter* Player, const FString& SlotName)
{
	return Player && Player->LoadProgress(SlotName);
}

bool UWistoriaSaveLibrary::DeleteSave(const FString& SlotName)
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0)
		&& UGameplayStatics::DeleteGameInSlot(SlotName, 0);
}

bool UWistoriaSaveLibrary::StartNewGamePlus(AWistoriaCharacter* Player, const FString& SlotName)
{
	if (!Player)
	{
		return false;
	}

	UWistoriaSaveGame* ExistingSave = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	UWistoriaSaveGame* NewSave = Cast<UWistoriaSaveGame>(UGameplayStatics::CreateSaveGameObject(UWistoriaSaveGame::StaticClass()));
	if (!NewSave)
	{
		return false;
	}

	if (ExistingSave)
	{
		NewSave->bSaveStarted = true;
		NewSave->CharacterLevel = ExistingSave->CharacterLevel;
		NewSave->CurrentXP = ExistingSave->CurrentXP;
		NewSave->Coins = ExistingSave->Coins;
		NewSave->AcademyRankScore = 0;
		NewSave->TotalRaidClearsCount = 0;
		NewSave->AcademyRanks = ExistingSave->AcademyRanks;
		NewSave->CurrentAcademyRankId = ExistingSave->CurrentAcademyRankId;
		NewSave->UnlockedCosmeticCapeIds = ExistingSave->UnlockedCosmeticCapeIds;
		NewSave->WeaponLog = ExistingSave->WeaponLog;
		NewSave->InventoryItems = ExistingSave->InventoryItems;
		NewSave->EquippedItems = ExistingSave->EquippedItems;
		NewSave->Gold = ExistingSave->Gold;
		NewSave->HairColor = ExistingSave->HairColor;
		NewSave->RobeColor = ExistingSave->RobeColor;
		NewSave->AvailableSkillPoints = ExistingSave->AvailableSkillPoints;
		NewSave->SkillTree = ExistingSave->SkillTree;
		NewSave->CompanionBonds = ExistingSave->CompanionBonds;
		NewSave->UnlockedTeleportPoints = ExistingSave->UnlockedTeleportPoints;
		NewSave->NewGamePlusCount = ExistingSave->NewGamePlusCount + 1;
	}
	else
	{
		Player->SaveProgress(SlotName);
		ExistingSave = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (!ExistingSave)
		{
			return false;
		}
		NewSave->bSaveStarted = true;
		NewSave->CharacterLevel = ExistingSave->CharacterLevel;
		NewSave->CurrentXP = ExistingSave->CurrentXP;
		NewSave->Coins = ExistingSave->Coins;
		NewSave->AcademyRankScore = 0;
		NewSave->TotalRaidClearsCount = 0;
		NewSave->AcademyRanks = ExistingSave->AcademyRanks;
		NewSave->CurrentAcademyRankId = ExistingSave->CurrentAcademyRankId;
		NewSave->UnlockedCosmeticCapeIds = ExistingSave->UnlockedCosmeticCapeIds;
		NewSave->WeaponLog = ExistingSave->WeaponLog;
		NewSave->InventoryItems = ExistingSave->InventoryItems;
		NewSave->EquippedItems = ExistingSave->EquippedItems;
		NewSave->Gold = ExistingSave->Gold;
		NewSave->HairColor = ExistingSave->HairColor;
		NewSave->RobeColor = ExistingSave->RobeColor;
		NewSave->AvailableSkillPoints = ExistingSave->AvailableSkillPoints;
		NewSave->SkillTree = ExistingSave->SkillTree;
		NewSave->CompanionBonds = ExistingSave->CompanionBonds;
		NewSave->UnlockedTeleportPoints = ExistingSave->UnlockedTeleportPoints;
		NewSave->NewGamePlusCount = 1;
	}

	// Story and quest records intentionally reset for the next loop.
	NewSave->QuestRecords.Empty();
	NewSave->CompletedStoryFlags.Empty();

	const bool bSaved = UGameplayStatics::SaveGameToSlot(NewSave, SlotName, 0);
	if (bSaved)
	{
		Player->LoadProgress(SlotName);
	}
	return bSaved;
}
