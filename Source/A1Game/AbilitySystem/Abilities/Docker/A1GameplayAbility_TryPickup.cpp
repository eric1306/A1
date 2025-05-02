// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "AbilitySystem/Abilities/Docker/A1GameplayAbility_TryPickup.h"

#include "A1GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Physics/LyraCollisionChannels.h"
#include "Player/LyraPlayerController.h"
#include "Character/LyraCharacter.h"
#include "Character/A1CreatureBase.h"
#include "Item/Managers/A1ItemManagerComponent.h"
#include "Actors/A1PickupableItemBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_TryPickup)

UA1GameplayAbility_TryPickup::UA1GameplayAbility_TryPickup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UA1GameplayAbility_TryPickup::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
		TryPickup();
	}
}

void UA1GameplayAbility_TryPickup::TryPickup()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();

	if (LyraCharacter && LyraPlayerController)
	{

		FVector CameraLocation;
		FRotator CameraRotation;
		LyraPlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector StartLocation = CameraLocation;
		FVector EndLocation = StartLocation + (CameraRotation.Vector() * MaxDistance);

		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore = { LyraCharacter };

		bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(A1_TraceChannel_AimAssist), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);

		if (bHit)
		{
			AA1PickupableItemBase* Target = Cast<AA1PickupableItemBase>(HitResult.GetActor());
			if (Target)
			{
				UA1ItemManagerComponent* ItemManager = LyraCharacter->GetComponentByClass<UA1ItemManagerComponent>();
				ItemManager->TryPickItem(Target);
			}
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
