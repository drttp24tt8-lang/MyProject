#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WistoriaMountCharacter.generated.h"

class AWistoriaCharacter;

UCLASS()
class MYPROJECT_API AWistoriaMountCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWistoriaMountCharacter();

	UFUNCTION(BlueprintCallable, Category = "Mount")
	bool MountRider(AWistoriaCharacter* NewRider, FName SeatSocketName);

	UFUNCTION(BlueprintCallable, Category = "Mount")
	bool DismountRider();

	UFUNCTION(BlueprintPure, Category = "Mount")
	AWistoriaCharacter* GetRider() const;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount", meta = (ClampMin = "300.0"))
	float MountedWalkSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	FVector DismountOffset = FVector(0.0f, 120.0f, 0.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Mount")
	TObjectPtr<AWistoriaCharacter> Rider;

	UPROPERTY(BlueprintReadOnly, Category = "Mount")
	TObjectPtr<AController> RiderController;

	UFUNCTION(BlueprintImplementableEvent, Category = "Mount")
	void OnRiderMounted(AWistoriaCharacter* MountedRider);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mount")
	void OnRiderDismounted(AWistoriaCharacter* DismountedRider);

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void HandleDismountInput();
};
