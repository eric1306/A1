// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "AbilitySystem/Attributes/A1CharacterAttributeSet.h"
#include "A1LogChannels.h"
#include "GameplayEffectExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1CharacterAttributeSet)

UA1CharacterAttributeSet::UA1CharacterAttributeSet() :
	MaxHealth(100.0f),
	MaxOxygen(100.f),
	MaxHunger(100.0f)
{
	InitHealth(GetMaxHealth());
	InitOxygen(GetMaxOxygen());
	InitHunger(50.f);

	bOutOfHealth = false;
}

void UA1CharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	NewValue = FMath::Clamp(NewValue, 0.0f, 100.f);
}

void UA1CharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		UE_LOG(LogA1, Log, TEXT("Health : %f -> %f"), OldValue, NewValue);

		// TODO Jerry
		// Replication 작업 시 이주
		const float CurrentHealth = GetHealth();
		const float EstimatedMagnitude = CurrentHealth /* - OldValue.GetCurrentValue() */ ;

		if (bOutOfHealth == false && CurrentHealth <= 0.0f)
		{
			OnOutOfHealth.Broadcast(0.f, GetHealth());
		}

		bOutOfHealth = (CurrentHealth <= 0.0f);
	}
	else if (Attribute == GetOxygenAttribute())
	{
		UE_LOG(LogA1, Log, TEXT("Oxygen : %f -> %f"), OldValue, NewValue);
	}
	else if (Attribute == GetHungerAttribute())
	{
		UE_LOG(LogA1, Log, TEXT("Hunger : %f -> %f"), OldValue, NewValue);
	}

}

void UA1CharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.f;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UE_LOG(LogA1, Warning, TEXT("Direct Health Access : %f"), GetHealth());
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		UE_LOG(LogA1, Log, TEXT("Damage : %f"), GetDamage());
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.f);
	}
}
