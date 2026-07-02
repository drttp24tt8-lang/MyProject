#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CompanionAIController.generated.h"

class AWistoriaCharacter;
class AWistoriaEnemy;

UCLASS()
class MYPROJECT_API ACompanionAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACompanionAIController();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Targeting")
	TObjectPtr<AWistoriaCharacter> PlayerCharacterRef;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Targeting")
	TObjectPtr<AActor> CurrentCombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement", meta = (ClampMin = "100.0"))
	float FollowRadius = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement", meta = (ClampMin = "100.0"))
	float CombatRange = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement", meta = (ClampMin = "100.0"))
	float AttackAcceptanceRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting", meta = (ClampMin = "100.0"))
	float EnemySearchRadius = 1200.0f;

	UFUNCTION(BlueprintImplementableEvent, Category = "AI|Combat")
	void OnCompanionCombatTargetChanged(AActor* NewTarget);

private:
	void HandleBehaviorTreeFallback();
	AActor* FindPlayerLockedTarget() const;
	AActor* FindNearestEnemy() const;
	void SetCombatTarget(AActor* NewTarget);
};
