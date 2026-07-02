#include "WistoriaSpellProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "WistoriaEnemy.h"
#include "WistoriaStatusComponent.h"

AWistoriaSpellProjectile::AWistoriaSpellProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSphereRadius(18.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->SetNotifyRigidBodyCollision(true);

	TrailEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffectComponent"));
	TrailEffectComponent->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->InitialSpeed = 1800.0f;
	MovementComponent->MaxSpeed = 1800.0f;
	MovementComponent->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 4.0f;
}

void AWistoriaSpellProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentHit.AddDynamic(this, &AWistoriaSpellProjectile::HandleHit);
}

void AWistoriaSpellProjectile::ConfigureProjectile(float InDamage, EWistoriaElement InElement, AActor* InInstigatorActor)
{
	Damage = FMath::Max(0.0f, InDamage);
	Element = InElement;
	InstigatorActor = InInstigatorActor;
}

void AWistoriaSpellProjectile::ConfigureProjectileFromSpellRow(const FWistoriaSpellDataRow& SpellRow, float DamageMultiplier, AActor* InInstigatorActor)
{
	Damage = FMath::Max(0.0f, SpellRow.BaseDamage * DamageMultiplier);
	Element = SpellRow.Element;
	StatusToApply = SpellRow.StatusToApply;
	StatusDuration = SpellRow.StatusDuration;
	StatusChance = FMath::Clamp(SpellRow.StatusChance, 0.0f, 1.0f);
	InstigatorActor = InInstigatorActor;
}

void AWistoriaSpellProjectile::HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == InstigatorActor)
	{
		return;
	}

	if (AWistoriaEnemy* Enemy = Cast<AWistoriaEnemy>(OtherActor))
	{
		Enemy->ReceiveDamageFrom(Damage, Element, InstigatorActor);
		TryApplyStatusToEnemy(Enemy);
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
	}

	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}

	Destroy();
}

EWistoriaElement AWistoriaSpellProjectile::ResolveDefaultStatusForElement() const
{
	if (Element == EWistoriaElement::Fire)
	{
		return EWistoriaElement::Burn;
	}
	if (Element == EWistoriaElement::Ice)
	{
		return EWistoriaElement::Freeze;
	}
	if (Element == EWistoriaElement::Miasma)
	{
		return EWistoriaElement::Miasma;
	}
	return EWistoriaElement::None;
}

float AWistoriaSpellProjectile::ResolveDefaultStatusDuration(EWistoriaElement Status) const
{
	if (Status == EWistoriaElement::Burn)
	{
		return 5.0f;
	}
	if (Status == EWistoriaElement::Freeze)
	{
		return 3.0f;
	}
	if (Status == EWistoriaElement::Miasma)
	{
		return 8.0f;
	}
	return 0.0f;
}

void AWistoriaSpellProjectile::TryApplyStatusToEnemy(AWistoriaEnemy* Enemy) const
{
	if (!Enemy)
	{
		return;
	}

	const EWistoriaElement ResolvedStatus = StatusToApply == EWistoriaElement::None
		? ResolveDefaultStatusForElement()
		: StatusToApply;
	if (ResolvedStatus == EWistoriaElement::None)
	{
		return;
	}

	const float ResolvedDuration = StatusDuration > 0.0f
		? StatusDuration
		: ResolveDefaultStatusDuration(ResolvedStatus);
	if (ResolvedDuration <= 0.0f || FMath::FRand() > StatusChance)
	{
		return;
	}

	if (UWistoriaStatusComponent* StatusComponent = Enemy->FindComponentByClass<UWistoriaStatusComponent>())
	{
		StatusComponent->ApplyStatus(ResolvedStatus, ResolvedDuration);
	}
}
