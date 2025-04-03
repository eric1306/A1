#include "A1GameplayAbility_Weapon_Gun_NormalShoot.h"

#include "A1GameplayTags.h"
#include "Actors/A1EquipmentBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Physics/LyraCollisionChannels.h"
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
		Shoot();
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

void UA1GameplayAbility_Weapon_Gun_NormalShoot::Shoot()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;

	AA1EquipmentBase* WeaponActor = GetFirstEquipmentActor();
	if (WeaponActor == nullptr)
		return;

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo();

	if (LyraCharacter && LyraPlayerController)
	{
		FTransform SocketTransform = LyraCharacter->GetMesh()->GetSocketTransform(SpawnSocketName, RTS_World);
		FVector SocketLocation = SocketTransform.GetLocation();
		FRotator SocketRotation = SocketTransform.GetRotation().Rotator();

		FVector CameraLocation;
		FRotator CameraRotation;
		LyraPlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		//FTransform SpawnTransform;
		if (bApplyAimAssist)
		{
			float Distance = (SocketLocation - CameraLocation).Dot(CameraRotation.Vector());
			FVector StartLocation = CameraLocation + CameraRotation.Vector() * (Distance + AimAssistMinDistance);
			FVector EndLocation = StartLocation + (CameraRotation.Vector() * AimAssistMaxDistance);

			FHitResult HitResult;
			TArray<AActor*> ActorsToIgnore = { LyraCharacter, WeaponActor };

			bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(A1_TraceChannel_AimAssist), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);
			SocketRotation = bHit ? (HitResult.ImpactPoint - SocketLocation).Rotation() : (EndLocation - SocketLocation).Rotation();

			//SpawnTransform.SetLocation(SocketLocation);
			//SpawnTransform.SetRotation(SocketRotation.Quaternion());
		}
		else
		{
			//SpawnTransform.SetLocation(SocketLocation);
			//SpawnTransform.SetRotation(CameraRotation.Quaternion());
		}
	}
}
