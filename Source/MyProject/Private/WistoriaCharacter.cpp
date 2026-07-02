#include "WistoriaCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "WistoriaSaveGame.h"
#include "WistoriaSaveLibrary.h"
#include "WistoriaMeleeHitbox.h"
#include "WistoriaSpellProjectile.h"
#include "WistoriaStatusComponent.h"
#include "WistoriaInteractable.h"
#include "WistoriaInventoryComponent.h"
#include "WistoriaQuestComponent.h"
#include "WistoriaSkillComponent.h"
#include "WistoriaCompanionCharacter.h"
#include "WistoriaEnemy.h"
#include "WistoriaGameInstance.h"
#include "WistoriaMountCharacter.h"
#include "WistoriaRPGDataAssets.h"

AWistoriaCharacter::AWistoriaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 550.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	MeleeHitbox = CreateDefaultSubobject<UWistoriaMeleeHitbox>(TEXT("MeleeHitbox"));
	MeleeHitbox->SetupAttachment(GetMesh());
	MeleeHitbox->SetRelativeLocation(FVector(80.0f, 0.0f, 40.0f));

	StatusComponent = CreateDefaultSubobject<UWistoriaStatusComponent>(TEXT("StatusComponent"));
	InventoryComponent = CreateDefaultSubobject<UWistoriaInventoryComponent>(TEXT("InventoryComponent"));
	QuestComponent = CreateDefaultSubobject<UWistoriaQuestComponent>(TEXT("QuestComponent"));
	SkillComponent = CreateDefaultSubobject<UWistoriaSkillComponent>(TEXT("SkillComponent"));

	CosmeticCapeComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CosmeticCapeComponent"));
	CosmeticCapeComponent->SetupAttachment(GetMesh());
	CosmeticCapeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWistoriaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (DefaultMappingContext)
				{
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
		}
	}

	bool bLoadedActiveSave = false;
	FString ActiveSaveSlot;
	if (const UWistoriaGameInstance* WistoriaGameInstance = Cast<UWistoriaGameInstance>(GetGameInstance()))
	{
		ActiveSaveSlot = WistoriaGameInstance->GetActiveSaveSlot();
		if (!ActiveSaveSlot.IsEmpty())
		{
			if (const UWistoriaSaveGame* ActiveSave = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(ActiveSaveSlot, 0)))
			{
				if (ActiveSave->bSaveStarted)
				{
					bLoadedActiveSave = LoadProgress(ActiveSaveSlot);
				}
			}
		}
	}

	if (!bLoadedActiveSave)
	{
		ClampResources();
		GrantStarterLoadout();
		ApplyCustomization();
		if (!ActiveSaveSlot.IsEmpty())
		{
			SaveProgress(ActiveSaveSlot);
		}
	}

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	OnCourageChanged.Broadcast(CurrentCourage, MaxCourage);
	OnCoinsChanged.Broadcast(Coins);
}

void AWistoriaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateLockOn(DeltaTime);
	DrawDebugResourceOverlay();
}

void AWistoriaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWistoriaCharacter::MoveEnhanced);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWistoriaCharacter::LookEnhanced);
		}
		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (AttackAction)
		{
			EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &AWistoriaCharacter::ExecutePrimaryAttack);
		}
		if (CastSpellAction)
		{
			EnhancedInput->BindAction(CastSpellAction, ETriggerEvent::Started, this, &AWistoriaCharacter::CastSpellSlotOne);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AWistoriaCharacter::HandleInteractInput);
		}
		if (LockOnAction)
		{
			EnhancedInput->BindAction(LockOnAction, ETriggerEvent::Started, this, &AWistoriaCharacter::HandleLockOnInput);
		}
		if (ParryAction)
		{
			EnhancedInput->BindAction(ParryAction, ETriggerEvent::Started, this, &AWistoriaCharacter::StartParry);
		}
		if (DodgeAction)
		{
			EnhancedInput->BindAction(DodgeAction, ETriggerEvent::Started, this, &AWistoriaCharacter::HandleDodgeInput);
		}
	}

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AWistoriaCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AWistoriaCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AWistoriaCharacter::ExecutePrimaryAttack);
	PlayerInputComponent->BindAction(TEXT("CastSpell"), IE_Pressed, this, &AWistoriaCharacter::CastSpellSlotOne);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AWistoriaCharacter::HandleInteractInput);
	PlayerInputComponent->BindAction(TEXT("LockOn"), IE_Pressed, this, &AWistoriaCharacter::HandleLockOnInput);
	PlayerInputComponent->BindAction(TEXT("Parry"), IE_Pressed, this, &AWistoriaCharacter::StartParry);
	PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &AWistoriaCharacter::HandleDodgeInput);
	PlayerInputComponent->BindAction(TEXT("Mount"), IE_Pressed, this, &AWistoriaCharacter::HandleMountInput);
}

void AWistoriaCharacter::AddCoins_Implementation(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	Coins += Amount;
	OnCoinsChanged.Broadcast(Coins);
}

void AWistoriaCharacter::ExecutePrimaryAttack()
{
	if (CombatStyle == EWistoriaCombatStyle::MagiclessSword)
	{
		if (!IsMiasmaSuppressed())
		{
			CurrentCourage = FMath::Clamp(CurrentCourage + 5.0f * GetCourageGainMultiplier(), 0.0f, MaxCourage);
			OnCourageChanged.Broadcast(CurrentCourage, MaxCourage);
		}
	}
	else if (CurrentMana >= 5.0f)
	{
		const float ManaCost = IsMiasmaSuppressed() ? 10.0f : 5.0f;
		CurrentMana = FMath::Clamp(CurrentMana - ManaCost, 0.0f, MaxMana);
		OnManaChanged.Broadcast(CurrentMana, MaxMana);
	}

	OnPrimaryAttackExecuted(CombatStyle);
}

