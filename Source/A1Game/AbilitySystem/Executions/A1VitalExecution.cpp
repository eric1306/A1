#include "A1VitalExecution.h"

#include "AbilitySystem/Attributes/A1VitalSet.h"
#include "AbilitySystem/Attributes/A1CombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1VitalExecution)

struct FVitalStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealthDef;
	FGameplayEffectAttributeCaptureDefinition BaseOxygenDef;

	FVitalStatics()
	{
		BaseHealthDef = FGameplayEffectAttributeCaptureDefinition(UA1CombatSet::GetBaseHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseOxygenDef = FGameplayEffectAttributeCaptureDefinition(UA1CombatSet::GetBaseOxygenAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FVitalStatics& HealStatics()
{
	static FVitalStatics Statics;
	return Statics;
}

UA1VitalExecution::UA1VitalExecution()
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealthDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseOxygenDef);
}

void UA1VitalExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealthDef, EvaluateParameters, BaseHealth);

	float BaseOxygen = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseOxygenDef, EvaluateParameters, BaseOxygen);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UA1VitalSet::GetIncomingHealthAttribute(), EGameplayModOp::Additive, BaseHealth));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UA1VitalSet::GetIncomingOxygenAttribute(), EGameplayModOp::Additive, BaseOxygen));
	
#endif // #if WITH_SERVER_CODE
}
