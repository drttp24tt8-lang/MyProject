#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaInteractable.h"
#include "SavePoint.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class USoundBase;
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API ASavePoint : public AActor, public IWistoriaInteractable
{
	GENERATED_BODY()

public:
	ASavePoint();

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual FText GetInteractionText_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Save Point")
	bool ActivateSavePoint(AActor* ActivatingActor);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Point")
	FName SavePointId = TEXT("AcademyStart");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Point")
	FText InteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Point")
	FString SaveSlotName = TEXT("WistoriaSlot0");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Point")
	FVector RespawnOffset = FVector(120.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> ActivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ActivateEffect;

	UFUNCTION(BlueprintImplementableEvent, Category = "Save Point")
	void OnSavePointActivated(AActor* ActivatingActor);
};