void AWistoriaCharacter::CastActiveSpell(int32 SpellSlot)
{
	EWistoriaElement Element = EWistoriaElement::None;

	if (CombatStyle == EWistoriaCombatStyle::MagiclessSword)
	{
		if (CurrentCourage < 80.0f)
		{
			return;
		}

		CurrentCourage = FMath::Clamp(CurrentCourage - 80.0f, 0.0f, MaxCourage);
		OnCourageChanged.Broadcast(CurrentCourage, MaxCourage);
		Element = EWistoriaElement::Lightning;
		SpawnCourageBurstEffect();
	}
	else
	{
		const float ManaCost = IsMiasmaSuppressed() ? 50.0f : 35.0f;
		if (CurrentMana < ManaCost)
		{
			return;
		}

		CurrentMana = FMath::Clamp(CurrentMana - ManaCost, 0.0f, MaxMana);
		OnManaChanged.Broadcast(CurrentMana, MaxMana);
		Element = SpellSlot == 1 ? EWistoriaElement::Fire : EWistoriaElement::Ice;
	}

	OnSpellCast(SpellSlot, Element);
	SpawnSpellProjectile(Element);
}

void AWistoriaCharacter::SetCombatStyle(EWistoriaCombatStyle NewCombatStyle)
{
	CombatStyle = NewCombatStyle;
	OnPrimaryAttackExecuted(CombatStyle);
}

EWistoriaCombatStyle AWistoriaCharacter::GetCombatStyle() const
{
	return CombatStyle;
}

void AWistoriaCharacter::CastSpellSlotOne()
{
	CastActiveSpell(1);
}

bool AWistoriaCharacter::CastSpellFromLog(FName SpellId, UDataTable* SpellDataTable)
{
	if (!SpellDataTable || SpellId.IsNone())
	{
		return false;
	}

	const FWistoriaSpellDataRow* SpellRow = SpellDataTable->FindRow<FWistoriaSpellDataRow>(SpellId, TEXT("WistoriaSpellCast"));
	if (!SpellRow || CurrentMana < SpellRow->ManaCost)
	{
		return false;
	}

	CurrentMana = FMath::Clamp(CurrentMana - SpellRow->ManaCost, 0.0f, MaxMana);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	OnSpellCast(1, SpellRow->Element);
	SpawnSpellProjectileFromRow(*SpellRow);
	return true;
}

void AWistoriaCharacter::ActivateMeleeWindow(float DamageAmount)
{
	if (MeleeHitbox)
	{
		const EWistoriaElement AttackElement = CombatStyle == EWistoriaCombatStyle::MagiclessSword
			? EWistoriaElement::None
			: EWistoriaElement::Fire;
		float FinalDamage = DamageAmount + GetPhysicalAttackBonus();
		if (FMath::FRand() <= GetCriticalChanceBonus())
		{
			FinalDamage *= 1.5f;
		}
		MeleeHitbox->ActivateHitbox(FinalDamage, AttackElement);
	}
}

void AWistoriaCharacter::DeactivateMeleeWindow()
{
	if (MeleeHitbox)
	{
		MeleeHitbox->DeactivateHitbox();
	}
}

void AWistoriaCharacter::ReceiveDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		OnCharacterDefeated();
	}
}

void AWistoriaCharacter::StartParry()
{
	bIsParrying = true;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ParryTimerHandle,
			this,
			&AWistoriaCharacter::EndParry,
			ParryWindowDuration,
			false);
	}
}

void AWistoriaCharacter::EndParry()
{
	bIsParrying = false;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ParryTimerHandle);
	}
}

bool AWistoriaCharacter::TryResolveParry(AActor* Attacker)
{
	if (!bIsParrying)
	{
		return false;
	}

	EndParry();
	CurrentCourage = FMath::Clamp(CurrentCourage + ParryCourageReward, 0.0f, MaxCourage);
	OnCourageChanged.Broadcast(CurrentCourage, MaxCourage);
	OnParrySucceeded(Attacker);
	return true;
}

bool AWistoriaCharacter::IsParrying() const
{
	return bIsParrying;
}

bool AWistoriaCharacter::Dodge()
{
	if (!bCanDodge || CurrentHealth <= 0.0f || CurrentCourage < DodgeCourageCost)
	{
		return false;
	}

	if (DodgeCourageCost > 0.0f)
	{
		CurrentCourage = FMath::Clamp(CurrentCourage - DodgeCourageCost, 0.0f, MaxCourage);
		OnCourageChanged.Broadcast(CurrentCourage, MaxCourage);
	}

	FVector DodgeDirection = GetLastMovementInputVector();
	if (DodgeDirection.IsNearlyZero())
	{
		DodgeDirection = GetActorForwardVector();
	}
	DodgeDirection.Z = 0.0f;
	DodgeDirection.Normalize();

	LaunchCharacter(DodgeDirection * DodgeStrength, true, false);
	bCanDodge = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			DodgeCooldownTimerHandle,
			this,
			&AWistoriaCharacter::ResetDodge,
			DodgeCooldown,
			false);
	}

	OnDodgeStarted();
	return true;
}

