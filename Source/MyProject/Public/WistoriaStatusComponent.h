#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WistoriaTypes.h"
#include "WistoriaStatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatusChangedSignature, EWistoriaElement, Status, bool, bActive);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UWistoriaStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWistoriaStatusComponent();

	UFUNCTION(BlueprintCallable, Category = "Status")
	void ApplyStatus(EWistoriaElement Status, float DurationSeconds);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void ClearStatus(EWistoriaElement Status);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void ClearAllStatuses();

	UFUNCTION(BlueprintPure, Category = "Status")
	bool HasStatus(EWistoriaElement Status) const;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FStatusChangedSignature OnStatusChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Status")
	TSet<EWistoriaElement> ActiveStatuses;

private:
	FTimerHandle MiasmaTimerHandle;
	FTimerHandle BurnTickTimerHandle;
	FTimerHandle BurnDurationTimerHandle;
	FTimerHandle FreezeTimerHandle;
	float CachedWalkSpeed = 0.0f;

	void ClearMiasma();
	void ClearBurn();
	void ApplyBurnTick();
	void ClearFreeze();
};
