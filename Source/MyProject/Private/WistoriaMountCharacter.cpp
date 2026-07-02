#include "WistoriaMountCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WistoriaCharacter.h"

AWistoriaMountCharacter::AWistoriaMountCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCharacterMovement()->MaxWalkSpeed = MountedWalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AWistoriaMountCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AWistoriaMountCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AWistoriaMountCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AWistoriaMountCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AWistoriaMountCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Mount"), IE_Pressed, this, &AWistoriaMountCharacter::HandleDismountInput);
}

bool AWistoriaMountCharacter::MountRider(AWistoriaCharacter* NewRider, FName SeatSocketName)
{
	if (!NewRider || Rider)
	{
		return false;
	}

	Rider = NewRider;
	RiderController = NewRider->GetController();
	if (!RiderController)
	{
		Rider = nullptr;
		return false;
	}

	NewRider->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewRider->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SeatSocketName);
	RiderController->Possess(this);
	OnRiderMounted(NewRider);
	return true;
}

bool AWistoriaMountCharacter::DismountRider()
{
	if (!Rider || !RiderController)
	{
		return false;
	}

	AWistoriaCharacter* DismountedRider = Rider;
	const FVector DismountLocation = GetActorLocation() + GetActorRotation().RotateVector(DismountOffset);
	const FRotator DismountRotation = GetActorRotation();

	DismountedRider->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	DismountedRider->SetActorLocationAndRotation(DismountLocation, DismountRotation, false, nullptr, ETeleportType::TeleportPhysics);
	DismountedRider->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RiderController->Possess(DismountedRider);

	OnRiderDismounted(DismountedRider);
	Rider = nullptr;
	RiderController = nullptr;
	Destroy();
	return true;
}

AWistoriaCharacter* AWistoriaMountCharacter::GetRider() const
{
	return Rider;
}

void AWistoriaMountCharacter::MoveForward(float Value)
{
	if (Controller && !FMath::IsNearlyZero(Value))
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
	}
}

void AWistoriaMountCharacter::MoveRight(float Value)
{
	if (Controller && !FMath::IsNearlyZero(Value))
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
	}
}

void AWistoriaMountCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AWistoriaMountCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AWistoriaMountCharacter::HandleDismountInput()
{
	if (Rider)
	{
		Rider->Dismount();
	}
	else
	{
		DismountRider();
	}
}
