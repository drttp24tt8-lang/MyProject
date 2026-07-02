#include "WistoriaMeleeHitbox.h"

#include "WistoriaEnemy.h"

UWistoriaMeleeHitbox::UWistoriaMeleeHitbox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BoxExtent = FVector(75.0f, 45.0f, 45.0f);

	SetCollisionProfileName(TEXT("Trigger"));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetGenerateOverlapEvents(true);
}

void UWistoriaMeleeHitbox::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UWistoriaMeleeHitbox::HandleOverlap);
}

void UWistoriaMeleeHitbox::ActivateHitbox(float InDamage, EWistoriaElement InElement)
{
	Damage = FMath::Max(0.0f, InDamage);
	Element = InElement;
	DamagedActors.Empty();

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetGenerateOverlapEvents(true);
}

void UWistoriaMeleeHitbox::DeactivateHitbox()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamagedActors.Empty();
}

void UWistoriaMeleeHitbox::HandleOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	if (bIgnoreOwner && OtherActor == GetOwner())
	{
		return;
	}

	const TWeakObjectPtr<AActor> OtherActorKey(OtherActor);

	if (DamagedActors.Contains(OtherActorKey))
	{
		return;
	}

	AWistoriaEnemy* Enemy = Cast<AWistoriaEnemy>(OtherActor);
	if (!Enemy)
	{
		return;
	}

	DamagedActors.Add(OtherActorKey);
	Enemy->ReceiveDamageFrom(Damage, Element, GetOwner());
}