#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyCycleManager.generated.h"

class UExponentialHeightFogComponent;
class UNiagaraComponent;

UCLASS()
class MYPROJECT_API ASkyCycleManager : public AActor
{
	GENERATED_BODY()

public:
	ASkyCycleManager();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Sky")
	void TriggerStorm(float NewFogDensity = 0.1f);

	UFUNCTION(BlueprintCallable, Category = "Sky")
	void ClearStorm();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky")
	TObjectPtr<AActor> DirectionalLightActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UExponentialHeightFogComponent> FogComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> RainComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky")
	float TimeScale = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky")
	float ClearFogDensity = 0.02f;
};
