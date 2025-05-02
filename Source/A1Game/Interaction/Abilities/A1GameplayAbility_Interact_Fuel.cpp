// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Interaction/Abilities/A1GameplayAbility_Interact_Fuel.h"

#include "Actors/A1FuelBase.h"

UA1GameplayAbility_Interact_Fuel::UA1GameplayAbility_Interact_Fuel(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UA1GameplayAbility_Interact_Fuel::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

    AA1FuelBase* FuelActor = Cast<AA1FuelBase>(InteractableActor);
    if (FuelActor == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    // ���� ���� Ȱ��ȭ - ��κ��� ��ȣ�ۿ��� �ڵ����� �̷�����Ƿ� ���⼭�� Ư���� �۾� ����
    // �ʿ��ϴٸ� �������� ���� ���� Ʈ���� ����
    FuelActor->DetectAndAbsorbFuelItems();

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}
