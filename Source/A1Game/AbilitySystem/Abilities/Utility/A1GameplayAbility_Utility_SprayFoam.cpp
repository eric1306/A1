// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "AbilitySystem/Abilities/Utility/A1GameplayAbility_Utility_SprayFoam.h"

#include "A1GameplayTags.h"
#include "A1LogChannels.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Character/LyraCharacter.h"
#include "Player/LyraPlayerController.h"
#include "Physics/LyraCollisionChannels.h"
#include "System/LyraAssetManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_Utility_SprayFoam)

UA1GameplayAbility_Utility_SprayFoam::UA1GameplayAbility_Utility_SprayFoam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = false;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
	ActivationRequiredTags.AddTag(A1GameplayTags::Status_MainHand_Left);
}

void UA1GameplayAbility_Utility_SprayFoam::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &UA1GameplayAbility_Utility_SprayFoam::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}

	if (SpraySound)
	{
		LoopingAudioComponent = UGameplayStatics::SpawnSoundAttached(
			SpraySound,
			GetAvatarActorFromActorInfo()->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			true  // Looping
		);

		if (LoopingAudioComponent)
		{
			LoopingAudioComponent->bIsUISound = false;
		}
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		GetWorld()->GetTimerManager().SetTimer(LoopHandle, this, &UA1GameplayAbility_Utility_SprayFoam::TrySprayFoam, 0.1f, true);
	}
}

void UA1GameplayAbility_Utility_SprayFoam::OnInputReleased(float TimeHeld)
{
	GetWorld()->GetTimerManager().ClearTimer(LoopHandle);
	if (LoopingAudioComponent)
	{
		LoopingAudioComponent->Stop();
		LoopingAudioComponent = nullptr;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UA1GameplayAbility_Utility_SprayFoam::TrySprayFoam()
{
	ALyraPlayerController* Controller = GetLyraPlayerControllerFromActorInfo();
	ALyraCharacter* Character = GetLyraCharacterFromActorInfo();
	if (Character == nullptr || Controller == nullptr)
		return;

	const float MaxDistance = 1000.f;
	const int32 MaxTryCount = 5.f;
	TArray<AActor*> ActorsToIgnore = { Character };

	FHitResult HitResult;
	FVector2D RandPoint = FMath::RandPointInCircle(MaxDistance);

	FVector CameraLocation;
	FRotator CameraRotation;
	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector StartLocation = CameraLocation;
	FVector EndLocation = StartLocation + (CameraRotation.Vector() * MaxDistance);


	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation, UEngineTypes::ConvertToTraceType(A1_TraceChannel_AimAssist), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);

	TSubclassOf<AActor> FoamClass = ULyraAssetManager::Get().GetSubclassByName<AActor>("FoamBase");;


	if(bHit)
	{
		// ���� ����� �� ������ ���
		if (FoamClass == HitResult.GetActor()->GetClass())
		{
			float rate = HitResult.GetActor()->GetActorScale3D().X;
			if(rate <=0.5)
				HitResult.GetActor()->SetActorScale3D(HitResult.GetActor()->GetActorScale3D() * 1.2f);
		}
		else
		{
			// ���� ����
			FVector SpawnLocation = HitResult.Location + HitResult.Normal * 3.f;
			FRotator SpawnRotation = FRotationMatrix::MakeFromZ(HitResult.Normal).Rotator();

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
			AActor* SpawnedFoam = GetWorld()->SpawnActor<AActor>(FoamClass, SpawnLocation, SpawnRotation, SpawnParameters);


			// skeletal mesh ����� ���
			USkeletalMeshComponent* MeshComp = HitResult.GetActor()->FindComponentByClass<USkeletalMeshComponent>();
			if (MeshComp)
			{
				// 1. �� �߿��� HitResult.Location�� ���� ����� �� ã��
				FName ClosestBoneName = MeshComp->FindClosestBone(HitResult.Location);
				if (ClosestBoneName == NAME_None)
					return;

				// 2. ��� ��ġ ���
				FVector BoneWorldLocation = MeshComp->GetBoneLocation(ClosestBoneName);
				FVector RelativeOffset = HitResult.Location - BoneWorldLocation;

				// 3. ���̱�: ��� ��ġ�� �����ϸ� �ش� ���� ���̱�
				if (SpawnedFoam)
				{
					SpawnedFoam->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepRelativeTransform, ClosestBoneName);
					SpawnedFoam->SetActorRelativeLocation(RelativeOffset); // �� ���� ��� ��ġ�� ������
				}
			}

			// Static Mesh�� ���
			else               
			{
				if (HitResult.GetComponent() != nullptr)
				{
					if(SpawnedFoam)
					{
						SpawnedFoam->AttachToComponent(HitResult.GetComponent(), FAttachmentTransformRules::KeepWorldTransform);
					}
				}
			}
		}
	}
}