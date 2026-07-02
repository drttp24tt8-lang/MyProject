#include "WistoriaTitlePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/SWidget.h"

void AWistoriaTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	ShowSaveSlotMenu();
}

void AWistoriaTitlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AWistoriaTitlePlayerController::QuitGame);
	}
}

void AWistoriaTitlePlayerController::ShowSaveSlotMenu()
{
	if (!SaveSlotWidget && SaveSlotWidgetClass)
	{
		SaveSlotWidget = CreateWidget<UUserWidget>(this, SaveSlotWidgetClass);
	}

	if (SaveSlotWidget && !SaveSlotWidget->IsInViewport())
	{
		SaveSlotWidget->AddToViewport();
		bShowMouseCursor = true;

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(SaveSlotWidget->TakeWidget());
		SetInputMode(InputMode);
	}
}

void AWistoriaTitlePlayerController::HideSaveSlotMenu()
{
	if (SaveSlotWidget)
	{
		SaveSlotWidget->RemoveFromParent();
	}

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void AWistoriaTitlePlayerController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}
