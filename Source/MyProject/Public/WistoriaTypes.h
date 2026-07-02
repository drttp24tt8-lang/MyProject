#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WistoriaTypes.generated.h"

class UNiagaraSystem;
class USkeletalMesh;
class UStaticMesh;

UENUM(BlueprintType)
enum class EWistoriaCombatStyle : uint8
{
	MagiclessSword UMETA(DisplayName = "Will Serfort Style"),
	AcademyMage UMETA(DisplayName = "Academy Mage")
};

UENUM(BlueprintType)
enum class EWistoriaElement : uint8
{
	None UMETA(DisplayName = "None"),
	Fire UMETA(DisplayName = "Fire"),
	Ice UMETA(DisplayName = "Ice"),
	Lightning UMETA(DisplayName = "Lightning"),
	Miasma UMETA(DisplayName = "Corruption Miasma"),
	Burn UMETA(DisplayName = "Burn"),
	Freeze UMETA(DisplayName = "Freeze")
};

UENUM(BlueprintType)
enum class EWistoriaItemType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Consumable UMETA(DisplayName = "Consumable"),
	KeyItem UMETA(DisplayName = "Key Item"),
	Material UMETA(DisplayName = "Material")
};

UENUM(BlueprintType)
enum class EWistoriaItemRarity : uint8
{
	Standard UMETA(DisplayName = "Standard"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct FWistoriaItemAffix
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FName AffixId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FName StatAffected = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	float StatValueBonus = 0.0f;
};

UENUM(BlueprintType)
enum class EWistoriaEquipmentSlot : uint8
{
	MainHand UMETA(DisplayName = "Main Hand"),
	OffHand UMETA(DisplayName = "Off Hand"),
	Chest UMETA(DisplayName = "Chest"),
	Boots UMETA(DisplayName = "Boots"),
	Accessory UMETA(DisplayName = "Accessory")
};

UENUM(BlueprintType)
enum class EWistoriaQuestState : uint8
{
	Inactive UMETA(DisplayName = "Inactive"),
	Active UMETA(DisplayName = "Active"),
	Completed UMETA(DisplayName = "Completed")
};

UENUM(BlueprintType)
enum class EWistoriaCompanionCommand : uint8
{
	Follow UMETA(DisplayName = "Follow"),
	HoldPosition UMETA(DisplayName = "Hold Position"),
	AttackTarget UMETA(DisplayName = "Attack Target"),
	InteractWithTarget UMETA(DisplayName = "Interact With Target"),
	DefendPlayer UMETA(DisplayName = "Defend Player")
};

USTRUCT(BlueprintType)
struct FWistoriaCompanionBond
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	FName CompanionId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	FText CompanionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion", meta = (ClampMin = "0"))
	int32 Affinity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion", meta = (ClampMin = "1"))
	int32 UltimateUnlockAffinity = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	FName TeamUltimateId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	bool bTeamUltimateUnlocked = false;
};

USTRUCT(BlueprintType)
struct FWistoriaAcademyRank
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FName RankId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FText RankTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "1"))
	int32 RequiredLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "0"))
	int32 RequiredRaidScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "0"))
	int32 RequiredRaidClears = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FName CosmeticCapeId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TSoftObjectPtr<USkeletalMesh> RewardCapeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	bool bUnlocked = false;
};

USTRUCT(BlueprintType)
struct FWistoriaInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName ItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EWistoriaItemType ItemType = EWistoriaItemType::Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EWistoriaItemRarity Rarity = EWistoriaItemRarity::Standard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EWistoriaEquipmentSlot EquipmentSlot = EWistoriaEquipmentSlot::MainHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "1"))
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float PhysicalAttackBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float MagicPowerBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float CourageBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float CritChanceBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float SpellChantSpeedBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float CourageGainMultiplierBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FWistoriaItemAffix> Affixes;
};