void AWistoriaCharacter::ApplyCustomization()
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (!CharacterMesh)
	{
		return;
	}

	if (!HairMaterialInstance && CharacterMesh->GetMaterial(HairMaterialIndex))
	{
		HairMaterialInstance = CharacterMesh->CreateDynamicMaterialInstance(HairMaterialIndex);
	}
	if (!RobeMaterialInstance && CharacterMesh->GetMaterial(RobeMaterialIndex))
	{
		RobeMaterialInstance = CharacterMesh->CreateDynamicMaterialInstance(RobeMaterialIndex);
	}

	if (HairMaterialInstance)
	{
		HairMaterialInstance->SetVectorParameterValue(HairColorParameterName, ActiveHairColor);
	}
	if (RobeMaterialInstance)
	{
		RobeMaterialInstance->SetVectorParameterValue(RobeColorParameterName, ActiveRobeColor);
	}
}

void AWistoriaCharacter::SetHairColor(const FLinearColor& NewHairColor)
{
	ActiveHairColor = NewHairColor;
	ApplyCustomization();
	OnCustomizationChanged();
}

void AWistoriaCharacter::SetRobeColor(const FLinearColor& NewRobeColor)
{
	ActiveRobeColor = NewRobeColor;
	ApplyCustomization();
	OnCustomizationChanged();
}

FLinearColor AWistoriaCharacter::GetHairColor() const
{
	return ActiveHairColor;
}

FLinearColor AWistoriaCharacter::GetRobeColor() const
{
	return ActiveRobeColor;
}

bool AWistoriaCharacter::ToggleMount()
{
	return bMounted ? Dismount() : SummonAndMount();
}

bool AWistoriaCharacter::SummonAndMount()
{
	if (bMounted || !MountClass || !GetWorld())
	{
		return false;
	}

	const FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(MountSpawnOffset);
	const FRotator SpawnRotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	CurrentMount = GetWorld()->SpawnActor<AWistoriaMountCharacter>(MountClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!CurrentMount)
	{
		return false;
	}

	if (!CurrentMount->MountRider(this, MountSeatSocketName))
	{
		CurrentMount->Destroy();
		CurrentMount = nullptr;
		return false;
	}

	bMounted = true;
	OnMounted(CurrentMount);
	return true;
}

bool AWistoriaCharacter::Dismount()
{
	if (!bMounted || !CurrentMount)
	{
		return false;
	}

	AWistoriaMountCharacter* MountToDismount = CurrentMount;
	MountToDismount->DismountRider();
	bMounted = false;
	CurrentMount = nullptr;
	OnDismounted(MountToDismount);
	return true;
}

bool AWistoriaCharacter::IsMounted() const
{
	return bMounted;
}

void AWistoriaCharacter::MoveForward(float Value)
{
	if (Controller && !FMath::IsNearlyZero(Value))
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
	}
}

void AWistoriaCharacter::MoveRight(float Value)
{
	if (Controller && !FMath::IsNearlyZero(Value))
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
	}
}

void AWistoriaCharacter::MoveEnhanced(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	MoveForward(MovementVector.Y);
	MoveRight(MovementVector.X);
}

void AWistoriaCharacter::LookEnhanced(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void AWistoriaCharacter::HandleInteractInput()
{
	TryInteract();
}

void AWistoriaCharacter::HandleLockOnInput()
{
	ToggleLockOn();
}

void AWistoriaCharacter::HandleDodgeInput()
{
	Dodge();
}

void AWistoriaCharacter::HandleMountInput()
{
	ToggleMount();
}

bool AWistoriaCharacter::TryInteract()
{
	if (!Controller || !GetWorld())
	{
		return false;
	}

	const FVector Start = FollowCamera ? FollowCamera->GetComponentLocation() : GetActorLocation();
	const FVector Direction = FollowCamera ? FollowCamera->GetForwardVector() : GetActorForwardVector();
	const FVector End = Start + Direction * InteractionRange;

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WistoriaInteractTrace), false, this);
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
	{
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor || !HitActor->GetClass()->ImplementsInterface(UWistoriaInteractable::StaticClass()))
	{
		return false;
	}

	IWistoriaInteractable::Execute_Interact(HitActor, this);
	return true;
}

bool AWistoriaCharacter::ToggleLockOn()
{
	if (bIsTargeting)
	{
		ClearLockOn();
		return false;
	}

	LockedTarget = FindBestLockOnTarget();
	bIsTargeting = LockedTarget != nullptr;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = !bIsTargeting;
		Movement->bUseControllerDesiredRotation = bIsTargeting;
	}

	return bIsTargeting;
}

void AWistoriaCharacter::ClearLockOn()
{
	LockedTarget = nullptr;
	bIsTargeting = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->bUseControllerDesiredRotation = false;
	}
}

void AWistoriaCharacter::RegisterCompanion(AWistoriaCompanionCharacter* Companion)
{
	if (!Companion)
	{
		return;
	}

	ActiveCompanions.RemoveAll([](const AWistoriaCompanionCharacter* ExistingCompanion)
	{
		return !IsValid(ExistingCompanion);
	});

	if (ActiveCompanions.Contains(Companion))
	{
		Companion->SetFollowTarget(this);
		return;
	}

	if (ActiveCompanions.Num() >= MaxPartySize)
	{
		return;
	}

	ActiveCompanions.AddUnique(Companion);
	Companion->SetFollowTarget(this);
	const FName CompanionId = Companion->GetCompanionId();
	if (!CompanionId.IsNone())
	{
		FWistoriaCompanionBond& Bond = CompanionBonds.FindOrAdd(CompanionId);
		Bond.CompanionId = CompanionId;
		if (Bond.CompanionName.IsEmpty())
		{
			Bond.CompanionName = FText::FromName(CompanionId);
		}
		OnCompanionBondChanged(Bond);
	}
	OnCompanionRosterChanged();
}

