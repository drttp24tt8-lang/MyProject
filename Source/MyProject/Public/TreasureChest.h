#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaInteractable.h"
#include "WistoriaTypes.h"
#include "TreasureChest.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class USoundBase;
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API ATreasureChest : public AActor, public IWistoriaInteractable
{
	GENERATED_BODY()

public:
	ATreasureChest();

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual FText GetInteractionText_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Treasure")
	bool OpenChest(AActor* Opener);

	UFUNCTION(BlueprintPure, Category = "Treasure")
	bool IsOpened() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure")
	TArray<FWistoriaInventoryItem> RewardItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure", meta = (ClampMin = "0"))
	int32 GoldReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure")
	FText ClosedInteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure")
	FText OpenedInteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure|Persistence")
	FName ChestId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure|Persistence")
	bool bPersistOpenedState = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure|Persistence")
	FString SaveSlotName = TEXT("WistoriaSlot0");

	UPROPERTY(BlueprintReadOnly, Category = "Treasure")
	bool bOpened = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> OpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> OpenEffect;

	UFUNCTION(BlueprintImplementableEvent, Category = "Treasure")
	void OnChestOpened(AActor* Opener);

private:
	void LoadOpenedState();
	void SaveOpenedState() const;
};
