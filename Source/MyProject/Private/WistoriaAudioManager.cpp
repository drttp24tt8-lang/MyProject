#include "WistoriaAudioManager.h"

#include "Components/AudioComponent.h"

AWistoriaAudioManager::AWistoriaAudioManager()
{
	PrimaryActorTick.bCanEverTick = false;

	MusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
	SetRootComponent(MusicComponent);
	MusicComponent->bAutoActivate = true;
}

void AWistoriaAudioManager::SetCombatIntensity(float NewCombatIntensity)
{
	MusicComponent->SetFloatParameter(CombatIntensityParameterName, FMath::Clamp(NewCombatIntensity, 0.0f, 1.0f));
}

void AWistoriaAudioManager::PlayMusic(USoundBase* MusicAsset)
{
	if (!MusicAsset)
	{
		return;
	}

	MusicComponent->SetSound(MusicAsset);
	MusicComponent->Play();
}