bool AWistoriaCharacter::RecruitCompanion(ACharacter* NewCompanion)
{
	AWistoriaCompanionCharacter* Companion = Cast<AWistoriaCompanionCharacter>(NewCompanion);
	if (!Companion)
	{
		return false;
	}

	if (ActiveCompanions.Contains(Companion))
	{
		Companion->SetFollowTarget(this);
		return true;
	}

	if (GetAvailableCompanionSlots() <= 0)
	{
		return false;
	}

	RegisterCompanion(Companion);
	return ActiveCompanions.Contains(Companion);
}

void AWistoriaCharacter::UnregisterCompanion(AWistoriaCompanionCharacter* Companion)
{
	if (Companion && ActiveCompanions.Remove(Companion) > 0)
	{
		OnCompanionRosterChanged();
	}
}

TArray<AWistoriaCompanionCharacter*> AWistoriaCharacter::GetActiveCompanions() const
{
	TArray<AWistoriaCompanionCharacter*> Companions;
	for (AWistoriaCompanionCharacter* Companion : ActiveCompanions)
	{
		if (IsValid(Companion))
		{
			Companions.Add(Companion);
		}
	}
	return Companions;
}

int32 AWistoriaCharacter::GetAvailableCompanionSlots() const
{
	int32 ValidCompanionCount = 0;
	for (const AWistoriaCompanionCharacter* Companion : ActiveCompanions)
	{
		if (IsValid(Companion))
		{
			ValidCompanionCount++;
		}
	}

	return FMath::Max(0, MaxPartySize - ValidCompanionCount);
}

int32 AWistoriaCharacter::IssueCompanionCommand(EWistoriaCompanionCommand Command, AActor* CommandTarget)
{
	AActor* ResolvedTarget = CommandTarget;
	if (Command == EWistoriaCompanionCommand::Follow)
	{
		ResolvedTarget = this;
	}
	else if (Command == EWistoriaCompanionCommand::HoldPosition)
	{
		ResolvedTarget = nullptr;
	}

	int32 IssuedCount = 0;
	for (AWistoriaCompanionCharacter* Companion : GetActiveCompanions())
	{
		Companion->IssueCommand(Command, ResolvedTarget);
		IssuedCount++;
	}

	OnCompanionCommandIssued(Command, ResolvedTarget, IssuedCount);
	return IssuedCount;
}

int32 AWistoriaCharacter::CommandCompanionsToFollow()
{
	return IssueCompanionCommand(EWistoriaCompanionCommand::Follow, this);
}

int32 AWistoriaCharacter::CommandCompanionsToHoldPosition()
{
	return IssueCompanionCommand(EWistoriaCompanionCommand::HoldPosition, nullptr);
}

int32 AWistoriaCharacter::CommandCompanionsToAttackLockedTarget()
{
	if (!LockedTarget)
	{
		return 0;
	}

	return IssueCompanionCommand(EWistoriaCompanionCommand::AttackTarget, LockedTarget);
}

int32 AWistoriaCharacter::CommandCompanionsToDefendPlayer()
{
	return IssueCompanionCommand(EWistoriaCompanionCommand::DefendPlayer, this);
}

bool AWistoriaCharacter::AddCompanionAffinity(FName CompanionId, int32 Amount)
{
	if (CompanionId.IsNone() || Amount == 0)
	{
		return false;
	}

	FWistoriaCompanionBond& Bond = CompanionBonds.FindOrAdd(CompanionId);
	Bond.CompanionId = CompanionId;
	Bond.Affinity = FMath::Max(0, Bond.Affinity + Amount);

	if (!Bond.bTeamUltimateUnlocked && Bond.Affinity >= Bond.UltimateUnlockAffinity)
	{
		Bond.bTeamUltimateUnlocked = true;
	}

	OnCompanionBondChanged(Bond);
	return true;
}

int32 AWistoriaCharacter::AwardAffinityToParty(int32 Amount)
{
	if (Amount == 0)
	{
		return 0;
	}

	int32 AwardedCount = 0;
	for (AWistoriaCompanionCharacter* Companion : GetActiveCompanions())
	{
		if (Companion && AddCompanionAffinity(Companion->GetCompanionId(), Amount))
		{
			AwardedCount++;
		}
	}

	return AwardedCount;
}

FWistoriaCompanionBond AWistoriaCharacter::GetCompanionBond(FName CompanionId) const
{
	if (const FWistoriaCompanionBond* Bond = CompanionBonds.Find(CompanionId))
	{
		return *Bond;
	}

	FWistoriaCompanionBond EmptyBond;
	EmptyBond.CompanionId = CompanionId;
	return EmptyBond;
}

TMap<FName, FWistoriaCompanionBond> AWistoriaCharacter::GetCompanionBonds() const
{
	return CompanionBonds;
}

bool AWistoriaCharacter::TryActivateTeamUltimate(FName CompanionId)
{
	FWistoriaCompanionBond* Bond = CompanionBonds.Find(CompanionId);
	if (!Bond || !Bond->bTeamUltimateUnlocked)
	{
		return false;
	}

	OnTeamUltimateActivated(*Bond);
	return true;
}

