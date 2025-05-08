// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "AbilitySystem/Executions/A1HungerExecution.h"
#include "AbilitySystem/Attributes/A1CharacterAttributeSet.h"
#include "GameplayEffectTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1HungerExecution)

struct FHungerStatics
{
    FGameplayEffectAttributeCaptureDefinition HungerDef;

    FHungerStatics()
    {
        HungerDef = FGameplayEffectAttributeCaptureDefinition(UA1CharacterAttributeSet::GetHungerAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
    }
};

static FHungerStatics& HungerStatics()
{
    static FHungerStatics Statics;
    return Statics;
}

UA1HungerExecution::UA1HungerExecution()
{
    RelevantAttributesToCapture.Add(HungerStatics().HungerDef);
}

void UA1HungerExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;

    float CurrentHunger = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HungerStatics().HungerDef, EvaluateParameters, CurrentHunger);

    // 5초마다 Hunger 감소
    OutExecutionOutput.AddOutputModifier(
        FGameplayModifierEvaluatedData(UA1CharacterAttributeSet::GetHungerAttribute(), EGameplayModOp::Additive, -1.f));

    // Hunger가 120 이하면 weight 증가
    if (CurrentHunger <= 120.0f)
    {
        OutExecutionOutput.AddOutputModifier(
            FGameplayModifierEvaluatedData(UA1CharacterAttributeSet::GetWeightAttribute(), EGameplayModOp::Additive, 10.f));
    }

    // Hunger가 0 이하이면 health 감소
    if (CurrentHunger <= 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(
            FGameplayModifierEvaluatedData(UA1CharacterAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -10.f));
    }

#endif // #if WITH_SERVER_CODE
}
