#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "A1AttributeSet.generated.h"

class ULyraAbilitySystemComponent;
struct FGameplayEffectSpec;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)				\
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)				\
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)				\
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_SixParams(FLyraAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FAttributeEvent, float /*OldValue*/, float /*NewValue*/);

UCLASS()
class A1GAME_API UA1AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UA1AttributeSet();

public:
	UWorld* GetWorld() const override;
	ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const;
};
