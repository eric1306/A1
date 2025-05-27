// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Interaction/Abilities/A1GameplayAbility_Interact_SignalDetection.h"

#include "Actors/A1SignalDetectionBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_Interact_SignalDetection)

UA1GameplayAbility_Interact_SignalDetection::UA1GameplayAbility_Interact_SignalDetection(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UA1GameplayAbility_Interact_SignalDetection::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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

    AA1SignalDetectionBase* SignalDetectionActor = Cast<AA1SignalDetectionBase>(InteractableActor);
    if (SignalDetectionActor == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }
    if (SignalDetectionActor->GetOwningSpaceship()->IsExternalMapActive())
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    if (SignalDetectionActor->GetSignalDetectionState() == ESignalDetectionState::None)
    {
        SignalDetectionActor->StartDetectSignal();
    }
    else
    {
        SignalDetectionActor->StopDetectSignal();
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
