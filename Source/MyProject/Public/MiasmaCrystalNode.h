#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaInteractable.h"
#include "MiasmaCrystalNode.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class MYPROJECT_API AMiasmaCrystalNode : public AActor, public IWistoriaInteractable
{
	GENERATED_BODY()

public:
	AMiasmaCrystalNode();

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual FText GetInteractionText_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Miasma")
	void CleanseNode(AActor* Cleanser);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CleanseRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miasma")
	float MiasmaDuration = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miasma")
	TObjectPtr<UNiagaraSystem> CleanseEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miasma")
	TObjectPtr<USoundBase> CleanseSound;

	UFUNCTION(BlueprintImplementableEvent, Category = "Miasma")
	void OnNodeCleansed(AActor* Cleanser);
};
