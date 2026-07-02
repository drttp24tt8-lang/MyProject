#pragma once

#include "CoreMinimal.h"
#include "WistoriaEnemy.h"
#include "WistoriaBossEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBossHealthChangedSignature, float, CurrentHealth, float, MaxHealth);

UCLASS()
class MYPROJECT_API AWistoriaBossEnemy : public AWistoriaEnemy
{
	GENERATED_BODY()

public:
	AWistoriaBossEnemy();
	virtual void ReceiveDamage(float DamageAmount, EWistoriaElement DamageElement) override;

	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FBossHealthChangedSignature OnBossHealthChanged;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
	void OnBossFightStarted();
};