bool AWistoriaCharacter::TriggerTeamUltimateCombo(FName CompanionId)
{
	FWistoriaCompanionBond* Bond = CompanionBonds.Find(CompanionId);
	if (!Bond || !Bond->bTeamUltimateUnlocked || !LockedTarget)
	{
		return false;
	}

	const float UltimateDamage = 1500.0f * GetMagicPowerMultiplier();
	if (AWistoriaEnemy* Enemy = Cast<AWistoriaEnemy>(LockedTarget))
	{
		Enemy->ReceiveDamageFrom(UltimateDamage, EWistoriaElement::Lightning, this);
		OnTeamUltimateActivated(*Bond);
		return true;
	}

	return false;
}

void AWistoriaCharacter::AddXP(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	CurrentXP += Amount;
	while (CurrentXP >= XPPerLevel)
	{
		CurrentXP -= XPPerLevel;
		CharacterLevel++;
		EvaluateAcademyRanks();
		if (SkillComponent)
		{
			SkillComponent->AddSkillPoints(1);
		}
		MaxHealth += 10.0f;
		MaxMana += 5.0f;
		MaxCourage += 5.0f;
		CurrentHealth = MaxHealth;
		CurrentMana = MaxMana;
		CurrentCourage = FMath::Clamp(CurrentCourage + 15.0f, 0.0f, MaxCourage);
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
		OnManaChanged.Broadcast(CurrentMana, MaxMana);
		OnCourageChanged.Broadcast(CurrentCourage, MaxCourage);
	}
}

void AWistoriaCharacter::AddInventoryItem(const FWistoriaInventoryItem& Item)
{
	if (InventoryComponent)
	{
		InventoryComponent->AddItem(Item);
	}

	if (Item.ItemType == EWistoriaItemType::Weapon && !Item.ItemId.IsNone())
	{
		WeaponLog.AddUnique(Item.ItemId);
	}
}

bool AWistoriaCharacter::EquipInventoryItem(FName ItemId)
{
	return InventoryComponent && InventoryComponent->EquipItem(ItemId);
}

bool AWistoriaCharacter::CraftOrUpgradeWeapon(const FWistoriaCraftingRecipe& Recipe)
{
	if (!InventoryComponent || Recipe.ResultItemId.IsNone())
	{
		return false;
	}

	for (const FWistoriaCraftingIngredient& Ingredient : Recipe.RequiredMaterials)
	{
		if (!InventoryComponent->HasItem(Ingredient.MaterialItemId, Ingredient.RequiredQuantity))
		{
			return false;
		}
	}

	if (!InventoryComponent->SpendGold(Recipe.GoldCost))
	{
		return false;
	}

	for (const FWistoriaCraftingIngredient& Ingredient : Recipe.RequiredMaterials)
	{
		InventoryComponent->RemoveItem(Ingredient.MaterialItemId, Ingredient.RequiredQuantity);
	}

	FWistoriaInventoryItem ResultItem;
	ResultItem.ItemId = Recipe.ResultItemId;
	ResultItem.DisplayName = Recipe.ResultDisplayName;
	ResultItem.ItemType = EWistoriaItemType::Weapon;
	ResultItem.EquipmentSlot = Recipe.ResultEquipmentSlot;
	ResultItem.Quantity = 1;
	ResultItem.PhysicalAttackBonus = Recipe.ResultPhysicalAttackBonus;
	ResultItem.MagicPowerBonus = Recipe.ResultMagicPowerBonus;
	ResultItem.CourageBonus = Recipe.ResultCourageBonus;
	AddInventoryItem(ResultItem);
	return true;
}

bool AWistoriaCharacter::ReforgeWeaponSlot(int32 ItemIndex, int32 AffixIndexToReplace, int32 CostInGold)
{
	return InventoryComponent && InventoryComponent->ReforgeWeaponAffix(ItemIndex, AffixIndexToReplace, CostInGold);
}

bool AWistoriaCharacter::PurchaseSkill(FName SkillId)
{
	return SkillComponent && SkillComponent->PurchaseSkill(SkillId);
}

void AWistoriaCharacter::AddAcademyRankScore(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	AcademyRankScore += Amount;
	EvaluateAcademyRanks();
}

void AWistoriaCharacter::AddRaidClearProgress(int32 ClearCount)
{
	if (ClearCount <= 0)
	{
		return;
	}

	TotalRaidClearsCount += ClearCount;
	EvaluateAcademyRanks();
}

void AWistoriaCharacter::EvaluateAcademyRanks()
{
	for (FWistoriaAcademyRank& Rank : AcademyRanks)
	{
		if (Rank.bUnlocked || CharacterLevel < Rank.RequiredLevel || AcademyRankScore < Rank.RequiredRaidScore || TotalRaidClearsCount < Rank.RequiredRaidClears)
		{
			continue;
		}

		Rank.bUnlocked = true;
		CurrentAcademyRankId = Rank.RankId;
		if (!Rank.CosmeticCapeId.IsNone())
		{
			UnlockedCosmeticCapeIds.AddUnique(Rank.CosmeticCapeId);
		}
		ApplyAcademyRankCape(Rank);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				6.0f,
				FColor::Yellow,
				FString::Printf(TEXT("ACADEMY ACCLAIM: You have earned the title: %s!"), *Rank.RankTitle.ToString()));
		}
		OnAcademyRankUnlocked(Rank);
	}
}

