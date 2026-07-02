#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "WistoriaTypes.h"
#include "WistoriaMeleeHitbox.generated.h"

class AWistoriaEnemy;

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UWistoriaMeleeHitbox : public UBoxComponent
{
	GENERATED_BODY()

public:
	UWistoriaMeleeHitbox(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateHitbox(float InDamage, EWistoriaElement InElement = EWistoriaElement::None);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DeactivateHitbox();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = "0.0"))
	float Damage = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EWistoriaElement Element = EWistoriaElement::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIgnoreOwner = true;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

private:
	TSet<TWeakObjectPtr<AActor>> DamagedActors;
};
