#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WistoriaPlayerController.generated.h"

class UUserWidget;

UCLASS()
class MYPROJECT_API AWistoriaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|UI")
	void ShowHUD();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|UI")
	void HideHUD();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|UI")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|UI")
	void ShowPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|UI")
	void HidePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|UI")
	void ToggleCompanionCommandWheel();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|UI")
	void ShowCompanionCommandWheel();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|UI")
	void HideCompanionCommandWheel();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|UI")
	TSubclassOf<UUserWidget> CompanionCommandWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|UI")
	TObjectPtr<UUserWidget> HUDWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|UI")
	TObjectPtr<UUserWidget> PauseMenuWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|UI")
	TObjectPtr<UUserWidget> CompanionCommandWidget;
};
