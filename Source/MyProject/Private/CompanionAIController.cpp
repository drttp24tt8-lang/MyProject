#include "CompanionAIController.h"

#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "WistoriaCompanionCharacter.h"
#include "WistoriaCharacter.h"
#include "WistoriaEnemy.h"

ACompanionAIController::ACompanionAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACompanionAIController::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacterRef = Cast<AWistoriaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void ACompanionAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleBehaviorTreeFallback();
}

void ACompanionAIController::HandleBehaviorTreeFallback()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !PlayerCharacterRef)
	{
		return;
	}

	AWistoriaCompanionCharacter* CompanionPawn = Cast<AWistoriaCompanionCharacter>(ControlledPawn);
	if (CompanionPawn)
	{
		if (CompanionPawn->GetCurrentCommand() == EWistoriaCompanionCommand::HoldPosition)
		{
			StopMovement();
			SetCombatTarget(nullptr);
			return;
		}

		if (CompanionPawn->GetCurrentCommand() == EWistoriaCompanionCommand::InteractWithTarget)
		{
			SetCombatTarget(nullptr);
			return;
		}

		if (CompanionPawn->GetCurrentCommand() == EWistoriaCompanionCommand::AttackTarget)
		{
			AActor* CommandTarget = CompanionPawn->GetCommandTarget();
			SetCombatTarget(IsValid(CommandTarget) ? CommandTarget : nullptr);
			if (CurrentCombatTarget)
			{
				const float DistanceToTargetSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), CurrentCombatTarget->GetActorLocation());
				if (DistanceToTargetSq <= FMath::Square(AttackAcceptanceRadius))
				{
					StopMovement();
					return;
				}

				MoveToActor(CurrentCombatTarget, AttackAcceptanceRadius);
			}
			return;
		}
	}

	AActor* DesiredTarget = FindPlayerLockedTarget();
	if (!DesiredTarget)
	{
		DesiredTarget = FindNearestEnemy();
	}

	SetCombatTarget(DesiredTarget);
	if (CurrentCombatTarget)
	{
		const float DistanceToTargetSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), CurrentCombatTarget->GetActorLocation());
		if (DistanceToTargetSq <= FMath::Square(AttackAcceptanceRadius))
		{
			StopMovement();
			return;
		}

		MoveToActor(CurrentCombatTarget, AttackAcceptanceRadius);
		return;
	}

	if (FVector::DistSquared(ControlledPawn->GetActorLocation(), PlayerCharacterRef->GetActorLocation()) > FMath::Square(FollowRadius))
	{
		MoveToActor(PlayerCharacterRef, FollowRadius * 0.5f);
	}
}

AActor* ACompanionAIController::FindPlayerLockedTarget() const
{
	if (!PlayerCharacterRef || !PlayerCharacterRef->IsTargeting())
	{
		return nullptr;
	}

	AActor* LockedTarget = PlayerCharacterRef->GetLockedTarget();
	if (!LockedTarget || !LockedTarget->IsA<AWistoriaEnemy>())
	{
		return nullptr;
	}

	if (APawn* ControlledPawn = GetPawn())
	{
		if (FVector::DistSquared(ControlledPawn->GetActorLocation(), LockedTarget->GetActorLocation()) <= FMath::Square(CombatRange))
		{
			return LockedTarget;
		}
	}

	return nullptr;
}

AActor* ACompanionAIController::FindNearestEnemy() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !GetWorld())
	{
		return nullptr;
	}

	TArray<FOverlapResult> Results;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(EnemySearchRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(WistoriaCompanionEnemySearch), false, ControlledPawn);
	GetWorld()->OverlapMultiByObjectType(
		Results,
		ControlledPawn->GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		Sphere,
		Params);

	AActor* BestTarget = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();
	for (const FOverlapResult& Result : Results)
	{
		AActor* Candidate = Result.GetActor();
		if (!Candidate || !Candidate->IsA<AWistoriaEnemy>())
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(ControlledPawn->GetActorLocation(), Candidate->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}

void ACompanionAIController::SetCombatTarget(AActor* NewTarget)
{
	if (CurrentCombatTarget == NewTarget)
	{
		return;
	}

	CurrentCombatTarget = NewTarget;
	OnCompanionCombatTargetChanged(CurrentCombatTarget);
}
