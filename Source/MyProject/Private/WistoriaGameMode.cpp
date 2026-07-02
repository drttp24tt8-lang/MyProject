#include "WistoriaGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "WistoriaCharacter.h"
#include "WistoriaPlayerController.h"

AWistoriaGameMode::AWistoriaGameMode()
{
	DefaultPawnClass = AWistoriaCharacter::StaticClass();
	PlayerControllerClass = AWistoriaPlayerController::StaticClass();
}

void AWistoriaGameMode::SetActiveCompanionCount(int32 NewCompanionCount)
{
	ActiveCompanionCount = FMath::Clamp(NewCompanionCount, 0, 3);
}

void AWistoriaGameMode::AddActiveCompanion()
{
	SetActiveCompanionCount(ActiveCompanionCount + 1);
}

void AWistoriaGameMode::RemoveActiveCompanion()
{
	SetActiveCompanionCount(ActiveCompanionCount - 1);
}

int32 AWistoriaGameMode::GetActiveCompanionCount() const
{
	if (bAutoSyncCompanionCountFromPlayer)
	{
		if (const AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			return FMath::Clamp(PlayerCharacter->GetActiveCompanions().Num(), 0, 3);
		}
	}

	return ActiveCompanionCount;
}

int32 AWistoriaGameMode::RefreshActiveCompanionCountFromPlayer()
{
	if (const AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		SetActiveCompanionCount(PlayerCharacter->GetActiveCompanions().Num());
	}

	return ActiveCompanionCount;
}

float AWistoriaGameMode::GetEnemyHealthMultiplier() const
{
	return 1.0f + static_cast<float>(GetActiveCompanionCount()) * HealthScalePerCompanion;
}

float AWistoriaGameMode::GetEnemyDamageMultiplier() const
{
	return 1.0f + static_cast<float>(GetActiveCompanionCount()) * DamageScalePerCompanion;
}

void AWistoriaGameMode::AwardXPToPlayer(int32 XPAmount)
{
	if (XPAmount <= 0)
	{
		return;
	}

	AWistoriaCharacter* PlayerCharacter = Cast<AWistoriaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter)
	{
		PlayerCharacter->AddXP(XPAmount);
	}
}
