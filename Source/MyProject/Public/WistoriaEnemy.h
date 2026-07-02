#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WistoriaTypes.h"
#include "WistoriaEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDefeatedSignature, class AWistoriaEnemy*, Enemy);

UCLASS()
class MYPROJECT_API AWistoriaEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AWistoriaEnemy();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ApplyScaling(float HealthMultiplier, float DamageMultiplier);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	virtual void ReceiveDamage(float DamageAmount, EWistoriaElement DamageElement = EWistoriaElement::None);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	virtual void ReceiveDamageFrom(float DamageAmount, EWistoriaElement DamageElement, AActor* DamageInstigator);

	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsDefeated() const;

	UFUNCTION(BlueprintPure, Category = "Enemy")
	float GetCurrentHealthValue() const;

	UFUNCTION(BlueprintPure, Category = "Enemy")
	float GetMaxHealthValue() const;

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	bool TryMeleeAttack(AActor* TargetActor);

	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FEnemyDefeatedSignature OnEnemyDefeated;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWistoriaStatusComponent> StatusComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy", meta = (ClampMin = "1.0"))
	float MaxHealth = 50.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	float CurrentHealth = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy", meta = (ClampMin = "0.0"))
	float BaseDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Combat", meta = (ClampMin = "10.0"))
	float MeleeRange = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EWistoriaElement Element = EWistoriaElement::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	FName EnemyId = TEXT("DungeonBeast");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	int32 XPReward = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot")
	TArray<FWistoriaInventoryItem> LootDrops;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot")
	bool bRollProceduralWeaponDrop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot", meta = (EditCondition = "bRollProceduralWeaponDrop"))
	TArray<FWistoriaInventoryItem> ProceduralWeaponDropPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bRollProceduralWeaponDrop"))
	float ProceduralWeaponDropChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot|Rarity", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bRollProceduralWeaponDrop"))
	float LegendaryDropRate = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot|Rarity", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bRollProceduralWeaponDrop"))
	float EpicDropRate = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot|Rarity", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bRollProceduralWeaponDrop"))
	float RareDropRate = 0.30f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnDamaged(float DamageAmount, EWistoriaElement DamageElement);

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnDefeated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Combat")
	void OnAttackParried(AActor* ParryingActor);

private:
	UPROPERTY()
	TObjectPtr<AActor> LastDamageInstigator;

	void GrantDefeatRewards();
	FWistoriaInventoryItem GenerateProceduralWeaponDrop(const FWistoriaInventoryItem& BaseWeapon) const;
	EWistoriaItemRarity RollWeaponRarity() const;
	void ApplyRandomAffixes(FWistoriaInventoryItem& Weapon, int32 AffixCount, float RarityMultiplier) const;
};
