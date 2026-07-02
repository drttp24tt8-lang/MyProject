#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaDungeonPuzzleGate.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API AWistoriaDungeonPuzzleGate : public AActor
{
	GENERATED_BODY()

public:
	AWistoriaDungeonPuzzleGate();

	UFUNCTION(BlueprintCallable, Category = "Dungeon|Puzzle")
	void RegisterSwitchState(FName SwitchId, bool bActive, AActor* ActivatingActor);

	UFUNCTION(BlueprintCallable, Category = "Dungeon|Puzzle")
	void ResetPuzzle();

	UFUNCTION(BlueprintPure, Category = "Dungeon|Puzzle")
	bool IsOpened() const;

	UFUNCTION(BlueprintPure, Category = "Dungeon|Puzzle")
	int32 GetActiveSwitchCount() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> GateBounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> GateMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle", meta = (ClampMin = "1"))
	int32 RequiredActiveSwitches = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle")
	bool bStayOpenOnceSolved = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon|Puzzle")
	FVector OpenLocationOffset = FVector(0.0f, 0.0f, 350.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Dungeon|Puzzle")
	bool bOpened = false;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeon|Puzzle")
	TArray<FName> ActiveSwitchIds;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|Puzzle")
	void OnPuzzleSolved();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeon|Puzzle")
	void OnPuzzleReset();

private:
	FVector ClosedLocation = FVector::ZeroVector;

	void EvaluatePuzzle();
	void OpenGate();
	void CloseGate();
};
