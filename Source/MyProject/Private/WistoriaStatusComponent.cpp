#include "WistoriaStatusComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "WistoriaEnemy.h"

UWistoriaStatusComponent::UWistoriaStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWistoriaStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWistoriaStatusComponent::ApplyStatus(EWistoriaElement Status, float DurationSeconds)
{
	if (Status == EWistoriaElement::None)
	{
		return;
	}

	const bool bAlreadyActive = ActiveStatuses.Contains(Status);
	ActiveStatuses.Add(Status);

	if (!bAlreadyActive)
	{
		OnStatusChanged.Broadcast(Status, true);
	}

	if (Status == EWistoriaElement::Miasma && DurationSeconds > 0.0f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			MiasmaTimerHandle,
			this,
			&UWistoriaStatusComponent::ClearMiasma,
			DurationSeconds,
			false);
	}
	else if (Status == EWistoriaElement::Burn && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			BurnTickTimerHandle,
			this,
			&UWistoriaStatusComponent::ApplyBurnTick,
			1.0f,
			true);

		GetWorld()->GetTimerManager().SetTimer(
			BurnDurationTimerHandle,
			this,
			&UWistoriaStatusComponent::ClearBurn,
			DurationSeconds,
			false);
	}
	else if (Status == EWistoriaElement::Freeze && GetWorld())
	{
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
			if (Movement && CachedWalkSpeed <= 0.0f)
			{
				CachedWalkSpeed = Movement->MaxWalkSpeed;
				Movement->MaxWalkSpeed *= 0.4f;
			}
		}

		GetWorld()->GetTimerManager().SetTimer(
			FreezeTimerHandle,
			this,
			&UWistoriaStatusComponent::ClearFreeze,
			DurationSeconds,
			false);
	}
}

void UWistoriaStatusComponent::ClearStatus(EWistoriaElement Status)
{
	if (ActiveStatuses.Remove(Status) > 0)
	{
		OnStatusChanged.Broadcast(Status, false);
	}

	if (Status == EWistoriaElement::Miasma && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MiasmaTimerHandle);
	}
	else if (Status == EWistoriaElement::Burn && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BurnTickTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(BurnDurationTimerHandle);
	}
	else if (Status == EWistoriaElement::Freeze && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FreezeTimerHandle);
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
			if (Movement && CachedWalkSpeed > 0.0f)
			{
				Movement->MaxWalkSpeed = CachedWalkSpeed;
				CachedWalkSpeed = 0.0f;
			}
		}
	}
}

void UWistoriaStatusComponent::ClearAllStatuses()
{
	TArray<EWistoriaElement> Statuses = ActiveStatuses.Array();
	for (EWistoriaElement Status : Statuses)
	{
		ClearStatus(Status);
	}
}

bool UWistoriaStatusComponent::HasStatus(EWistoriaElement Status) const
{
	return ActiveStatuses.Contains(Status);
}

void UWistoriaStatusComponent::ClearMiasma()
{
	ClearStatus(EWistoriaElement::Miasma);
}

void UWistoriaStatusComponent::ClearBurn()
{
	ClearStatus(EWistoriaElement::Burn);
}

void UWistoriaStatusComponent::ApplyBurnTick()
{
	if (AWistoriaEnemy* Enemy = Cast<AWistoriaEnemy>(GetOwner()))
	{
		Enemy->ReceiveDamage(10.0f, EWistoriaElement::Burn);
	}
}

void UWistoriaStatusComponent::ClearFreeze()
{
	ClearStatus(EWistoriaElement::Freeze);
}
