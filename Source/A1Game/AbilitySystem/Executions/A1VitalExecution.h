#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "A1VitalExecution.generated.h"

UCLASS()
class UA1VitalExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UA1VitalExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
