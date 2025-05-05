// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Docker/A1GameplayAbility_Equipment.h"
#include "A1GameplayAbility_Utility_Food.generated.h"

/**
 * 
 */
UCLASS()
class A1GAME_API UA1GameplayAbility_Utility_Food : public UA1GameplayAbility_Equipment
{
	GENERATED_BODY()

public:
	UA1GameplayAbility_Utility_Food(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	
};
