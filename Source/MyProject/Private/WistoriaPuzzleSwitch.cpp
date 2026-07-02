#include "WistoriaPuzzleSwitch.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "WistoriaDungeonPuzzleGate.h"

AWistoriaPuzzleSwitch::AWistoriaPuzzleSwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	RootComponent = InteractionVolume;
	InteractionVolume->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	InteractionVolume->SetCollisionProfileName(TEXT("Trigger"));

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
	SwitchMesh->SetupAttachment(RootComponent);
	SwitchMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwitchMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SwitchMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InactiveInteractionText = NSLOCTEXT("WistoriaPuzzle", "ActivateSwitch", "Activate Rune Switch");
	ActiveInteractionText = NSLOCTEXT("WistoriaPuzzle", "ActiveSwitch", "Rune Switch Active");
}

void AWistoriaPuzzleSwitch::Interact_Implementation(AActor* InteractingActor)
{
	ActivateSwitch(InteractingActor);
}

FText AWistoriaPuzzleSwitch::GetInteractionText_Implementation() const
{
	return bActive ? ActiveInteractionText : InactiveInteractionText;
}

bool AWistoriaPuzzleSwitch::ActivateSwitch(AActor* ActivatingActor)
{
	if (bActive || !LinkedGate)
	{
		return false;
	}

	bActive = true;
	LinkedGate->RegisterSwitchState(SwitchId, true, ActivatingActor);

	if (!bOneShot && ActiveDuration > 0.0f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &AWistoriaPuzzleSwitch::ReleaseSwitch, ActiveDuration, false);
	}

	OnSwitchActivated(ActivatingActor);
	return true;
}

void AWistoriaPuzzleSwitch::ReleaseSwitch()
{
	if (!bActive || bOneShot)
	{
		return;
	}

	bActive = false;
	if (LinkedGate)
	{
		LinkedGate->RegisterSwitchState(SwitchId, false, nullptr);
	}
	OnSwitchReleased();
}

bool AWistoriaPuzzleSwitch::IsActive() const
{
	return bActive;
}
