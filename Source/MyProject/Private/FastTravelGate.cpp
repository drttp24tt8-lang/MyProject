#include "FastTravelGate.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "WistoriaCharacter.h"

AFastTravelGate::AFastTravelGate()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetBoxExtent(FVector(120.0f, 120.0f, 160.0f));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AFastTravelGate::Interact_Implementation(AActor* InteractingActor)
{
	GateConfig.bUnlocked = true;
	if (GateConfig.WorldLocation.IsNearlyZero())
	{
		GateConfig.WorldLocation = GetActorLocation();
	}

	OnGateActivated(InteractingActor, GateConfig);
}

FText AFastTravelGate::GetInteractionText_Implementation() const
{
	return GateConfig.bUnlocked
		? FText::FromString(TEXT("Open Fast Travel"))
		: FText::FromString(TEXT("Activate Travel Gate"));
}
