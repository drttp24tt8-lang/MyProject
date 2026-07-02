#include "WistoriaPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/SWidget.h"

void AWistoriaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ShowHUD();
}

void AWistoriaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AWistoriaPlayerController::TogglePauseMenu);
		InputComponent->BindAction(TEXT("CommandWheel"), IE_Pressed, this, &AWistoriaPlayerController::ToggleCompanionCommandWheel);
	}
}

void AWistoriaPlayerController::ShowHUD()
{
	if (!HUDWidget && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
	}

	if (HUDWidget && !HUDWidget->IsInViewport())
	{
		HUDWidget->AddToViewport();
	}
}

void AWistoriaPlayerController::HideHUD()
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
	}
}

void AWistoriaPlayerController::TogglePauseMenu()
{
	if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
	{
		HidePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}
}

void AWistoriaPlayerController::ShowPauseMenu()
{
	if (!PauseMenuWidget && PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
	}

	if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->AddToViewport(10);
		SetPause(true);
		bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
		SetInputMode(InputMode);
	}
}

void AWistoriaPlayerController::HidePauseMenu()
{
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
	}

	SetPause(false);
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void AWistoriaPlayerController::ToggleCompanionCommandWheel()
{
	if (CompanionCommandWidget && CompanionCommandWidget->IsInViewport())
	{
		HideCompanionCommandWheel();
	}
	else
	{
		ShowCompanionCommandWheel();
	}
}

void AWistoriaPlayerController::ShowCompanionCommandWheel()
{
	if (!CompanionCommandWidget && CompanionCommandWidgetClass)
	{
		CompanionCommandWidget = CreateWidget<UUserWidget>(this, CompanionCommandWidgetClass);
	}

	if (CompanionCommandWidget && !CompanionCommandWidget->IsInViewport())
	{
		CompanionCommandWidget->AddToViewport(8);
		bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(CompanionCommandWidget->TakeWidget());
		SetInputMode(InputMode);
	}
}

void AWistoriaPlayerController::HideCompanionCommandWheel()
{
	if (CompanionCommandWidget)
	{
		CompanionCommandWidget->RemoveFromParent();
	}

	if (!PauseMenuWidget || !PauseMenuWidget->IsInViewport())
	{
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}
