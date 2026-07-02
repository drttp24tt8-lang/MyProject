#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiasmaHazardVolume.generated.h"

class UBoxComponent;
class UPrimitiveComponent;

UCLASS()
class MYPROJECT_API AMiasmaHazardVolume : public AActor
{
	GENERATED_BODY()

public:
	AMiasmaHazardVolume();

	UFUNCTION(BlueprintCallable, Category = "Miasma")
	void SetMiasmaEnabled(bool bNewEnabled);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> HazardVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miasma")
	bool bMiasmaEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miasma")
	bool bClearMiasmaOnExit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miasma", meta = (ClampMin = "0.0"))
	float TimedMiasmaDuration = 0.0f;

	UFUNCTION()
	void OnHazardBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHazardEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Miasma")
	void OnMiasmaApplied(AActor* AffectedActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Miasma")
	void OnMiasmaCleared(AActor* AffectedActor);

private:
	void ApplyMiasmaTo(AActor* TargetActor);
	void ClearMiasmaFrom(AActor* TargetActor);
};
