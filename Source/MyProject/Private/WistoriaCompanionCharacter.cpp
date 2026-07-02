#include "WistoriaCompanionCharacter.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "CompanionAIController.h"
#include "WistoriaCharacter.h"
#include "WistoriaGameMode.h"
#include "WistoriaInteractable.h"

AWistoriaCompanionCharacter::AWistoriaCompanionCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ACompanionAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AWistoriaCompanionCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!FollowTarget)
	{
		FollowTarget = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	if (AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(FollowTarget))
	{
		PlayerCharacter->RegisterCompanion(this);
	}

	if (AWistoriaGameMode* WistoriaGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AWistoriaGameMode>() : nullptr)
	{
		WistoriaGameMode->AddActiveCompanion();
	}
}

void AWistoriaCompanionCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(FollowTarget))
	{
		PlayerCharacter->UnregisterCompanion(this);
	}

	if (AWistoriaGameMode* WistoriaGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AWistoriaGameMode>() : nullptr)
	{
		WistoriaGameMode->RemoveActiveCompanion();
	}

	Super::EndPlay(EndPlayReason);
}

void AWistoriaCompanionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentCommand)
	{
	case EWistoriaCompanionCommand::Follow:
	case EWistoriaCompanionCommand::DefendPlayer:
		ProcessFollowCommand();
		break;
	case EWistoriaCompanionCommand::AttackTarget:
		ProcessAttackCommand();
		break;
	case EWistoriaCompanionCommand::InteractWithTarget:
		ProcessInteractCommand();
		break;
	case EWistoriaCompanionCommand::HoldPosition:
	default:
		break;
	}
}

void AWistoriaCompanionCharacter::SetFollowTarget(AActor* NewFollowTarget)
{
	FollowTarget = NewFollowTarget;
}

void AWistoriaCompanionCharacter::IssueCommand(EWistoriaCompanionCommand NewCommand, AActor* NewCommandTarget)
{
	CurrentCommand = NewCommand;
	CommandTarget = NewCommandTarget;
	OnCompanionCommandChanged(CurrentCommand, CommandTarget);
}

void AWistoriaCompanionCharacter::ClearCommand()
{
	IssueCommand(EWistoriaCompanionCommand::Follow, FollowTarget);
}

EWistoriaCompanionCommand AWistoriaCompanionCharacter::GetCurrentCommand() const
{
	return CurrentCommand;
}

AActor* AWistoriaCompanionCharacter::GetCommandTarget() const
{
	return CommandTarget;
}

AActor* AWistoriaCompanionCharacter::GetFollowTarget() const
{
	return FollowTarget;
}

FName AWistoriaCompanionCharacter::GetCompanionId() const
{
	return CompanionId;
}

void AWistoriaCompanionCharacter::ProcessFollowCommand()
{
	MoveToActorIfNeeded(FollowTarget, FollowDistance);
}

void AWistoriaCompanionCharacter::ProcessAttackCommand()
{
	if (!CommandTarget)
	{
		ClearCommand();
		return;
	}

	MoveToActorIfNeeded(CommandTarget, CommandAcceptanceRadius);
	OnAttackCommandTick(CommandTarget);
}

void AWistoriaCompanionCharacter::ProcessInteractCommand()
{
	if (!CommandTarget)
	{
		ClearCommand();
		return;
	}

	MoveToActorIfNeeded(CommandTarget, CommandAcceptanceRadius);

	if (FVector::DistSquared(GetActorLocation(), CommandTarget->GetActorLocation()) <= FMath::Square(CommandAcceptanceRadius))
	{
		if (CommandTarget->GetClass()->ImplementsInterface(UWistoriaInteractable::StaticClass()))
		{
			IWistoriaInteractable::Execute_Interact(CommandTarget, this);
		}

		ClearCommand();
	}
}

void AWistoriaCompanionCharacter::MoveToActorIfNeeded(AActor* TargetActor, float AcceptanceRadius)
{
	if (!TargetActor)
	{
		return;
	}

	if (FVector::DistSquared(GetActorLocation(), TargetActor->GetActorLocation()) <= FMath::Square(AcceptanceRadius))
	{
		return;
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToActor(TargetActor, AcceptanceRadius);
	}
}
