#include "WistoriaEnemyAIController.h"

#include "Kismet/GameplayStatics.h"
#include "WistoriaCharacter.h"
#include "WistoriaEnemy.h"

AWistoriaEnemyAIController::AWistoriaEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWistoriaEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledEnemy = Cast<AWistoriaEnemy>(InPawn);
}

void AWistoriaEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTarget();
	ChaseOrAttack(DeltaTime);
}

AActor* AWistoriaEnemyAIController::FindTarget() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn || !ControlledEnemy)
	{
		return nullptr;
	}

	const float DistanceSq = FVector::DistSquared(PlayerPawn->GetActorLocation(), ControlledEnemy->GetActorLocation());
	return DistanceSq <= FMath::Square(AggroRadius) ? PlayerPawn : nullptr;
}

void AWistoriaEnemyAIController::UpdateTarget()
{
	AActor* NewTarget = FindTarget();
	if (NewTarget == CurrentTarget)
	{
		return;
	}

	CurrentTarget = NewTarget;
	if (CurrentTarget)
	{
		OnTargetAcquired(CurrentTarget);
	}
	else
	{
		StopMovement();
		OnTargetLost();
	}
}

void AWistoriaEnemyAIController::ChaseOrAttack(float DeltaTime)
{
	if (!ControlledEnemy || !CurrentTarget || ControlledEnemy->IsDefeated())
	{
		return;
	}

	TimeUntilNextAttack = FMath::Max(0.0f, TimeUntilNextAttack - DeltaTime);

	const float DistanceSq = FVector::DistSquared(ControlledEnemy->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (DistanceSq > FMath::Square(AcceptanceRadius))
	{
		MoveToActor(CurrentTarget, AcceptanceRadius);
		return;
	}

	StopMovement();
	if (TimeUntilNextAttack <= 0.0f)
	{
		ControlledEnemy->TryMeleeAttack(CurrentTarget);
		TimeUntilNextAttack = AttackCooldown;
	}
}
