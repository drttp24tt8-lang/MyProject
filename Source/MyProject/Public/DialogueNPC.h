#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WistoriaInteractable.h"
#include "WistoriaTypes.h"
#include "DialogueNPC.generated.h"

UCLASS()
class MYPROJECT_API ADialogueNPC : public ACharacter, public IWistoriaInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual FText GetInteractionText_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText NPCName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FText> DialogueLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FWistoriaQuestRecord OfferedQuest;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnDialogueOpened(AActor* InteractingActor, const TArray<FText>& Lines);
};
