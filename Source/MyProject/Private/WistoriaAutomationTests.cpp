#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "WistoriaInventoryComponent.h"
#include "WistoriaQuestComponent.h"
#include "WistoriaSaveGame.h"
#include "WistoriaSaveLibrary.h"
#include "WistoriaSkillComponent.h"

#include "Kismet/GameplayStatics.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWistoriaSaveSlotAutomationTest, "WandAndWistoria.Core.SaveSlots", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWistoriaSaveSlotAutomationTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Slot 1 name"), UWistoriaSaveLibrary::GetIndexedSaveSlot(1), FString(TEXT("WistoriaSlot_1")));
	TestEqual(TEXT("Slot 2 name"), UWistoriaSaveLibrary::GetIndexedSaveSlot(2), FString(TEXT("WistoriaSlot_2")));
	TestEqual(TEXT("Slot 3 name"), UWistoriaSaveLibrary::GetIndexedSaveSlot(3), FString(TEXT("WistoriaSlot_3")));
	TestEqual(TEXT("Low indexes clamp to slot 1"), UWistoriaSaveLibrary::GetIndexedSaveSlot(-5), FString(TEXT("WistoriaSlot_1")));
	TestEqual(TEXT("High indexes clamp to slot 3"), UWistoriaSaveLibrary::GetIndexedSaveSlot(99), FString(TEXT("WistoriaSlot_3")));

	const FString TempSlot = TEXT("WistoriaAutomationTempSlot");
	UWistoriaSaveLibrary::DeleteSave(TempSlot);
	TestTrue(TEXT("New save can be created"), UWistoriaSaveLibrary::CreateNewSave(TempSlot));
	UWistoriaSaveGame* CreatedSave = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(TempSlot, 0));
	TestNotNull(TEXT("Created save can be loaded"), CreatedSave);
	if (CreatedSave)
	{
		TestTrue(TEXT("Created save is marked started"), CreatedSave->bSaveStarted);
	}
	TestTrue(TEXT("Temporary save can be deleted"), UWistoriaSaveLibrary::DeleteSave(TempSlot));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWistoriaInventoryAutomationTest, "WandAndWistoria.Core.Inventory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWistoriaInventoryAutomationTest::RunTest(const FString& Parameters)
{
	UWistoriaInventoryComponent* Inventory = NewObject<UWistoriaInventoryComponent>();
	TestNotNull(TEXT("Inventory component created"), Inventory);

	FWistoriaInventoryItem Material;
	Material.ItemId = TEXT("Dungeon_Core_Shard");
	Material.DisplayName = FText::FromString(TEXT("Dungeon Core Shard"));
	Material.ItemType = EWistoriaItemType::Material;
	Material.Quantity = 2;
	Inventory->AddItem(Material);
	Inventory->AddItem(Material);
	TestTrue(TEXT("Materials stack to quantity 4"), Inventory->HasItem(Material.ItemId, 4));

	FWistoriaInventoryItem Sword;
	Sword.ItemId = TEXT("Training_Sword");
	Sword.DisplayName = FText::FromString(TEXT("Training Sword"));
	Sword.ItemType = EWistoriaItemType::Weapon;
	Sword.EquipmentSlot = EWistoriaEquipmentSlot::MainHand;
	Sword.Quantity = 1;
	Sword.PhysicalAttackBonus = 12.0f;
	Inventory->AddItem(Sword);
	TestTrue(TEXT("Weapon equips from inventory"), Inventory->EquipItem(Sword.ItemId));
	TestEqual(TEXT("Equipped physical bonus applied"), Inventory->GetTotalPhysicalBonus(), 12.0f);
	TestFalse(TEXT("Equipped item leaves inventory"), Inventory->HasItem(Sword.ItemId, 1));

	Inventory->AddGold(100);
	TestTrue(TEXT("Gold can be spent"), Inventory->SpendGold(35));
	TestEqual(TEXT("Gold balance after spend"), Inventory->GetGold(), 65);
	TestFalse(TEXT("Cannot overspend gold"), Inventory->SpendGold(1000));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWistoriaSkillAutomationTest, "WandAndWistoria.Core.Skills", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWistoriaSkillAutomationTest::RunTest(const FString& Parameters)
{
	UWistoriaSkillComponent* Skills = NewObject<UWistoriaSkillComponent>();
	TestNotNull(TEXT("Skill component created"), Skills);

	FWistoriaSkillNode RootSkill;
	RootSkill.SkillId = TEXT("LightningSpark");
	RootSkill.SkillName = FText::FromString(TEXT("Lightning Spark"));
	RootSkill.SkillPointCost = 1;

	FWistoriaSkillNode AdvancedSkill;
	AdvancedSkill.SkillId = TEXT("ArsMagna");
	AdvancedSkill.SkillName = FText::FromString(TEXT("Ars Magna"));
	AdvancedSkill.SkillPointCost = 1;
	AdvancedSkill.PrerequisiteSkillId = RootSkill.SkillId;

	TMap<FName, FWistoriaSkillNode> SkillTree;
	SkillTree.Add(RootSkill.SkillId, RootSkill);
	SkillTree.Add(AdvancedSkill.SkillId, AdvancedSkill);
	Skills->SetSkillTree(SkillTree);
	Skills->AddSkillPoints(2);

	TestFalse(TEXT("Cannot buy child before prerequisite"), Skills->PurchaseSkill(AdvancedSkill.SkillId));
	TestTrue(TEXT("Can buy root skill"), Skills->PurchaseSkill(RootSkill.SkillId));
	TestTrue(TEXT("Can buy child after prerequisite"), Skills->PurchaseSkill(AdvancedSkill.SkillId));
	TestEqual(TEXT("Skill points spent"), Skills->GetAvailableSkillPoints(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWistoriaQuestAutomationTest, "WandAndWistoria.Core.Quests", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWistoriaQuestAutomationTest::RunTest(const FString& Parameters)
{
	UWistoriaQuestComponent* Quests = NewObject<UWistoriaQuestComponent>();
	TestNotNull(TEXT("Quest component created"), Quests);

	FWistoriaQuestRecord Quest;
	Quest.QuestId = TEXT("Quest_FirstDescent");
	Quest.QuestName = FText::FromString(TEXT("The First Descent"));
	Quest.RewardXP = 250;

	FWistoriaQuestObjective Objective;
	Objective.ObjectiveId = TEXT("DungeonBeast");
	Objective.Description = FText::FromString(TEXT("Defeat dungeon beasts"));
	Objective.RequiredCount = 2;
	Quest.Objectives.Add(Objective);

	Quests->StartQuest(Quest);
	TestTrue(TEXT("First kill advances objective"), Quests->AdvanceAnyActiveObjective(Objective.ObjectiveId, 1));
	TArray<FWistoriaQuestRecord> ActiveQuests = Quests->GetQuests();
	TestEqual(TEXT("Quest remains active after one kill"), ActiveQuests[0].State, EWistoriaQuestState::Active);

	TArray<FWistoriaQuestRecord> CompletedQuests;
	TestTrue(TEXT("Second kill advances objective"), Quests->AdvanceAnyActiveObjectiveWithCompletions(Objective.ObjectiveId, 1, CompletedQuests));
	ActiveQuests = Quests->GetQuests();
	TestEqual(TEXT("Quest completes after required kills"), ActiveQuests[0].State, EWistoriaQuestState::Completed);
	TestEqual(TEXT("Completion returned once"), CompletedQuests.Num(), 1);
	return true;
}

#endif
