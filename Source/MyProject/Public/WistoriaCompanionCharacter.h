#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WistoriaTypes.h"
#include "WistoriaCompanionCharacter.generated.h"

class AWistoriaEnemy;

UCLASS()
class MYPROJECT_API AWistoriaCompanionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWistoriaCompanionCharacter();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Companion")
	void SetFollowTarget(AActor* NewFollowTarget);

	UFUNCTION(BlueprintCallable, Category = "Companion")
	void IssueCommand(EWistoriaCompanionCommand NewCommand, AActor* NewCommandTarget);

	UFUNCTION(BlueprintCallable, Category = "Companion")
	void ClearCommand();

	UFUNCTION(BlueprintPure, Category = "Companion")
	EWistoriaCompanionCommand GetCurrentCommand() const;

	UFUNCTION(BlueprintPure, Category = "Companion")
	AActor* GetCommandTarget() const;

	UFUNCTION(BlueprintPure, Category = "Companion")
	AActor* GetFollowTarget() const;

	UFUNCTION(BlueprintPure, Category = "Companion")
	FName GetCompanionId() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	FName CompanionId = NAME_None;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion", meta = (ClampMin = "100.0"))
	float FollowDistance = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion", meta = (ClampMin = "100.0"))
	float CommandAcceptanceRadius = 180.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Companion")
	EWistoriaCompanionCommand CurrentCommand = EWistoriaCompanionCommand::Follow;

	UPROPERTY(BlueprintReadOnly, Category = "Companion")
	TObjectPtr<AActor> FollowTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Companion")
	TObjectPtr<AActor> CommandTarget;

	UFUNCTION(BlueprintImplementableEvent, Category = "Companion")
	void OnAttackCommandTick(AActor* TargetActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Companion")
	void OnCompanionCommandChanged(EWistoriaCompanionCommand NewCommand, AActor* NewTarget);

private:
	void ProcessFollowCommand();
	void ProcessAttackCommand();
	void ProcessInteractCommand();
	void MoveToActorIfNeeded(AActor* TargetActor, float AcceptanceRadius);
};
