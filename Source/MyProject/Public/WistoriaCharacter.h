#pragma once

#include "CoreMinimal.h"
#include "CoinCollector.h"
#include "GameFramework/Character.h"
#include "WistoriaTypes.h"
#include "WistoriaCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FResourceChangedSignature, float, CurrentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoinsChangedSignature, int32, NewCoinTotal);

UCLASS()
class MYPROJECT_API AWistoriaCharacter : public ACharacter, public ICoinCollector
{
	GENERATED_BODY()

public:
	AWistoriaCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void AddCoins_Implementation(int32 Amount) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecutePrimaryAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CastActiveSpell(int32 SpellSlot);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCombatStyle(EWistoriaCombatStyle NewCombatStyle);

	UFUNCTION(BlueprintPure, Category = "Combat")
	EWistoriaCombatStyle GetCombatStyle() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CastSpellSlotOne();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CastSpellFromLog(FName SpellId, class UDataTable* SpellDataTable);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateMeleeWindow(float DamageAmount = 15.0f);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DeactivateMeleeWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ReceiveDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Combat|Parry")
	void StartParry();

	UFUNCTION(BlueprintCallable, Category = "Combat|Parry")
	void EndParry();

	UFUNCTION(BlueprintCallable, Category = "Combat|Parry")
	bool TryResolveParry(AActor* Attacker);

	UFUNCTION(BlueprintPure, Category = "Combat|Parry")
	bool IsParrying() const;

	UFUNCTION(BlueprintCallable, Category = "Movement|Dodge")
	bool Dodge();

	UFUNCTION(BlueprintCallable, Category = "Customization")
	void ApplyCustomization();

	UFUNCTION(BlueprintCallable, Category = "Customization")
	void SetHairColor(const FLinearColor& NewHairColor);

	UFUNCTION(BlueprintCallable, Category = "Customization")
	void SetRobeColor(const FLinearColor& NewRobeColor);

	UFUNCTION(BlueprintPure, Category = "Customization")
	FLinearColor GetHairColor() const;

	UFUNCTION(BlueprintPure, Category = "Customization")
	FLinearColor GetRobeColor() const;

	UFUNCTION(BlueprintCallable, Category = "Mount")
	bool ToggleMount();

	UFUNCTION(BlueprintCallable, Category = "Mount")
	bool SummonAndMount();

	UFUNCTION(BlueprintCallable, Category = "Mount")
	bool Dismount();

	UFUNCTION(BlueprintPure, Category = "Mount")
	bool IsMounted() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool TryInteract();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool ToggleLockOn();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void ClearLockOn();

	UFUNCTION(BlueprintCallable, Category = "Companion")
	void RegisterCompanion(class AWistoriaCompanionCharacter* Companion);

	UFUNCTION(BlueprintCallable, Category = "Companion")
	bool RecruitCompanion(class ACharacter* NewCompanion);

	UFUNCTION(BlueprintCallable, Category = "Companion")
	void UnregisterCompanion(class AWistoriaCompanionCharacter* Companion);

	UFUNCTION(BlueprintPure, Category = "Companion")
	TArray<class AWistoriaCompanionCharacter*> GetActiveCompanions() const;

	UFUNCTION(BlueprintPure, Category = "Companion")
	int32 GetAvailableCompanionSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Companion")
	int32 IssueCompanionCommand(EWistoriaCompanionCommand Command, AActor* CommandTarget);

	UFUNCTION(BlueprintCallable, Category = "Companion")
	int32 CommandCompanionsToFollow();

	UFUNCTION(BlueprintCallable, Category = "Companion")
	int32 CommandCompanionsToHoldPosition();

	UFUNCTION(BlueprintCallable, Category = "Companion")
	int32 CommandCompanionsToAttackLockedTarget();

	UFUNCTION(BlueprintCallable, Category = "Companion")
	int32 CommandCompanionsToDefendPlayer();

