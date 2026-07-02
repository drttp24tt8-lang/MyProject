#include "MiasmaCrystalNode.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "WistoriaStatusComponent.h"

AMiasmaCrystalNode::AMiasmaCrystalNode()
{
	PrimaryActorTick.bCanEverTick = false;

	CleanseRadius = CreateDefaultSubobject<USphereComponent>(TEXT("CleanseRadius"));
	SetRootComponent(CleanseRadius);
	CleanseRadius->SetSphereRadius(180.0f);
	CleanseRadius->SetCollisionProfileName(TEXT("Trigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AMiasmaCrystalNode::Interact_Implementation(AActor* InteractingActor)
{
	CleanseNode(InteractingActor);
}

FText AMiasmaCrystalNode::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("Cleanse Miasma Crystal"));
}

void AMiasmaCrystalNode::CleanseNode(AActor* Cleanser)
{
	if (Cleanser)
	{
		if (UWistoriaStatusComponent* StatusComponent = Cleanser->FindComponentByClass<UWistoriaStatusComponent>())
		{
			StatusComponent->ClearStatus(EWistoriaElement::Miasma);
		}
	}

	if (CleanseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CleanseSound, GetActorLocation());
	}

	if (CleanseEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CleanseEffect, GetActorLocation(), GetActorRotation());
	}

	OnNodeCleansed(Cleanser);
	Destroy();
}
