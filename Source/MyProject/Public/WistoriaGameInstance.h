#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WistoriaGameInstance.generated.h"

UCLASS()
class MYPROJECT_API UWistoriaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	void SetActiveSaveSlot(const FString& NewSlotName);

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Save")
	void SetActiveSaveSlotByIndex(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Save")
	FString GetActiveSaveSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Game")
	void SetPendingNewGamePlus(bool bNewPendingState);

	UFUNCTION(BlueprintPure, Category = "Wistoria|Game")
	bool IsNewGamePlusPending() const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Save", meta = (AllowPrivateAccess = "true"))
	FString ActiveSaveSlot = TEXT("WistoriaSlot0");

	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Game", meta = (AllowPrivateAccess = "true"))
	bool bPendingNewGamePlus = false;
};
