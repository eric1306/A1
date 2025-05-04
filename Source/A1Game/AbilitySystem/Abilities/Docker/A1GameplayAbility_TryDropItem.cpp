// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "AbilitySystem/Abilities/Docker/A1GameplayAbility_TryDropItem.h"

#include "A1GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Physics/LyraCollisionChannels.h"
#include "Player/LyraPlayerController.h"
#include "Character/LyraCharacter.h"
#include "Character/A1CreatureBase.h"
#include "Item/Managers/A1ItemManagerComponent.h"
#include "Actors/A1EquipmentBase.h"

UA1GameplayAbility_TryDropItem::UA1GameplayAbility_TryDropItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UA1GameplayAbility_TryDropItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	if (K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		TryDrop();
	}
}

void UA1GameplayAbility_TryDropItem::TryDrop()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;

	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
	if (LyraPlayerController)
	{

		// 인벤토리에서 아이템 instance 제거




		// 제거된 아이템 바닥에 스폰
		UA1ItemManagerComponent* ItemManager = LyraPlayerController->GetComponentByClass<UA1ItemManagerComponent>();
		ItemManager->TryDropItem(nullptr, 0);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
