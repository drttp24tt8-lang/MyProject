#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WistoriaTypes.h"
#include "WistoriaSkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkillChangedSignature, FName, SkillId);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UWistoriaSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWistoriaSkillComponent();

	UFUNCTION(BlueprintCallable, Category = "Skills")
	void AddSkillPoints(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Skills")
	bool PurchaseSkill(FName SkillId);

	UFUNCTION(BlueprintPure, Category = "Skills")
	bool IsSkillUnlocked(FName SkillId) const;

	UFUNCTION(BlueprintPure, Category = "Skills")
	int32 GetAvailableSkillPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	void SetAvailableSkillPoints(int32 NewSkillPoints);

	UFUNCTION(BlueprintPure, Category = "Skills")
	TMap<FName, FWistoriaSkillNode> GetSkillTree() const;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	void SetSkillTree(const TMap<FName, FWistoriaSkillNode>& NewSkillTree);

	UPROPERTY(BlueprintAssignable, Category = "Skills")
	FSkillChangedSignature OnSkillChanged;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills", meta = (AllowPrivateAccess = "true"))
	int32 AvailableSkillPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FWistoriaSkillNode> SkillTree;
};
