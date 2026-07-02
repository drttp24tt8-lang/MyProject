#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WistoriaTypes.h"
#include "WistoriaQuestComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestChangedSignature, FName, QuestId);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UWistoriaQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWistoriaQuestComponent();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartQuest(const FWistoriaQuestRecord& Quest);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AdvanceObjective(FName QuestId, FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AdvanceAnyActiveObjective(FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AdvanceAnyActiveObjectiveWithCompletions(FName ObjectiveId, int32 Amount, TArray<FWistoriaQuestRecord>& CompletedQuests);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool CompleteQuest(FName QuestId);

	UFUNCTION(BlueprintPure, Category = "Quest")
	TArray<FWistoriaQuestRecord> GetQuests() const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SetQuests(const TArray<FWistoriaQuestRecord>& NewQuests);

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FQuestChangedSignature OnQuestChanged;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = "true"))
	TArray<FWistoriaQuestRecord> Quests;

	int32 FindQuestIndex(FName QuestId) const;
	void RefreshQuestCompletion(int32 QuestIndex);
};
