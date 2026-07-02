#include "WistoriaDungeonPuzzleGate.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AWistoriaDungeonPuzzleGate::AWistoriaDungeonPuzzleGate()
{
	PrimaryActorTick.bCanEverTick = false;

	GateBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("GateBounds"));
	RootComponent = GateBounds;
	GateBounds->SetBoxExtent(FVector(120.0f, 40.0f, 180.0f));
	GateBounds->SetCollisionProfileName(TEXT("BlockAll"));

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->SetupAttachment(RootComponent);
}

void AWistoriaDungeonPuzzleGate::BeginPlay()
{
	Super::BeginPlay();
	ClosedLocation = GetActorLocation();
}

void AWistoriaDungeonPuzzleGate::RegisterSwitchState(FName SwitchId, bool bActive, AActor* ActivatingActor)
{
	if (SwitchId.IsNone() || (bStayOpenOnceSolved && bOpened))
	{
		return;
	}

	if (bActive)
	{
		ActiveSwitchIds.AddUnique(SwitchId);
	}
	else
	{
		ActiveSwitchIds.Remove(SwitchId);
	}

	EvaluatePuzzle();
}

void AWistoriaDungeonPuzzleGate::ResetPuzzle()
{
	ActiveSwitchIds.Reset();
	CloseGate();
	OnPuzzleReset();
}

bool AWistoriaDungeonPuzzleGate::IsOpened() const
{
	return bOpened;
}

int32 AWistoriaDungeonPuzzleGate::GetActiveSwitchCount() const
{
	return ActiveSwitchIds.Num();
}

void AWistoriaDungeonPuzzleGate::EvaluatePuzzle()
{
	if (ActiveSwitchIds.Num() >= RequiredActiveSwitches)
	{
		OpenGate();
	}
	else if (!bStayOpenOnceSolved)
	{
		CloseGate();
	}
}

void AWistoriaDungeonPuzzleGate::OpenGate()
{
	if (bOpened)
	{
		return;
	}

	bOpened = true;
	SetActorLocation(ClosedLocation + OpenLocationOffset);
	GateBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnPuzzleSolved();
}

void AWistoriaDungeonPuzzleGate::CloseGate()
{
	if (!bOpened)
	{
		return;
	}

	bOpened = false;
	SetActorLocation(ClosedLocation);
	GateBounds->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
