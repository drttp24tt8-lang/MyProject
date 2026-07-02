#include "SavePoint.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "WistoriaCharacter.h"

ASavePoint::ASavePoint()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	RootComponent = InteractionVolume;
	InteractionVolume->SetBoxExtent(FVector(120.0f, 120.0f, 120.0f));
	InteractionVolume->SetCollisionProfileName(TEXT("Trigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InteractionText = NSLOCTEXT("WistoriaSavePoint", "SavePointInteraction", "Rest and Save");
}

void ASavePoint::Interact_Implementation(AActor* InteractingActor)
{
	ActivateSavePoint(InteractingActor);
}

FText ASavePoint::GetInteractionText_Implementation() const
{
	return InteractionText;
}

bool ASavePoint::ActivateSavePoint(AActor* ActivatingActor)
{
	AWistoriaCharacter* Character = Cast<AWistoriaCharacter>(ActivatingActor);
	if (!Character)
	{
		return false;
	}

	Character->RestoreAllResources();
	const FVector RespawnLocation = GetActorLocation() + GetActorRotation().RotateVector(RespawnOffset);
	Character->ActivateCheckpoint(SavePointId, RespawnLocation, GetActorRotation(), SaveSlotName);

	const FVector EffectLocation = GetActorLocation();
	if (ActivateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ActivateSound, EffectLocation);
	}
	if (ActivateEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ActivateEffect, EffectLocation, GetActorRotation());
	}

	OnSavePointActivated(Character);
	return true;
}
