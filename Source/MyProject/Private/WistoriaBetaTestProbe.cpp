#include "WistoriaBetaTestProbe.h"

#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "WistoriaCharacter.h"
#include "WistoriaGameInstance.h"
#include "WistoriaSaveLibrary.h"

AWistoriaBetaTestProbe::AWistoriaBetaTestProbe()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWistoriaBetaTestProbe::BeginPlay()
{
	Super::BeginPlay();

	if (bRunOnBeginPlay)
	{
		RunBetaSmokeTest();
	}
}

void AWistoriaBetaTestProbe::RunBetaSmokeTest()
{
	TArray<FString> Results;

	const AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	AddResult(TEXT("Player is AWistoriaCharacter"), PlayerCharacter != nullptr, Results);

	const UWistoriaGameInstance* WistoriaGameInstance = Cast<UWistoriaGameInstance>(GetGameInstance());
	AddResult(TEXT("GameInstance is UWistoriaGameInstance"), WistoriaGameInstance != nullptr, Results);

	if (WistoriaGameInstance)
	{
		AddResult(TEXT("Active save slot is set"), !WistoriaGameInstance->GetActiveSaveSlot().IsEmpty(), Results);
	}

	if (WeaponDataTable)
	{
		AddResult(
			FString::Printf(TEXT("Weapon table rows >= %d"), ExpectedWeaponRows),
			WeaponDataTable->GetRowMap().Num() >= ExpectedWeaponRows,
			Results);
	}
	else
	{
		Results.Add(TEXT("[WARN] Weapon data table not assigned to probe."));
	}

	if (SpellDataTable)
	{
		AddResult(
			FString::Printf(TEXT("Spell table rows >= %d"), ExpectedSpellRows),
			SpellDataTable->GetRowMap().Num() >= ExpectedSpellRows,
			Results);
	}
	else
	{
		Results.Add(TEXT("[WARN] Spell data table not assigned to probe."));
	}

	AddResult(TEXT("Indexed save slot 1 resolves"), UWistoriaSaveLibrary::GetIndexedSaveSlot(1) == TEXT("WistoriaSlot_1"), Results);
	AddResult(TEXT("Indexed save slot 3 resolves"), UWistoriaSaveLibrary::GetIndexedSaveSlot(3) == TEXT("WistoriaSlot_3"), Results);

	const FString CombinedResults = FString::Join(Results, TEXT("\n"));
	UE_LOG(LogTemp, Display, TEXT("Wand and Wistoria beta smoke test:\n%s"), *CombinedResults);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, MessageDuration, FColor::Cyan, CombinedResults);
	}
}

void AWistoriaBetaTestProbe::AddResult(const FString& Label, bool bPassed, TArray<FString>& OutResults) const
{
	OutResults.Add(FString::Printf(TEXT("[%s] %s"), bPassed ? TEXT("PASS") : TEXT("FAIL"), *Label));
}
