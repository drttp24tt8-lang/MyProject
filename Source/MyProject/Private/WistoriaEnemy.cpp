#include "WistoriaEnemy.h"

#include "WistoriaEnemyAIController.h"
#include "WistoriaCharacter.h"
#include "WistoriaStatusComponent.h"

AWistoriaEnemy::AWistoriaEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	StatusComponent = CreateDefaultSubobject<UWistoriaStatusComponent>(TEXT("StatusComponent"));
	AIControllerClass = AWistoriaEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AWistoriaEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void AWistoriaEnemy::ApplyScaling(float HealthMultiplier, float DamageMultiplier)
{
	MaxHealth *= FMath::Max(HealthMultiplier, 0.01f);
	BaseDamage *= FMath::Max(DamageMultiplier, 0.0f);
	CurrentHealth = MaxHealth;
}

void AWistoriaEnemy::ReceiveDamage(float DamageAmount, EWistoriaElement DamageElement)
{
	ReceiveDamageFrom(DamageAmount, DamageElement, nullptr);
}

void AWistoriaEnemy::ReceiveDamageFrom(float DamageAmount, EWistoriaElement DamageElement, AActor* DamageInstigator)
{
	if (DamageAmount <= 0.0f || IsDefeated())
	{
		return;
	}

	if (DamageInstigator)
	{
		LastDamageInstigator = DamageInstigator;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	OnDamaged(DamageAmount, DamageElement);

	if (IsDefeated())
	{
		GrantDefeatRewards();
		OnEnemyDefeated.Broadcast(this);
		OnDefeated();
		Destroy();
	}
}

bool AWistoriaEnemy::IsDefeated() const
{
	return CurrentHealth <= 0.0f;
}

float AWistoriaEnemy::GetCurrentHealthValue() const
{
	return CurrentHealth;
}

float AWistoriaEnemy::GetMaxHealthValue() const
{
	return MaxHealth;
}

bool AWistoriaEnemy::TryMeleeAttack(AActor* TargetActor)
{
	AWistoriaCharacter* TargetCharacter = Cast<AWistoriaCharacter>(TargetActor);
	if (!TargetCharacter || IsDefeated())
	{
		return false;
	}

	if (FVector::DistSquared(GetActorLocation(), TargetCharacter->GetActorLocation()) > FMath::Square(MeleeRange))
	{
		return false;
	}

	if (TargetCharacter->TryResolveParry(this))
	{
		OnAttackParried(TargetCharacter);
		return false;
	}

	TargetCharacter->ReceiveDamage(BaseDamage);
	return true;
}

void AWistoriaEnemy::GrantDefeatRewards()
{
	AWistoriaCharacter* Killer = Cast<AWistoriaCharacter>(LastDamageInstigator);
	if (!Killer)
	{
		return;
	}

	Killer->AddXP(XPReward);
	Killer->NotifyEnemyKilled(EnemyId);

	for (const FWistoriaInventoryItem& Loot : LootDrops)
	{
		Killer->AddInventoryItem(Loot);
	}

	if (bRollProceduralWeaponDrop && ProceduralWeaponDropPool.Num() > 0 && FMath::FRand() <= ProceduralWeaponDropChance)
	{
		const int32 DropIndex = FMath::RandRange(0, ProceduralWeaponDropPool.Num() - 1);
		Killer->AddInventoryItem(GenerateProceduralWeaponDrop(ProceduralWeaponDropPool[DropIndex]));
	}
}

FWistoriaInventoryItem AWistoriaEnemy::GenerateProceduralWeaponDrop(const FWistoriaInventoryItem& BaseWeapon) const
{
	FWistoriaInventoryItem Weapon = BaseWeapon;
	Weapon.ItemType = EWistoriaItemType::Weapon;
	Weapon.Quantity = 1;
	Weapon.Rarity = RollWeaponRarity();
	Weapon.Affixes.Reset();

	int32 AffixCount = 0;
	float RarityMultiplier = 1.0f;
	switch (Weapon.Rarity)
	{
	case EWistoriaItemRarity::Legendary:
		AffixCount = 3;
		RarityMultiplier = 2.0f;
		break;
	case EWistoriaItemRarity::Epic:
		AffixCount = 2;
		RarityMultiplier = 1.5f;
		break;
	case EWistoriaItemRarity::Rare:
		AffixCount = 1;
		RarityMultiplier = 1.2f;
		break;
	case EWistoriaItemRarity::Standard:
	default:
		AffixCount = 0;
		RarityMultiplier = 1.0f;
		break;
	}

	Weapon.PhysicalAttackBonus *= RarityMultiplier;
	Weapon.MagicPowerBonus *= RarityMultiplier;
	Weapon.CourageBonus *= RarityMultiplier;
	ApplyRandomAffixes(Weapon, AffixCount, RarityMultiplier);
	return Weapon;
}

EWistoriaItemRarity AWistoriaEnemy::RollWeaponRarity() const
{
	const float Roll = FMath::FRand();
	const float LegendaryThreshold = LegendaryDropRate;
	const float EpicThreshold = LegendaryThreshold + EpicDropRate;
	const float RareThreshold = EpicThreshold + RareDropRate;

	if (Roll <= LegendaryThreshold)
	{
		return EWistoriaItemRarity::Legendary;
	}
	if (Roll <= EpicThreshold)
	{
		return EWistoriaItemRarity::Epic;
	}
	if (Roll <= RareThreshold)
	{
		return EWistoriaItemRarity::Rare;
	}
	return EWistoriaItemRarity::Standard;
}

void AWistoriaEnemy::ApplyRandomAffixes(FWistoriaInventoryItem& Weapon, int32 AffixCount, float RarityMultiplier) const
{
	static const TArray<FName> AvailableStats =
	{
		TEXT("CritChance"),
		TEXT("SpellChantSpeed"),
		TEXT("CourageGainScale"),
		TEXT("PhysicalAttack"),
		TEXT("MagicPower")
	};

	for (int32 Index = 0; Index < AffixCount; ++Index)
	{
		const FName SelectedStat = AvailableStats[FMath::RandRange(0, AvailableStats.Num() - 1)];
		const float Bonus = FMath::FRandRange(0.05f, 0.15f) * RarityMultiplier;

		FWistoriaItemAffix Affix;
		Affix.AffixId = FName(*FString::Printf(TEXT("%s_%d"), *SelectedStat.ToString(), Index));
		Affix.StatAffected = SelectedStat;
		Affix.StatValueBonus = Bonus;

		if (SelectedStat == FName(TEXT("CritChance")))
		{
			Weapon.CritChanceBonus += Bonus;
			Affix.Description = FText::FromString(FString::Printf(TEXT("+%.0f%% critical strike chance"), Bonus * 100.0f));
		}
		else if (SelectedStat == FName(TEXT("SpellChantSpeed")))
		{
			Weapon.SpellChantSpeedBonus += Bonus;
			Affix.Description = FText::FromString(FString::Printf(TEXT("+%.0f%% spell chant output"), Bonus * 100.0f));
		}
		else if (SelectedStat == FName(TEXT("CourageGainScale")))
		{
			Weapon.CourageGainMultiplierBonus += Bonus;
			Affix.Description = FText::FromString(FString::Printf(TEXT("+%.0f%% courage gain"), Bonus * 100.0f));
		}
		else if (SelectedStat == FName(TEXT("PhysicalAttack")))
		{
			Weapon.PhysicalAttackBonus += Bonus * 100.0f;
			Affix.Description = FText::FromString(FString::Printf(TEXT("+%.1f physical attack"), Bonus * 100.0f));
		}
		else if (SelectedStat == FName(TEXT("MagicPower")))
		{
			Weapon.MagicPowerBonus += Bonus;
			Affix.Description = FText::FromString(FString::Printf(TEXT("+%.0f%% magic power"), Bonus * 100.0f));
		}

		Weapon.Affixes.Add(Affix);
	}
}
