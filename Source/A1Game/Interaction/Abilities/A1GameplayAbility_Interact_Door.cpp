// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Interaction/Abilities/A1GameplayAbility_Interact_Door.h"
#include "Actors/A1DoorBase.h"
#include "Kismet/KismetMathLibrary.h"

UA1GameplayAbility_Interact_Door::UA1GameplayAbility_Interact_Door(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UA1GameplayAbility_Interact_Door::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	AA1DoorBase* DoorActor = Cast<AA1DoorBase>(InteractableActor);
	if (DoorActor == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	//�ܺ� ���� Ȱ��ȭ ���� ���� ���� ��ȣ�ۿ� �� �� �ְ� ����.
	if (AA1SpaceshipBase* Spaceship = DoorActor->GetOwningSpaceship())
	{
		if (!Spaceship->GetIsExternalMapActive())
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}
	}

	EDoorState CurrentDoorState = DoorActor->GetDoorState();
	if (CurrentDoorState == EDoorState::Open)
	{
		DoorActor->SetDoorState(EDoorState::Close);
	}
	else
	{
		DoorActor->SetDoorState(EDoorState::Open);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
