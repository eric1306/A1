#pragma once

#include "A1InteractionQuery.generated.h"

USTRUCT(BlueprintType)
struct FA1InteractionQuery
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> RequestingAvatar;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AController> RequestingController;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> OptionalObjectData;
};