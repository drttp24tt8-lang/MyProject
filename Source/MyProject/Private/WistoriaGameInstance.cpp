#include "WistoriaGameInstance.h"

#include "WistoriaSaveLibrary.h"

void UWistoriaGameInstance::SetActiveSaveSlot(const FString& NewSlotName)
{
	if (!NewSlotName.IsEmpty())
	{
		ActiveSaveSlot = NewSlotName;
	}
}

void UWistoriaGameInstance::SetActiveSaveSlotByIndex(int32 SlotIndex)
{
	SetActiveSaveSlot(UWistoriaSaveLibrary::GetIndexedSaveSlot(SlotIndex));
}

FString UWistoriaGameInstance::GetActiveSaveSlot() const
{
	return ActiveSaveSlot;
}

void UWistoriaGameInstance::SetPendingNewGamePlus(bool bNewPendingState)
{
	bPendingNewGamePlus = bNewPendingState;
}

bool UWistoriaGameInstance::IsNewGamePlusPending() const
{
	return bPendingNewGamePlus;
}
