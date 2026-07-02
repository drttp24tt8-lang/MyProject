#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WistoriaTitlePlayerController.generated.h"

class UUserWidget;

UCLASS()
class MYPROJECT_API AWistoriaTitlePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Title")
	void ShowSaveSlotMenu();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Title")
	void HideSaveSlotMenu();

	UFUNCTION(BlueprintCallable, Category = "Wistoria|Title")
	void QuitGame();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wistoria|Title")
	TSubclassOf<UUserWidget> SaveSlotWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Wistoria|Title")
	TObjectPtr<UUserWidget> SaveSlotWidget;
};
