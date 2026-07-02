#include "CompanionPartySpawner.h"

#include "Components/BoxComponent.h"
#include "WistoriaCharacter.h"
#include "WistoriaCompanionCharacter.h"

ACompanionPartySpawner::ACompanionPartySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	ActivationVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationVolume"));
	RootComponent = ActivationVolume;
	ActivationVolume->SetBoxExtent(FVector(300.0f, 300.0f, 180.0f));
	ActivationVolume->SetCollisionProfileName(TEXT("Trigger"));

	LocalSpawnOffsets.Add(FVector(160.0f, -120.0f, 80.0f));
	LocalSpawnOffsets.Add(FVector(160.0f, 120.0f, 80.0f));
	LocalSpawnOffsets.Add(FVector(-160.0f, 0.0f, 80.0f));
}

void ACompanionPartySpawner::BeginPlay()
{
	Super::BeginPlay();
	ActivationVolume->OnComponentBeginOverlap.AddDynamic(this, &ACompanionPartySpawner::OnActivationOverlap);
}

int32 ACompanionPartySpawner::SpawnCompanionParty(AWistoriaCharacter* PlayerCharacter)
{
	if (bHasSpawnedParty || !PlayerCharacter || !GetWorld() || CompanionClasses.Num() == 0)
	{
		return 0;
	}

	const int32 SpawnCount = FMath::Min3(MaxCompanionsToSpawn, CompanionClasses.Num(), PlayerCharacter->GetAvailableCompanionSlots());
	int32 SpawnedCount = 0;
	for (int32 Index = 0; Index < SpawnCount; ++Index)
	{
		const TSubclassOf<AWistoriaCompanionCharacter> CompanionClass = CompanionClasses[Index];
		if (!CompanionClass)
		{
			continue;
		}

		AWistoriaCompanionCharacter* Companion = GetWorld()->SpawnActor<AWistoriaCompanionCharacter>(
			CompanionClass,
			GetSpawnLocation(Index),
			GetActorRotation());

		if (!Companion)
		{
			continue;
		}

		if (PlayerCharacter->RecruitCompanion(Companion))
		{
			SpawnedCount++;
		}
		else
		{
			Companion->Destroy();
		}
	}

	bHasSpawnedParty = SpawnedCount > 0;
	return SpawnedCount;
}

void ACompanionPartySpawner::OnActivationOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bSpawnOnPlayerOverlap)
	{
		return;
	}

	if (AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(OtherActor))
	{
		SpawnCompanionParty(PlayerCharacter);
	}
}

FVector ACompanionPartySpawner::GetSpawnLocation(int32 SpawnIndex) const
{
	if (LocalSpawnOffsets.Num() == 0)
	{
		return GetActorLocation();
	}

	return GetActorTransform().TransformPosition(LocalSpawnOffsets[SpawnIndex % LocalSpawnOffsets.Num()]);
}