TArray<FWistoriaAcademyRank> AWistoriaCharacter::GetAcademyRanks() const
{
	return AcademyRanks;
}

FWistoriaAcademyRank AWistoriaCharacter::GetCurrentAcademyRank() const
{
	for (const FWistoriaAcademyRank& Rank : AcademyRanks)
	{
		if (Rank.RankId == CurrentAcademyRankId)
		{
			return Rank;
		}
	}

	return FWistoriaAcademyRank();
}

TArray<FName> AWistoriaCharacter::GetUnlockedCosmeticCapeIds() const
{
	return UnlockedCosmeticCapeIds;
}

void AWistoriaCharacter::ApplyAcademyRankCape(const FWistoriaAcademyRank& Rank)
{
	if (!CosmeticCapeComponent || Rank.RewardCapeMesh.IsNull())
	{
		return;
	}

	if (USkeletalMesh* LoadedCapeMesh = Rank.RewardCapeMesh.LoadSynchronous())
	{
		CosmeticCapeComponent->SetSkeletalMesh(LoadedCapeMesh);
	}
}

void AWistoriaCharacter::StartQuest(const FWistoriaQuestRecord& Quest)
{
	if (QuestComponent)
	{
		QuestComponent->StartQuest(Quest);
	}
}

bool AWistoriaCharacter::AdvanceQuestObjective(FName QuestId, FName ObjectiveId, int32 Amount)
{
	return QuestComponent && QuestComponent->AdvanceObjective(QuestId, ObjectiveId, Amount);
}

bool AWistoriaCharacter::NotifyEnemyKilled(FName EnemyId)
{
	if (!QuestComponent)
	{
		return false;
	}

	TArray<FWistoriaQuestRecord> CompletedQuests;
	const bool bAdvanced = QuestComponent->AdvanceAnyActiveObjectiveWithCompletions(EnemyId, 1, CompletedQuests);
	for (const FWistoriaQuestRecord& Quest : CompletedQuests)
	{
		AddXP(Quest.RewardXP);
	}
	return bAdvanced;
}

void AWistoriaCharacter::SaveProgress(const FString& SlotName) const
{
	UWistoriaSaveGame* SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!SaveGame)
	{
		SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::CreateSaveGameObject(UWistoriaSaveGame::StaticClass()));
	}

	if (!SaveGame)
	{
		return;
	}

	SaveGame->bSaveStarted = true;
	SaveGame->CharacterLevel = CharacterLevel;
	SaveGame->CurrentXP = CurrentXP;
	SaveGame->Coins = Coins;
	SaveGame->AcademyRankScore = AcademyRankScore;
	SaveGame->TotalRaidClearsCount = TotalRaidClearsCount;
	SaveGame->AcademyRanks = AcademyRanks;
	SaveGame->CurrentAcademyRankId = CurrentAcademyRankId;
	SaveGame->UnlockedCosmeticCapeIds = UnlockedCosmeticCapeIds;
	SaveGame->WeaponLog = WeaponLog;
	SaveGame->HairColor = ActiveHairColor;
	SaveGame->RobeColor = ActiveRobeColor;
	if (InventoryComponent)
	{
		SaveGame->InventoryItems = InventoryComponent->GetItems();
		SaveGame->EquippedItems = InventoryComponent->GetEquippedItems();
		SaveGame->Gold = InventoryComponent->GetGold();
	}
	if (QuestComponent)
	{
		SaveGame->QuestRecords = QuestComponent->GetQuests();
	}
	if (SkillComponent)
	{
		SaveGame->AvailableSkillPoints = SkillComponent->GetAvailableSkillPoints();
		SaveGame->SkillTree = SkillComponent->GetSkillTree();
	}
	SaveGame->CompanionBonds = CompanionBonds;

	UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);
}

bool AWistoriaCharacter::LoadProgress(const FString& SlotName)
{
	UWistoriaSaveGame* SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!SaveGame)
	{
		return false;
	}

	CharacterLevel = SaveGame->CharacterLevel;
	CurrentXP = SaveGame->CurrentXP;
	Coins = SaveGame->Coins;
	AcademyRankScore = SaveGame->AcademyRankScore;
	TotalRaidClearsCount = SaveGame->TotalRaidClearsCount;
	AcademyRanks = SaveGame->AcademyRanks;
	CurrentAcademyRankId = SaveGame->CurrentAcademyRankId;
	UnlockedCosmeticCapeIds = SaveGame->UnlockedCosmeticCapeIds;
	WeaponLog = SaveGame->WeaponLog;
	ActiveHairColor = SaveGame->HairColor;
	ActiveRobeColor = SaveGame->RobeColor;
	if (InventoryComponent)
	{
		InventoryComponent->SetItems(SaveGame->InventoryItems);
		InventoryComponent->SetEquippedItems(SaveGame->EquippedItems);
		InventoryComponent->SetGold(SaveGame->Gold);
	}
	if (QuestComponent)
	{
		QuestComponent->SetQuests(SaveGame->QuestRecords);
	}
	if (SkillComponent)
	{
		SkillComponent->SetAvailableSkillPoints(SaveGame->AvailableSkillPoints);
		SkillComponent->SetSkillTree(SaveGame->SkillTree);
	}
	CompanionBonds = SaveGame->CompanionBonds;

	ClampResources();
	ApplyCustomization();
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	OnCourageChanged.Broadcast(CurrentCourage, MaxCourage);
	OnCoinsChanged.Broadcast(Coins);
	return true;
}

