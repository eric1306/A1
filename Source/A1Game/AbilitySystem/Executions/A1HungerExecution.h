// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "A1HungerExecution.generated.h"

UCLASS()
class A1GAME_API UA1HungerExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
    UA1HungerExecution();

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
