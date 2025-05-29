// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "A1GameplayAbility_Cmd.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class A1GAME_API UA1GameplayAbility_Cmd : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	UA1GameplayAbility_Cmd(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ActiveWidget();
	UFUNCTION(BlueprintImplementableEvent)
	void DeactiveWidget();

protected:
	UFUNCTION()
	void OpenMap(FGameplayEventData Payload);

	UFUNCTION()
	void CloseCmd(FGameplayEventData Payload);

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> OpenMapTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ExitWidgetTask;
};
