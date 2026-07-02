#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaInteractable.h"
#include "WistoriaTypes.h"
#include "FastTravelGate.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API AFastTravelGate : public AActor, public IWistoriaInteractable
{
	GENERATED_BODY()

public:
	AFastTravelGate();

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual FText GetInteractionText_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fast Travel")
	FWistoriaTeleportPoint GateConfig;

	UFUNCTION(BlueprintImplementableEvent, Category = "Fast Travel")
	void OnGateActivated(AActor* ActivatingActor, const FWistoriaTeleportPoint& ActivatedGate);
};
