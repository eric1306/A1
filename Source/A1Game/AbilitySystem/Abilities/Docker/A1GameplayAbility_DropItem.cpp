// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "AbilitySystem/Abilities/Docker/A1GameplayAbility_DropItem.h"

#include "A1GameplayTags.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/A1EquipManagerComponent.h"
#include "Item/Managers/A1EquipmentManagerComponent.h"
#include "Item/Managers/A1ItemManagerComponent.h"
#include "Player/LyraPlayerController.h"

UA1GameplayAbility_DropItem::UA1GameplayAbility_DropItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = true;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ActivationGroup = ELyraAbilityActivationGroup::Independent;

	AbilityTags.AddTag(A1GameplayTags::Ability_DropItem);
	ActivationBlockedTags.AddTag(A1GameplayTags::Status_TryInteract);
}

void UA1GameplayAbility_DropItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();
	if (LyraPlayerController == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	UA1ItemManagerComponent* ItemManager = LyraPlayerController->FindComponentByClass<UA1ItemManagerComponent>();
	if (ItemManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	ItemManager->Server_DropItem(false);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
