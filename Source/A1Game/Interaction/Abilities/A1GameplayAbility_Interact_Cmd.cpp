// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Interaction/Abilities/A1GameplayAbility_Interact_Cmd.h"

#include "A1GameplayTags.h"
#include "A1LogChannels.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Abilities/Docker/A1GameplayAbility_Cmd.h"
#include "Actors/A1CMDBase.h"
#include "UIExtensionSystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_Interact_Cmd)

UA1GameplayAbility_Interact_Cmd::UA1GameplayAbility_Interact_Cmd()
{
}

void UA1GameplayAbility_Interact_Cmd::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UE_LOG(LogTemp, Log, TEXT("Activate Ability!"));

	if (TriggerEventData == nullptr || bInitialized == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

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

void UA1GameplayAbility_Interact_Cmd::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UA1GameplayAbility_Interact_Cmd::OpenMap(FGameplayEventData Payload)
{
	// Map Material Set
	UE_LOG(LogA1System, Log, TEXT("OpenMap"));
	if (AA1CMDBase* CMD = Cast<AA1CMDBase>(InteractableActor))
	{
		TArray<UMeshComponent*> OutMesh;
		CMD->GetMeshComponents(OutMesh);
		if (OutMesh.Num() == 1)
		{
			if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(OutMesh[0]))
			{
				StaticMeshComponent->SetMaterial(1, MinimapMaterial);
			}
		}
	}
}

void UA1GameplayAbility_Interact_Cmd::CloseCmd(FGameplayEventData Payload)
{
	DeactiveWidget();
}
