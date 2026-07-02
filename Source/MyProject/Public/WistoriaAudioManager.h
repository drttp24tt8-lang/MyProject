#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WistoriaAudioManager.generated.h"

class UAudioComponent;
class USoundBase;

UCLASS()
class MYPROJECT_API AWistoriaAudioManager : public AActor
{
	GENERATED_BODY()

public:
	AWistoriaAudioManager();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetCombatIntensity(float NewCombatIntensity);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayMusic(USoundBase* MusicAsset);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> MusicComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FName CombatIntensityParameterName = TEXT("CombatIntensity");
};
