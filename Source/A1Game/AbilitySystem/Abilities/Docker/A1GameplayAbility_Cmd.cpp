// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1GameplayAbility_Cmd.h"

#include "A1GameplayTags.h"
#include "A1LogChannels.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "A1GameplayAbility_Cmd.h"

UA1GameplayAbility_Cmd::UA1GameplayAbility_Cmd(const FObjectInitializer& ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = true;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ActivationGroup = ELyraAbilityActivationGroup::Independent;
}

void UA1GameplayAbility_Cmd::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ExitWidgetTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, A1GameplayTags::GameplayEvent_Cmd_Close, nullptr, false, true);
	if (ExitWidgetTask)
	{
		ExitWidgetTask->EventReceived.AddDynamic(this, &ThisClass::CloseCmd);
		ExitWidgetTask->ReadyForActivation();
	}

	OpenMapTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, A1GameplayTags::GameplayEvent_Cmd_Map, nullptr, false, true);
	if (OpenMapTask)
	{
		OpenMapTask->EventReceived.AddDynamic(this, &ThisClass::OpenMap);
		OpenMapTask->ReadyForActivation();
	}
}

void UA1GameplayAbility_Cmd::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UA1GameplayAbility_Cmd::CloseCmd(FGameplayEventData Payload)
{
	DeactiveWidget();
}

void UA1GameplayAbility_Cmd::OpenMap(FGameplayEventData Payload)
{
	// Map Material Set
	UE_LOG(LogA1System, Log, TEXT("OpenMap"));
}
