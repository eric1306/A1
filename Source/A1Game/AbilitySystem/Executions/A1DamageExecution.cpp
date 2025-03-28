#include "A1DamageExecution.h"

#include "AbilitySystem/LyraAbilitySourceInterface.h"
#include "AbilitySystem/Attributes/A1VitalSet.h"
#include "AbilitySystem/Attributes/A1CombatSet.h"
#include "AbilitySystem/LyraGameplayEffectContext.h"
#include "Kismet/KismetMathLibrary.h"
#include "Teams/LyraTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1DamageExecution)

struct FDamageStatics
{
public:
	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UA1CombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}

public:
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UA1DamageExecution::UA1DamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
}

void UA1DamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FLyraGameplayEffectContext* TypedContext = FLyraGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
	check(TypedContext);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	const AActor* EffectCauser = TypedContext->GetEffectCauser();
	const FHitResult* HitActorResult = TypedContext->GetHitResult();
	AActor* HitActor = nullptr;
	
	if (HitActorResult)
	{
		const FHitResult& CurrHitResult = *HitActorResult;
		HitActor = CurrHitResult.HitObjectHandle.FetchActor();
	}

	// Apply rules for team damage
	float DamageInteractionAllowedMultiplier = 0.0f;
	if (HitActor)
	{
		ULyraTeamSubsystem* TeamSubsystem = HitActor->GetWorld()->GetSubsystem<ULyraTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			DamageInteractionAllowedMultiplier = TeamSubsystem->CanCauseDamage(EffectCauser, HitActor) ? 1.f : 0.f;
		}
	}
	
	// Weak point damage calculation
	float PhysicalMaterialAttenuation = 1.0f;
	if (const ILyraAbilitySourceInterface* AbilitySource = TypedContext->GetAbilitySource())
	{
		if (const UPhysicalMaterial* PhysicalMaterial = TypedContext->GetPhysicalMaterial())
		{
			PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysicalMaterial, SourceTags, TargetTags);
		}
	}
	
	float DamageDone = FMath::Max(BaseDamage * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier, 0.0f);
	
	if (DamageDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UA1VitalSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, DamageDone));

		if (UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent())
		{
			SourceASC->ApplyModToAttribute(UA1VitalSet::GetHealthAttribute(), EGameplayModOp::Additive, DamageDone);
		}
	}
#endif // WITH_SERVER_CODE
}
