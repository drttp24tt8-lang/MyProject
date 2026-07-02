#include "MiasmaHazardVolume.h"

#include "Components/BoxComponent.h"
#include "WistoriaStatusComponent.h"

AMiasmaHazardVolume::AMiasmaHazardVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	HazardVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("HazardVolume"));
	SetRootComponent(HazardVolume);
	HazardVolume->SetBoxExtent(FVector(500.0f, 500.0f, 180.0f));
	HazardVolume->SetCollisionProfileName(TEXT("Trigger"));
	HazardVolume->OnComponentBeginOverlap.AddDynamic(this, &AMiasmaHazardVolume::OnHazardBeginOverlap);
	HazardVolume->OnComponentEndOverlap.AddDynamic(this, &AMiasmaHazardVolume::OnHazardEndOverlap);
}

void AMiasmaHazardVolume::SetMiasmaEnabled(bool bNewEnabled)
{
	bMiasmaEnabled = bNewEnabled;
	if (!HazardVolume)
	{
		return;
	}

	if (!bMiasmaEnabled)
	{
		TArray<AActor*> OverlappingActors;
		HazardVolume->GetOverlappingActors(OverlappingActors);
		for (AActor* OverlappingActor : OverlappingActors)
		{
			ClearMiasmaFrom(OverlappingActor);
		}
	}

	HazardVolume->SetCollisionEnabled(bMiasmaEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AMiasmaHazardVolume::OnHazardBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ApplyMiasmaTo(OtherActor);
}

void AMiasmaHazardVolume::OnHazardEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bClearMiasmaOnExit)
	{
		ClearMiasmaFrom(OtherActor);
	}
}

void AMiasmaHazardVolume::ApplyMiasmaTo(AActor* TargetActor)
{
	if (!bMiasmaEnabled || !TargetActor)
	{
		return;
	}

	if (UWistoriaStatusComponent* StatusComponent = TargetActor->FindComponentByClass<UWistoriaStatusComponent>())
	{
		StatusComponent->ApplyStatus(EWistoriaElement::Miasma, TimedMiasmaDuration);
		OnMiasmaApplied(TargetActor);
	}
}

void AMiasmaHazardVolume::ClearMiasmaFrom(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}

	if (UWistoriaStatusComponent* StatusComponent = TargetActor->FindComponentByClass<UWistoriaStatusComponent>())
	{
		StatusComponent->ClearStatus(EWistoriaElement::Miasma);
		OnMiasmaCleared(TargetActor);
	}
}
