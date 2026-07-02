#include "Coin.h"

#include "CoinCollector.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

ACoin::ACoin()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Trigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACoin::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnOverlapBegin);
}

void ACoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
}

void ACoin::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCollected || !OtherActor || OtherActor == this)
	{
		return;
	}

	const bool bImplementsCollector = OtherActor->GetClass()->ImplementsInterface(UCoinCollector::StaticClass());
	const bool bIsPawn = OtherActor->IsA<APawn>();

	if (!bImplementsCollector && (bRequireCollectorInterface || !bIsPawn))
	{
		return;
	}

	bCollected = true;
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	const FVector SpawnLocation = GetActorLocation();

	if (bImplementsCollector)
	{
		ICoinCollector::Execute_AddCoins(OtherActor, ScoreValue);
	}

	OnCoinCollected.Broadcast(OtherActor, ScoreValue);

	if (CollectSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CollectSound, SpawnLocation);
	}

	if (CollectEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CollectEffect, SpawnLocation);
	}

	Destroy();
}
