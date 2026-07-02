#include "WorldLootPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "WistoriaCharacter.h"
#include "WistoriaInventoryComponent.h"

AWorldLootPickup::AWorldLootPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(80.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Trigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InteractionText = NSLOCTEXT("WistoriaLoot", "LootPickupInteraction", "Collect Loot");
}

void AWorldLootPickup::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AWorldLootPickup::OnOverlapBegin);
}

void AWorldLootPickup::ConfigureLoot(const FWistoriaInventoryItem& NewItem, int32 NewGoldAmount)
{
	Item = NewItem;
	GoldAmount = FMath::Max(0, NewGoldAmount);
}

void AWorldLootPickup::Interact_Implementation(AActor* InteractingActor)
{
	Collect(InteractingActor);
}

FText AWorldLootPickup::GetInteractionText_Implementation() const
{
	return InteractionText;
}

bool AWorldLootPickup::Collect(AActor* Collector)
{
	AWistoriaCharacter* Character = Cast<AWistoriaCharacter>(Collector);
	if (!Character)
	{
		return false;
	}

	bool bGrantedReward = false;
	if (!Item.ItemId.IsNone() && Item.Quantity > 0)
	{
		Character->AddInventoryItem(Item);
		bGrantedReward = true;
	}

	if (GoldAmount > 0)
	{
		if (UWistoriaInventoryComponent* Inventory = Character->FindComponentByClass<UWistoriaInventoryComponent>())
		{
			Inventory->AddGold(GoldAmount);
			bGrantedReward = true;
		}
	}

	if (!bGrantedReward)
	{
		return false;
	}

	const FVector SpawnLocation = GetActorLocation();
	if (CollectSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CollectSound, SpawnLocation);
	}
	if (CollectEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CollectEffect, SpawnLocation);
	}

	OnCollected(Character);
	Destroy();
	return true;
}

void AWorldLootPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bAutoCollectOnOverlap)
	{
		Collect(OtherActor);
	}
}
