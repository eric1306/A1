// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "A1GameplayAbility_Utility_RemoveFoam.generated.h"

/**
 * 
 */
UCLASS()
class A1GAME_API UA1GameplayAbility_Utility_RemoveFoam : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	UA1GameplayAbility_Utility_RemoveFoam(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnInputReleased(float TimeHeld);
	void TryRemoveFoam();

private:
	FTimerHandle LoopHandle;

protected:
	UPROPERTY()
	UAudioComponent* LoopingAudioComponent;

	UPROPERTY(EditAnywhere)
	USoundBase* RemoveSound;
};
