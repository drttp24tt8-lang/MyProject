#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossArenaTrigger.generated.h"

class UBoxComponent;
class AWistoriaBossEnemy;

UCLASS()
class MYPROJECT_API ABossArenaTrigger : public AActor
{
	GENERATED_BODY()

public:
	ABossArenaTrigger();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	TObjectPtr<AWistoriaBossEnemy> BossActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	bool bTriggerOnce = true;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	bool bHasTriggered = false;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
	void OnBossArenaEntered(AActor* PlayerActor, AWistoriaBossEnemy* Boss);
};
