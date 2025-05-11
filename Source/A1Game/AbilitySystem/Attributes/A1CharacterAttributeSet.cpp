// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "AbilitySystem/Attributes/A1CharacterAttributeSet.h"
#include "A1LogChannels.h"
#include "GameplayEffectExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1CharacterAttributeSet)

UA1CharacterAttributeSet::UA1CharacterAttributeSet() :
	MaxHealth(120.0f),
	MaxOxygen(500.0f),
	MaxHunger(720.0f),
	MaxWeight(100.0f)
{
	InitHealth(GetMaxHealth());
	InitOxygen(GetMaxOxygen());
	InitHunger(GetMaxHunger());
	InitWeight(0.f);

	bOutOfHealth = false;
}

void UA1CharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetWeightAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxWeight());
	}
	else if (Attribute == GetOxygenAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxOxygen());
	}
	else if (Attribute == GetHungerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHunger());
	}
}

void UA1CharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	// TODO Jerry
	// Replication �۾� �� ����
	if (Attribute == GetHealthAttribute())
	{
		const float CurrentHealth = GetHealth();
		UE_LOG(LogA1Player, Log, TEXT("Health : %f -> %f"), OldValue, NewValue);

		if (bOutOfHealth == false && CurrentHealth <= 0.0f)
			OnOutOfHealth.Broadcast(nullptr, OldValue, NewValue);

		bOutOfHealth = (CurrentHealth <= 0.0f);
	}
	else if (Attribute == GetWeightAttribute())
	{
		UE_LOG(LogA1Player, Log, TEXT("Weight : %f -> %f"), OldValue, NewValue);
	}
	else if (Attribute == GetOxygenAttribute())
	{
		UE_LOG(LogA1Player, Log, TEXT("Oxygen : %f -> %f"), OldValue, NewValue);
	}
	else if (Attribute == GetHungerAttribute())
	{
		UE_LOG(LogA1Player, Log, TEXT("Hunger : %f -> %f"), OldValue, NewValue);
	}		
}

void UA1CharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.f;
	AActor* Instigator = Data.EffectSpec.GetContext().GetInstigator();
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UE_LOG(LogA1Player, Warning, TEXT("Direct Health Access : %f"), GetHealth());
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		OnHealthChanged.Broadcast(Instigator, GetHealth(), GetHealth() - GetDamage());
		
		UE_LOG(LogA1Player, Log, TEXT("Damage : %f"), GetDamage());
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.f);
	}
}
