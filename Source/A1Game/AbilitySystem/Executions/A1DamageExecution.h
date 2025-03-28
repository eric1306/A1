#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "A1DamageExecution.generated.h"

UCLASS()
class UA1DamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UA1DamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
