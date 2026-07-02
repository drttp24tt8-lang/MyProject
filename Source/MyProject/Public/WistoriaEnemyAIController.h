#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WistoriaEnemyAIController.generated.h"

class AWistoriaEnemy;

UCLASS()
class MYPROJECT_API AWistoriaEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AWistoriaEnemyAIController();

	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ClampMin = "100.0"))
	float AggroRadius = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ClampMin = "0.1"))
	float AttackCooldown = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ClampMin = "10.0"))
	float AcceptanceRadius = 140.0f;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	TObjectPtr<AWistoriaEnemy> ControlledEnemy;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	TObjectPtr<AActor> CurrentTarget;

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnTargetAcquired(AActor* TargetActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnTargetLost();

private:
	float TimeUntilNextAttack = 0.0f;

	AActor* FindTarget() const;
	void UpdateTarget();
	void ChaseOrAttack(float DeltaTime);
};
