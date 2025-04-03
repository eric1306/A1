#include "A1GameplayAbility_Weapon_Gun_NormalShoot.h"

#include "A1GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
//#include "Actors/A1ProjectileBase.h"
#include "Actors/A1EquipmentBase.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_Weapon_Gun_NormalShoot)

UA1GameplayAbility_Weapon_Gun_NormalShoot::UA1GameplayAbility_Weapon_Gun_NormalShoot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(A1GameplayTags::Ability_Attack_Gun);
	ActivationOwnedTags.AddTag(A1GameplayTags::Status_Attack);
	ActivationRequiredTags.AddTag(A1GameplayTags::Status_ADS_Ready);
}

void UA1GameplayAbility_Weapon_Gun_NormalShoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
		// TODO  Jerry
		// Shoot();
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(A1GameplayTags::Status_ADS_Ready);
	UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(GetAvatarActorFromActorInfo(), TagContainer, true);

	//UAnimMontage* SelectedMontage = K2_CheckAbilityCost() ? ReleaseReloadMontage : ShootMontage;
	if (UAbilityTask_PlayMontageAndWait* ShootMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ShootMontage"), ShootMontage, GetSnapshottedAttackRate(), NAME_None, true, 1.f, 0.f, false))
	{
		ShootMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShootMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShootMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShootMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		ShootMontageTask->ReadyForActivation();
	}
}

void UA1GameplayAbility_Weapon_Gun_NormalShoot::OnMontageFinished()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEventData Payload;
		ASC->HandleGameplayEvent(A1GameplayTags::GameplayEvent_Gun_ADS, &Payload);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
