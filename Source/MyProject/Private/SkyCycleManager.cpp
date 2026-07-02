#include "SkyCycleManager.h"

#include "Components/ExponentialHeightFogComponent.h"
#include "NiagaraComponent.h"

ASkyCycleManager::ASkyCycleManager()
{
	PrimaryActorTick.bCanEverTick = true;

	FogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("FogComponent"));
	SetRootComponent(FogComponent);
	FogComponent->SetFogDensity(ClearFogDensity);

	RainComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RainComponent"));
	RainComponent->SetupAttachment(RootComponent);
	RainComponent->Deactivate();
}

void ASkyCycleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DirectionalLightActor)
	{
		DirectionalLightActor->AddActorLocalRotation(FRotator(TimeScale * DeltaTime, 0.0f, 0.0f));
	}
}

void ASkyCycleManager::TriggerStorm(float NewFogDensity)
{
	FogComponent->SetFogDensity(NewFogDensity);
	RainComponent->Activate(true);
}

void ASkyCycleManager::ClearStorm()
{
	FogComponent->SetFogDensity(ClearFogDensity);
	RainComponent->Deactivate();
}
