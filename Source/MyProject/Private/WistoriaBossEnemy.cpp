#include "WistoriaBossEnemy.h"

AWistoriaBossEnemy::AWistoriaBossEnemy()
{
	MaxHealth = 2500.0f;
	CurrentHealth = MaxHealth;
}

void AWistoriaBossEnemy::BeginPlay()
{
	Super::BeginPlay();
	OnBossFightStarted();
	OnBossHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void AWistoriaBossEnemy::ReceiveDamage(float DamageAmount, EWistoriaElement DamageElement)
{
	Super::ReceiveDamage(DamageAmount, DamageElement);
	OnBossHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}
