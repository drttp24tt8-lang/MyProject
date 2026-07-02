#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaBetaTestProbe.generated.h"

class UDataTable;

UCLASS()
class MYPROJECT_API AWistoriaBetaTestProbe : public AActor
{
	GENERATED_BODY()

public:
	AWistoriaBetaTestProbe();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Beta")
	void RunBetaSmokeTest();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Beta")
	bool bRunOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Beta")
	TObjectPtr<UDataTable> WeaponDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Beta")
	TObjectPtr<UDataTable> SpellDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Beta")
	int32 ExpectedWeaponRows = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Beta")
	int32 ExpectedSpellRows = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Beta")
	float MessageDuration = 8.0f;

private:
	void AddResult(const FString& Label, bool bPassed, TArray<FString>& OutResults) const;
};
