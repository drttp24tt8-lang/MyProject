#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaInteractable.h"
#include "WistoriaPuzzleSwitch.generated.h"

class AWistoriaDungeonPuzzleGate;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API AWistoriaPuzzleSwitch : public AActor, public IWistoriaInteractable
{
	GENERATED_BODY()

public:
	AWistoriaPuzzleSwitch();

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual FText GetInteractionText_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Dungeon|Puzzle")
	bool ActivateSwitch(AActor* ActivatingActor);

	UFUNCTION(BlueprintCallable, Category = "Dungeon|Puzzle")
	void ReleaseSwitch();

	UFUNCTION(BlueprintPure, Category = "Dungeon|Puzzle")
	bool IsActive() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SwitchMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle")
	FName SwitchId = TEXT("PuzzleSwitch");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle")
	TObjectPtr<AWistoriaDungeonPuzzleGate> LinkedGate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle")
	bool bOneShot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle", meta = (ClampMin = "0.0"))
	float ActiveDuration = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle")
	FText InactiveInteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle")
	FText ActiveInteractionText;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeon|Puzzle")
	bool bActive = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|Puzzle")
	void OnSwitchActivated(AActor* ActivatingActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|Puzzle")
	void OnSwitchReleased();

private:
	FTimerHandle ReleaseTimerHandle;
};
