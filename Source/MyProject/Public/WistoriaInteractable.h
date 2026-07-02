#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WistoriaInteractable.generated.h"

UINTERFACE(BlueprintType)
class MYPROJECT_API UWistoriaInteractable : public UInterface
{
	GENERATED_BODY()
};

class MYPROJECT_API IWistoriaInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* InteractingActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionText() const;
};
