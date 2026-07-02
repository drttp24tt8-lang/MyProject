#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaTypes.h"
#include "WistoriaSpellProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class MYPROJECT_API AWistoriaSpellProjectile : public AActor
{
	GENERATED_BODY()

public:
	AWistoriaSpellProjectile();

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void ConfigureProjectile(float InDamage, EWistoriaElement InElement, AActor* InInstigatorActor);

	UFUNCTION(BlueprintCallable, Category = "Spell")
	void ConfigureProjectileFromSpellRow(const FWistoriaSpellDataRow& SpellRow, float DamageMultiplier, AActor* InInstigatorActor);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> TrailEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell", meta = (ClampMin = "0.0"))
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	EWistoriaElement Element = EWistoriaElement::Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Status")
	EWistoriaElement StatusToApply = EWistoriaElement::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Status", meta = (ClampMin = "0.0"))
	float StatusDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Status", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StatusChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	TObjectPtr<USoundBase> ImpactSound;

	UFUNCTION()
	void HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY()
	TObjectPtr<AActor> InstigatorActor;

	EWistoriaElement ResolveDefaultStatusForElement() const;
	float ResolveDefaultStatusDuration(EWistoriaElement Status) const;
	void TryApplyStatusToEnemy(class AWistoriaEnemy* Enemy) const;
};
