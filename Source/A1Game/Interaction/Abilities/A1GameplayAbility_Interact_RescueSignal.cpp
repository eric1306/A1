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
	//이미 맵이 활성화 되어있는 경우
	if (Spaceship->GetIsExternalMapActive())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// 액터에 접근
	AActor* OwningActor = ActorInfo->AvatarActor.Get();
	if (OwningActor)
	{
		// 월드에서 대상 액터 찾기
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1RandomMapGenerator::StaticClass(), FoundActors);

		for (AActor* Actor : FoundActors)
		{
			// 원하는 액터를 찾아 함수 호출
			AA1RandomMapGenerator* TargetActor = Cast<AA1RandomMapGenerator>(Actor);
			if (TargetActor)
			{
				//서버니까 맵 생성 시작.
				TargetActor->StartRandomMap();
				break;
			}
		}
		//Spaceship의 외부맵 활성화 여부 true 전환.
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
