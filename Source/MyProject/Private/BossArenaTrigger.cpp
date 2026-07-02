#include "BossArenaTrigger.h"

#include "Components/BoxComponent.h"
#include "WistoriaCharacter.h"

ABossArenaTrigger::ABossArenaTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetBoxExtent(FVector(900.0f, 900.0f, 350.0f));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
}

void ABossArenaTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABossArenaTrigger::HandleBeginOverlap);
}

void ABossArenaTrigger::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if ((bHasTriggered && bTriggerOnce) || !OtherActor || !OtherActor->IsA<AWistoriaCharacter>())
	{
		return;
	}

	bHasTriggered = true;
	OnBossArenaEntered(OtherActor, BossActor);

	if (bTriggerOnce)
	{
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