bool AWistoriaCharacter::InitiateNewGamePlus(FName IntroLevelName)
{
	const UWistoriaGameInstance* WistoriaGameInstance = Cast<UWistoriaGameInstance>(GetGameInstance());
	const FString SlotName = WistoriaGameInstance ? WistoriaGameInstance->GetActiveSaveSlot() : UWistoriaSaveLibrary::GetDefaultSaveSlot();
	if (!UWistoriaSaveLibrary::StartNewGamePlus(this, SlotName))
	{
		return false;
	}

	if (IntroLevelName != NAME_None)
	{
		UGameplayStatics::OpenLevel(this, IntroLevelName);
	}

	return true;
}

void AWistoriaCharacter::ActivateCheckpoint(FName CheckpointId, const FVector& Location, const FRotator& Rotation, const FString& SlotName)
{
	UWistoriaSaveGame* SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!SaveGame)
	{
		SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::CreateSaveGameObject(UWistoriaSaveGame::StaticClass()));
	}

	if (!SaveGame)
	{
		return;
	}

	SaveGame->ActiveCheckpointId = CheckpointId;
	SaveGame->CheckpointLocation = Location;
	SaveGame->CheckpointRotation = Rotation;
	SaveGame->bHasCheckpoint = true;
	UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);

	SaveProgress(SlotName);
	OnCheckpointActivated(CheckpointId);
}

bool AWistoriaCharacter::RespawnAtCheckpoint(const FString& SlotName)
{
	UWistoriaSaveGame* SaveGame = Cast<UWistoriaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!SaveGame || !SaveGame->bHasCheckpoint)
	{
		return false;
	}

	SetActorLocationAndRotation(SaveGame->CheckpointLocation, SaveGame->CheckpointRotation, false, nullptr, ETeleportType::TeleportPhysics);
	RestoreAllResources();
	OnRespawnedAtCheckpoint(SaveGame->ActiveCheckpointId);
	return true;
}

void AWistoriaCharacter::RestoreAllResources()
{
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	CurrentCourage = MaxCourage;
	bIsParrying = false;
	bCanDodge = true;
	ClearLockOn();
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	OnCourageChanged.Broadcast(CurrentCourage, MaxCourage);
}

