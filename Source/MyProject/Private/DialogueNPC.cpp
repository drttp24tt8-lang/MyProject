#include "DialogueNPC.h"

#include "WistoriaCharacter.h"

void ADialogueNPC::Interact_Implementation(AActor* InteractingActor)
{
	if (AWistoriaCharacter* Player = Cast<AWistoriaCharacter>(InteractingActor))
	{
		if (!OfferedQuest.QuestId.IsNone())
		{
			Player->StartQuest(OfferedQuest);
		}
	}

	OnDialogueOpened(InteractingActor, DialogueLines);
}

FText ADialogueNPC::GetInteractionText_Implementation() const
{
	return FText::Format(FText::FromString(TEXT("Talk to {0}")), NPCName);
}
