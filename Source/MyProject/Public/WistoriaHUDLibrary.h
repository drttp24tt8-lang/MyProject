#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WistoriaTypes.h"
#include "WistoriaHUDLibrary.generated.h"

class AWistoriaCharacter;

UCLASS()
class MYPROJECT_API UWistoriaHUDLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Wistoria|HUD")
	static FWistoriaHUDSnapshot MakeHUDSnapshot(AWistoriaCharacter* Character);

	UFUNCTION(BlueprintPure, Category = "Wistoria|HUD")
	static FText FormatQuestObjective(const FWistoriaQuestRecord& Quest);
};