bool AWistoriaCharacter::TeleportToGate(const FWistoriaTeleportPoint& TargetGate)
{
	if (!TargetGate.bUnlocked)
	{
		return false;
	}

	return SetActorLocation(TargetGate.WorldLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

float AWistoriaCharacter::GetCurrentHealth() const
{
	return CurrentHealth;
}

float AWistoriaCharacter::GetMaxHealthValue() const
{
	return MaxHealth;
}

float AWistoriaCharacter::GetCurrentMana() const
{
	return CurrentMana;
}

float AWistoriaCharacter::GetMaxMana() const
{
	return MaxMana;
}

float AWistoriaCharacter::GetCurrentCourage() const
{
	return CurrentCourage;
}

float AWistoriaCharacter::GetMaxCourage() const
{
	return MaxCourage;
}

float AWistoriaCharacter::GetCriticalChanceBonus() const
{
	return InventoryComponent ? InventoryComponent->GetTotalCritChanceBonus() : 0.0f;
}

float AWistoriaCharacter::GetSpellChantSpeedBonus() const
{
	return InventoryComponent ? InventoryComponent->GetTotalSpellChantSpeedBonus() : 0.0f;
}

float AWistoriaCharacter::GetCourageGainMultiplier() const
{
	return 1.0f + (InventoryComponent ? InventoryComponent->GetTotalCourageGainMultiplierBonus() : 0.0f);
}

int32 AWistoriaCharacter::GetCharacterLevel() const
{
	return CharacterLevel;
}

int32 AWistoriaCharacter::GetCurrentXP() const
{
	return CurrentXP;
}

int32 AWistoriaCharacter::GetCoins() const
{
	return Coins;
}

bool AWistoriaCharacter::IsTargeting() const
{
	return bIsTargeting;
}

AActor* AWistoriaCharacter::GetLockedTarget() const
{
	return LockedTarget;
}

void AWistoriaCharacter::ClampResources()
{
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	CurrentMana = FMath::Clamp(CurrentMana, 0.0f, MaxMana);
	CurrentCourage = FMath::Clamp(CurrentCourage, 0.0f, MaxCourage);
}

void AWistoriaCharacter::GrantStarterLoadout()
{
	if (!bGrantStarterLoadoutOnBeginPlay || bStarterLoadoutGranted || !StarterLoadout)
	{
		return;
	}

	for (const FWistoriaInventoryItem& Item : StarterLoadout->MakeStartingInventory())
	{
		AddInventoryItem(Item);
	}

	for (const FWistoriaQuestRecord& Quest : StarterLoadout->MakeStartingQuests())
	{
		StartQuest(Quest);
	}

	if (StarterLoadout->StartingGold > 0 && InventoryComponent)
	{
		InventoryComponent->AddGold(StarterLoadout->StartingGold);
	}

	for (const FName WeaponId : StarterLoadout->UnlockedWeaponIds)
	{
		if (!WeaponId.IsNone())
		{
			WeaponLog.AddUnique(WeaponId);
		}
	}

	bStarterLoadoutGranted = true;
}

void AWistoriaCharacter::SpawnSpellProjectile(EWistoriaElement Element)
{
	SpawnSpellProjectileWithDamage(Element, 30.0f * GetMagicPowerMultiplier());
}

void AWistoriaCharacter::SpawnSpellProjectileWithDamage(EWistoriaElement Element, float DamageAmount)
{
	if (!SpellProjectileClass || !GetWorld())
	{
		return;
	}

	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0.0f, 0.0f, 60.0f);
	FRotator SpawnRotation = GetControlRotation();

	if (GetMesh() && GetMesh()->DoesSocketExist(SpellSpawnSocketName))
	{
		SpawnLocation = GetMesh()->GetSocketLocation(SpellSpawnSocketName);
		SpawnRotation = GetMesh()->GetSocketRotation(SpellSpawnSocketName);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	AWistoriaSpellProjectile* Projectile = GetWorld()->SpawnActor<AWistoriaSpellProjectile>(
		SpellProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	if (Projectile)
	{
		Projectile->ConfigureProjectile(DamageAmount, Element, this);
	}
}

void AWistoriaCharacter::SpawnSpellProjectileFromRow(const FWistoriaSpellDataRow& SpellRow)
{
	if (!SpellProjectileClass || !GetWorld())
	{
		return;
	}

	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0.0f, 0.0f, 60.0f);
	FRotator SpawnRotation = GetControlRotation();

	if (GetMesh() && GetMesh()->DoesSocketExist(SpellSpawnSocketName))
	{
		SpawnLocation = GetMesh()->GetSocketLocation(SpellSpawnSocketName);
		SpawnRotation = GetMesh()->GetSocketRotation(SpellSpawnSocketName);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	AWistoriaSpellProjectile* Projectile = GetWorld()->SpawnActor<AWistoriaSpellProjectile>(
		SpellProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	if (Projectile)
	{
		Projectile->ConfigureProjectileFromSpellRow(SpellRow, GetMagicPowerMultiplier(), this);
	}
}

void AWistoriaCharacter::SpawnCourageBurstEffect()
{
	if (!CourageBurstEffect || !GetWorld())
	{
		return;
	}

	if (GetMesh() && GetMesh()->DoesSocketExist(CourageBurstAttachSocketName))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			CourageBurstEffect,
			GetMesh(),
			CourageBurstAttachSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true);
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		CourageBurstEffect,
		GetActorLocation(),
		GetActorRotation());
}

bool AWistoriaCharacter::IsMiasmaSuppressed() const
{
	return StatusComponent && StatusComponent->HasStatus(EWistoriaElement::Miasma);
}

float AWistoriaCharacter::GetMagicPowerMultiplier() const
{
	float Multiplier = 1.0f + GetSpellChantSpeedBonus() + (InventoryComponent ? InventoryComponent->GetTotalMagicBonus() : 0.0f);
	if (IsMiasmaSuppressed())
	{
		Multiplier *= 0.7f;
	}
	return Multiplier;
}

float AWistoriaCharacter::GetPhysicalAttackBonus() const
{
	return InventoryComponent ? InventoryComponent->GetTotalPhysicalBonus() : 0.0f;
}

AActor* AWistoriaCharacter::FindBestLockOnTarget() const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	TArray<FOverlapResult> Results;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(LockOnRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(WistoriaLockOnTrace), false, this);
	GetWorld()->OverlapMultiByObjectType(
		Results,
		GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		Sphere,
		Params);

	AActor* BestTarget = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();
	for (const FOverlapResult& Result : Results)
	{
		AActor* Candidate = Result.GetActor();
		if (!Candidate || !Candidate->IsA<AWistoriaEnemy>())
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(GetActorLocation(), Candidate->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}

void AWistoriaCharacter::UpdateLockOn(float DeltaTime)
{
	if (!bIsTargeting || !LockedTarget || !Controller)
	{
		return;
	}

	if (FVector::DistSquared(GetActorLocation(), LockedTarget->GetActorLocation()) > FMath::Square(LockOnRadius * 1.25f))
	{
		ClearLockOn();
		return;
	}

	const FRotator TargetRotation = (LockedTarget->GetActorLocation() - GetActorLocation()).Rotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(Controller->GetControlRotation(), TargetRotation, DeltaTime, 8.0f);
	Controller->SetControlRotation(FRotator(SmoothedRotation.Pitch, SmoothedRotation.Yaw, 0.0f));
}

void AWistoriaCharacter::DrawDebugResourceOverlay() const
{
	if (!bShowDebugResourceOverlay || !GEngine)
	{
		return;
	}

	const UEnum* CombatStyleEnum = StaticEnum<EWistoriaCombatStyle>();
	const FText StyleText = CombatStyleEnum
		? CombatStyleEnum->GetDisplayNameTextByValue(static_cast<int64>(CombatStyle))
		: FText::FromString(TEXT("Unknown"));

	const FString DebugText = FString::Printf(
		TEXT("Style: %s | HP: %.0f / %.0f | Mana: %.0f / %.0f | Courage: %.0f / %.0f | Level: %d | XP: %d"),
		*StyleText.ToString(),
		CurrentHealth,
		MaxHealth,
		CurrentMana,
		MaxMana,
		CurrentCourage,
		MaxCourage,
		CharacterLevel,
		CurrentXP);

	GEngine->AddOnScreenDebugMessage(DebugResourceOverlayKey, 0.0f, FColor::Cyan, DebugText);
}

void AWistoriaCharacter::ResetDodge()
{
	bCanDodge = true;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DodgeCooldownTimerHandle);
	}
}
