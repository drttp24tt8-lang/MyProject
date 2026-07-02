#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WistoriaTypes.h"
#include "WistoriaSaveLibrary.generated.h"

class AWistoriaCharacter;
class UWistoriaSaveGame;

UCLASS()
class MYPROJECT_API UWistoriaSaveLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Wistoria|Save")
	static FString GetDefaultSaveSlot();

	UFUNCTION(BlueprintPure, Category = "Wistoria|Save")
	static FString GetIndexedSaveSlot(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Save")
	static FWistoriaSaveSlotInfo GetSaveSlotInfo(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Save")
	static TArray<FWistoriaSaveSlotInfo> GetSaveSlotInfos(int32 SlotCount = 3);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	static bool DoesSaveExist(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	static bool CreateNewSave(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	static bool SavePlayer(AWistoriaCharacter* Player, const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	static bool LoadPlayer(AWistoriaCharacter* Player, const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	static bool DeleteSave(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	static bool StartNewGamePlus(AWistoriaCharacter* Player, const FString& SlotName);
};
