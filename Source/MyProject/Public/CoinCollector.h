#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoinCollector.generated.h"

UINTERFACE(BlueprintType)
class MYPROJECT_API UCoinCollector : public UInterface
{
	GENERATED_BODY()
};

class MYPROJECT_API ICoinCollector
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Score")
	void AddCoins(int32 Amount);
};
