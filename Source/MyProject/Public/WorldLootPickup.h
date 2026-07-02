#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaInteractable.h"
#include "WistoriaTypes.h"
#include "WorldLootPickup.generated.h"

class UNiagaraSystem;
class USoundBase;
class UPrimitiveComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API AWorldLootPickup : public AActor, public IWistoriaInteractable
{
	GENERATED_BODY()

public:
	AWorldLootPickup();

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual FText GetInteractionText_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void ConfigureLoot(const FWistoriaInventoryItem& NewItem, int32 NewGoldAmount = 0);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	bool Collect(AActor* Collector);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FWistoriaInventoryItem Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "0"))
	int32 GoldAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	bool bAutoCollectOnOverlap = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FText InteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> CollectSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> CollectEffect;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Loot")
	void OnCollected(AActor* Collector);
};
