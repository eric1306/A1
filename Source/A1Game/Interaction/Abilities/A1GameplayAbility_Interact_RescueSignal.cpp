// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Interaction/Abilities/A1GameplayAbility_Interact_RescueSignal.h"

#include "Actors/A1RescueSignalBase.h"
#include "Camera/LyraCameraMode_ThirdPerson.h"
#include "Camera/LyraPlayerCameraManager.h"
#include "Camera/LyraUICameraManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Maps/A1RandomMapGenerator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_Interact_RescueSignal)

UA1GameplayAbility_Interact_RescueSignal::UA1GameplayAbility_Interact_RescueSignal(
	const FObjectInitializer& ObjectInitializer)
{
	
}

void UA1GameplayAbility_Interact_RescueSignal::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	AA1RescueSignalBase* RescueSignalActor = Cast<AA1RescueSignalBase>(InteractableActor);
	if (RescueSignalActor == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	AA1SpaceshipBase* Spaceship = RescueSignalActor->GetOwningSpaceship();
	if (!IsValid(Spaceship))
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	//�̹� ���� Ȱ��ȭ �Ǿ��ִ� ���
	if (Spaceship->GetIsExternalMapActive())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// ���Ϳ� ����
	AActor* OwningActor = ActorInfo->AvatarActor.Get();
	if (OwningActor)
	{
		// ���忡�� ��� ���� ã��
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1RandomMapGenerator::StaticClass(), FoundActors);

		for (AActor* Actor : FoundActors)
		{
			// ���ϴ� ���͸� ã�� �Լ� ȣ��
			AA1RandomMapGenerator* TargetActor = Cast<AA1RandomMapGenerator>(Actor);
			if (TargetActor)
			{
				//�����ϱ� �� ���� ����.
				TargetActor->StartRandomMap();
				break;
			}
		}
		//Spaceship�� �ܺθ� Ȱ��ȭ ���� true ��ȯ.
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1SpaceshipBase::StaticClass(), FoundActors);
		for (AActor* Actor : FoundActors)
		{
			if (AA1SpaceshipBase* SpaceshipActor = Cast<AA1SpaceshipBase>(Actor))
			{
				SpaceshipActor->SetIsExternamMapActive(true);
			}

		}
	}


	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