	UFUNCTION(BlueprintCallable, Category = "Companion|Bond")
	bool AddCompanionAffinity(FName CompanionId, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Companion|Bond")
	int32 AwardAffinityToParty(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Companion|Bond")
	FWistoriaCompanionBond GetCompanionBond(FName CompanionId) const;

	UFUNCTION(BlueprintPure, Category = "Companion|Bond")
	TMap<FName, FWistoriaCompanionBond> GetCompanionBonds() const;

	UFUNCTION(BlueprintCallable, Category = "Companion|Bond")
	bool TryActivateTeamUltimate(FName CompanionId);

	UFUNCTION(BlueprintCallable, Category = "Companion|Bond")
	bool TriggerTeamUltimateCombo(FName CompanionId);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddXP(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddInventoryItem(const FWistoriaInventoryItem& Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool EquipInventoryItem(FName ItemId);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CraftOrUpgradeWeapon(const FWistoriaCraftingRecipe& Recipe);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool ReforgeWeaponSlot(int32 ItemIndex, int32 AffixIndexToReplace, int32 CostInGold);

	UFUNCTION(BlueprintCallable, Category = "Skills")
	bool PurchaseSkill(FName SkillId);

	UFUNCTION(BlueprintCallable, Category = "Progression|Academy")
	void AddAcademyRankScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Progression|Academy")
	void AddRaidClearProgress(int32 ClearCount = 1);

	UFUNCTION(BlueprintCallable, Category = "Progression|Academy")
	void EvaluateAcademyRanks();

	UFUNCTION(BlueprintPure, Category = "Progression|Academy")
	TArray<FWistoriaAcademyRank> GetAcademyRanks() const;

	UFUNCTION(BlueprintPure, Category = "Progression|Academy")
	FWistoriaAcademyRank GetCurrentAcademyRank() const;

	UFUNCTION(BlueprintPure, Category = "Progression|Academy")
	TArray<FName> GetUnlockedCosmeticCapeIds() const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartQuest(const FWistoriaQuestRecord& Quest);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AdvanceQuestObjective(FName QuestId, FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool NotifyEnemyKilled(FName EnemyId);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void SaveProgress(const FString& SlotName) const;

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool LoadProgress(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool InitiateNewGamePlus(FName IntroLevelName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Progression|Checkpoint")
	void ActivateCheckpoint(FName CheckpointId, const FVector& Location, const FRotator& Rotation, const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Progression|Checkpoint")
	bool RespawnAtCheckpoint(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Progression|Checkpoint")
	void RestoreAllResources();

	UFUNCTION(BlueprintCallable, Category = "Fast Travel")
	bool TeleportToGate(const FWistoriaTeleportPoint& TargetGate);

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetMaxHealthValue() const;

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetCurrentMana() const;

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetCurrentCourage() const;

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetMaxCourage() const;

	UFUNCTION(BlueprintPure, Category = "Attributes|Equipment")
	float GetCriticalChanceBonus() const;

	UFUNCTION(BlueprintPure, Category = "Attributes|Equipment")
	float GetSpellChantSpeedBonus() const;

	UFUNCTION(BlueprintPure, Category = "Attributes|Equipment")
	float GetCourageGainMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetCurrentXP() const;

	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetCoins() const;

	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsTargeting() const;

	UFUNCTION(BlueprintPure, Category = "Targeting")
	AActor* GetLockedTarget() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWistoriaMeleeHitbox> MeleeHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWistoriaStatusComponent> StatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWistoriaInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWistoriaQuestComponent> QuestComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWistoriaSkillComponent> SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> CosmeticCapeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	EWistoriaCombatStyle CombatStyle = EWistoriaCombatStyle::MagiclessSword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Magic", meta = (ClampMin = "0.0"))
	float MaxMana = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Magic")
	float CurrentMana = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Sword", meta = (ClampMin = "0.0"))
	float MaxCourage = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sword")
	float CurrentCourage = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 CharacterLevel = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 CurrentXP = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 Coins = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "1"))
	int32 XPPerLevel = 100;

	UPROPERTY(BlueprintReadOnly, Category = "Progression|Academy")
	int32 AcademyRankScore = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Progression|Academy")
	int32 TotalRaidClearsCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression|Academy")
	TArray<FWistoriaAcademyRank> AcademyRanks;

	UPROPERTY(BlueprintReadOnly, Category = "Progression|Academy")
	FName CurrentAcademyRankId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Progression|Academy")
	TArray<FName> UnlockedCosmeticCapeIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression|Loadout")
	TObjectPtr<class UWistoriaStarterLoadoutDataAsset> StarterLoadout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression|Loadout")
	bool bGrantStarterLoadoutOnBeginPlay = true;

	UPROPERTY(BlueprintReadOnly, Category = "Progression|Loadout")
	bool bStarterLoadoutGranted = false;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<FName> WeaponLog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebugResourceOverlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (EditCondition = "bShowDebugResourceOverlay"))
	int32 DebugResourceOverlayKey = 31001;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class AWistoriaSpellProjectile> SpellProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effects")
	TObjectPtr<class UNiagaraSystem> CourageBurstEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName SpellSpawnSocketName = TEXT("hand_rSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effects")
	FName CourageBurstAttachSocketName = TEXT("spine_03");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "100.0"))
	float InteractionRange = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> CastSpellAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> ParryAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Enhanced")
	TObjectPtr<class UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting", meta = (ClampMin = "100.0"))
	float LockOnRadius = 1500.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<AActor> LockedTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	bool bIsTargeting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Companion")
	TArray<TObjectPtr<class AWistoriaCompanionCharacter>> ActiveCompanions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion", meta = (ClampMin = "0", ClampMax = "3"))
	int32 MaxPartySize = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion|Bond")
	TMap<FName, FWistoriaCompanionBond> CompanionBonds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
	FLinearColor ActiveHairColor = FLinearColor(0.08f, 0.06f, 0.05f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
	FLinearColor ActiveRobeColor = FLinearColor(0.05f, 0.12f, 0.55f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization", meta = (ClampMin = "0"))
	int32 HairMaterialIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization", meta = (ClampMin = "0"))
	int32 RobeMaterialIndex = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
	FName HairColorParameterName = TEXT("BaseColor");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
	FName RobeColorParameterName = TEXT("BaseColor");

	UPROPERTY(BlueprintReadOnly, Category = "Customization")
	TObjectPtr<class UMaterialInstanceDynamic> HairMaterialInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Customization")
	TObjectPtr<class UMaterialInstanceDynamic> RobeMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	TSubclassOf<class AWistoriaMountCharacter> MountClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	FName MountSeatSocketName = TEXT("SaddleSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	FVector MountSpawnOffset = FVector(180.0f, 0.0f, 0.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Mount")
	TObjectPtr<class AWistoriaMountCharacter> CurrentMount;

	UPROPERTY(BlueprintReadOnly, Category = "Mount")
	bool bMounted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Parry", meta = (ClampMin = "0.05"))
	float ParryWindowDuration = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Parry", meta = (ClampMin = "0.0"))
	float ParryCourageReward = 20.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Combat|Parry")
	bool bIsParrying = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dodge", meta = (ClampMin = "100.0"))
	float DodgeStrength = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dodge", meta = (ClampMin = "0.05"))
	float DodgeCooldown = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dodge", meta = (ClampMin = "0.0"))
	float DodgeCourageCost = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement|Dodge")
	bool bCanDodge = true;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FResourceChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FResourceChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FResourceChangedSignature OnCourageChanged;

	UPROPERTY(BlueprintAssignable, Category = "Progression")
	FCoinsChangedSignature OnCoinsChanged;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnPrimaryAttackExecuted(EWistoriaCombatStyle Style);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnSpellCast(int32 SpellSlot, EWistoriaElement Element);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnCharacterDefeated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Progression|Checkpoint")
	void OnCheckpointActivated(FName CheckpointId);

	UFUNCTION(BlueprintImplementableEvent, Category = "Progression|Checkpoint")
	void OnRespawnedAtCheckpoint(FName CheckpointId);

	UFUNCTION(BlueprintImplementableEvent, Category = "Progression|Academy")
	void OnAcademyRankUnlocked(const FWistoriaAcademyRank& Rank);

	UFUNCTION(BlueprintImplementableEvent, Category = "Companion")
	void OnCompanionRosterChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Companion")
	void OnCompanionCommandIssued(EWistoriaCompanionCommand Command, AActor* CommandTarget, int32 CompanionCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Companion|Bond")
	void OnCompanionBondChanged(const FWistoriaCompanionBond& Bond);

	UFUNCTION(BlueprintImplementableEvent, Category = "Companion|Bond")
	void OnTeamUltimateActivated(const FWistoriaCompanionBond& Bond);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Parry")
	void OnParrySucceeded(AActor* Attacker);

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement|Dodge")
	void OnDodgeStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Customization")
	void OnCustomizationChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Mount")
	void OnMounted(class AWistoriaMountCharacter* Mount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mount")
	void OnDismounted(class AWistoriaMountCharacter* Mount);

private:
	FTimerHandle ParryTimerHandle;
	FTimerHandle DodgeCooldownTimerHandle;

	void ClampResources();
	void GrantStarterLoadout();
	void ApplyAcademyRankCape(const FWistoriaAcademyRank& Rank);
	void SpawnSpellProjectile(EWistoriaElement Element);
	void SpawnSpellProjectileWithDamage(EWistoriaElement Element, float DamageAmount);
	void SpawnSpellProjectileFromRow(const FWistoriaSpellDataRow& SpellRow);
	void SpawnCourageBurstEffect();
	bool IsMiasmaSuppressed() const;
	float GetMagicPowerMultiplier() const;
	float GetPhysicalAttackBonus() const;
	AActor* FindBestLockOnTarget() const;
	void UpdateLockOn(float DeltaTime);
	void DrawDebugResourceOverlay() const;
	void MoveEnhanced(const struct FInputActionValue& Value);
	void LookEnhanced(const struct FInputActionValue& Value);
	void HandleInteractInput();
	void HandleLockOnInput();
	void HandleDodgeInput();
	void HandleMountInput();
	void ResetDodge();
};