USTRUCT(BlueprintType)
struct FWistoriaShopStockItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	FWistoriaInventoryItem Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop", meta = (ClampMin = "0"))
	int32 BuyPrice = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop", meta = (ClampMin = "0"))
	int32 SellPrice = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	bool bUnlimitedStock = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop", meta = (ClampMin = "0"))
	int32 StockQuantity = 1;
};

USTRUCT(BlueprintType)
struct FWistoriaQuestObjective
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName ObjectiveId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (ClampMin = "1"))
	int32 RequiredCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (ClampMin = "0"))
	int32 CurrentCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bCompleted = false;
};

USTRUCT(BlueprintType)
struct FWistoriaQuestRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName QuestId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	EWistoriaQuestState State = EWistoriaQuestState::Inactive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TArray<FWistoriaQuestObjective> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 RewardXP = 0;
};

USTRUCT(BlueprintType)
struct FWistoriaSkillNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FName SkillId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (ClampMin = "1"))
	int32 SkillPointCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FName PrerequisiteSkillId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bUnlocked = false;
};

USTRUCT(BlueprintType)
struct FWistoriaCraftingIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName MaterialItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting", meta = (ClampMin = "1"))
	int32 RequiredQuantity = 1;
};

USTRUCT(BlueprintType)
struct FWistoriaCraftingRecipe : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName RecipeId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FName ResultItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	FText ResultDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	EWistoriaEquipmentSlot ResultEquipmentSlot = EWistoriaEquipmentSlot::MainHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TArray<FWistoriaCraftingIngredient> RequiredMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting", meta = (ClampMin = "0"))
	int32 GoldCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float ResultPhysicalAttackBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float ResultMagicPowerBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	float ResultCourageBonus = 0.0f;
};

USTRUCT(BlueprintType)
struct FWistoriaTeleportPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fast Travel")
	FName GateId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fast Travel")
	FText GateDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fast Travel")
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fast Travel")
	bool bUnlocked = false;
};

USTRUCT(BlueprintType)
struct FWistoriaWeaponDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	FName WeaponId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	FText WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	bool bIsWand = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	float BasePhysicalDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	float BaseMagicalMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	EWistoriaEquipmentSlot EquipmentSlot = EWistoriaEquipmentSlot::MainHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	TSoftObjectPtr<UStaticMesh> WeaponMeshAsset;
};

USTRUCT(BlueprintType)
struct FWistoriaSpellDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	FName SpellId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	FText SpellName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	EWistoriaElement Element = EWistoriaElement::Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	float ManaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	float BaseDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	float ExplosionRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Status")
	EWistoriaElement StatusToApply = EWistoriaElement::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Status", meta = (ClampMin = "0.0"))
	float StatusDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Status", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StatusChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	TSoftObjectPtr<UNiagaraSystem> SpellVFXAsset;
};

USTRUCT(BlueprintType)
struct FWistoriaHUDSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	float HealthPercent = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	float ManaPercent = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	float CouragePercent = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	int32 CurrentXP = 0;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	int32 Coins = 0;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	int32 Gold = 0;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	bool bIsTargeting = false;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	FText PrimaryQuestText;
};

USTRUCT(BlueprintType)
struct FWistoriaRaidSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	bool bRaidActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	bool bRaidComplete = false;

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	bool bRaidFailed = false;

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	int32 CurrentWave = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	int32 MaxWaves = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	int32 AliveEnemies = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	int32 TotalEnemiesDefeated = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Raid")
	int32 RaidScore = 0;
};

USTRUCT(BlueprintType)
struct FWistoriaSaveSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	int32 SlotIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	FString SlotName;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	FText SlotLabel;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	bool bHasSave = false;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	int32 CharacterLevel = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	int32 CurrentXP = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	int32 Gold = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	int32 NewGamePlusCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	int32 TotalRaidClearsCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	FName CurrentAcademyRankId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	FText CurrentAcademyRankTitle;
};
