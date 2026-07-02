#include "TreasureChest.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "WistoriaCharacter.h"
#include "WistoriaInventoryComponent.h"
#include "WistoriaSaveGame.h"

ATreasureChest::ATreasureChest()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	RootComponent = InteractionVolume;
	InteractionVolume->SetBoxExtent(FVector(90.0f, 90.0f, 80.0f));
	InteractionVolume->SetCollisionProfileName(TEXT("Trigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	ClosedInteractionText = NSLOCTEXT("WistoriaTreasure", "ClosedChestInteraction", "Open Chest");
	OpenedInteractionText = NSLOCTEXT("WistoriaTreasure", "OpenedChestInteraction", "Empty Chest");
}

void ATreasureChest::BeginPlay()
{
	Super::BeginPlay();
	LoadOpenedState();
}

void ATreasureChest::Interact_Implementation(AActor* InteractingActor)
{
	OpenChest(InteractingActor);
}

FText ATreasureChest::GetInteractionText_Implementation() const
{
	return bOpened ? OpenedInteractionText : ClosedInteractionText;
}

bool ATreasureChest::OpenChest(AActor* Opener)
{
	if (bOpened)
	{
		return false;
	}

	AWistoriaCharacter* Character = Cast<AWistoriaCharacter>(Opener);
	if (!Character)
	{
		return false;
	}

	for (const FWistoriaInventoryItem& RewardItem : RewardItems)
	{
		Character->AddInventoryItem(RewardItem);
	}

	if (GoldReward > 0)
	{
		if (UWistoriaInventoryComponent* Inventory = Character->FindComponentByClass<UWistoriaInventoryComponent>())
		{
			Inventory->AddGold(GoldReward);
		}
	}

	bOpened = true;
	SaveOpenedState();

	const FVector SpawnLocation = GetActorLocation();
	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, SpawnLocation);
	}
	if (OpenEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, OpenEffect, SpawnLocation);
	}

	OnChestOpened(Character);
	return true;
}

bool ATreasureChest::IsOpened() const
{
	return bOpened;
}

void ATreasureChest::LoadOpenedState()
{
	if (!bPersistOpenedState || ChestId.IsNone() || SaveSlotName.IsEmpty())
	{
		return;
	}

	UWistoriaSaveGame* SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (SaveGame && SaveGame->CompletedStoryFlags.Contains(ChestId))
	{
		bOpened = true;
	}
}

void ATreasureChest::SaveOpenedState() const
{
	if (!bPersistOpenedState || ChestId.IsNone() || SaveSlotName.IsEmpty())
	{
		return;
	}

	UWistoriaSaveGame* SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (!SaveGame)
	{
		SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::CreateSaveGameObject(UWistoriaSaveGame::StaticClass()));
	}

	if (!SaveGame)
	{
		return;
	}

	SaveGame->CompletedStoryFlags.AddUnique(ChestId);
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, 0);
}
