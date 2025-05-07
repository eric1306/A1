// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1AnimNotifyState_PerformTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/A1EquipmentBase.h"
#include "Components/BoxComponent.h"
#include "Character/LyraCharacter.h"
#include "Development/A1DeveloperSettings.h"
#include "Item/Managers/A1EquipManagerComponent.h"
#include "KismetTraceUtils.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1AnimNotifyState_PerformTrace)

UA1AnimNotifyState_PerformTrace::UA1AnimNotifyState_PerformTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UA1AnimNotifyState_PerformTrace::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;

	if (ALyraCharacter* Character = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UA1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UA1EquipManagerComponent>())
		{
			WeaponActor = EquipManager->GetEquippedActor(WeaponHandType);
			if (WeaponActor.IsValid() == false)
				return;

			PreviousTraceTransform = WeaponActor->MeshComponent->GetComponentTransform();
			PreviousDebugTransform = WeaponActor->TraceDebugCollision->GetComponentTransform();
			PreviousSocketTransform = WeaponActor->MeshComponent->GetSocketTransform(TraceSocketName);

#if UE_EDITOR
			check(WeaponActor->MeshComponent->DoesSocketExist(TraceSocketName));
#endif
		}
	}

	HitActors.Empty();
}

void UA1AnimNotifyState_PerformTrace::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;

	if (WeaponActor.IsValid() == false)
		return;

	PerformTrace(MeshComponent);
}

void UA1AnimNotifyState_PerformTrace::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComponent, Animation, EventReference);

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;

	if (WeaponActor.IsValid() == false)
		return;

	PerformTrace(MeshComponent);
}

void UA1AnimNotifyState_PerformTrace::PerformTrace(USkeletalMeshComponent* MeshComponent)
{
	FTransform CurrentSocketTransform = WeaponActor->MeshComponent->GetSocketTransform(TraceSocketName);
	float Distance = (PreviousSocketTransform.GetLocation() - CurrentSocketTransform.GetLocation()).Length();

	int SubStepCount = FMath::CeilToInt(Distance / TargetDistance);
	if (SubStepCount <= 0)
		return;

	float SubstepRatio = 1.f / SubStepCount;

	const FTransform& CurrentTraceTransform = WeaponActor->MeshComponent->GetComponentTransform();
	const FTransform& CurrentDebugTransform = WeaponActor->TraceDebugCollision->GetComponentTransform();

	TArray<FHitResult> FinalHitResults;

	for (int32 i = 0; i < SubStepCount; i++)
	{
		FTransform StartTraceTransform = UKismetMathLibrary::TLerp(PreviousTraceTransform, CurrentTraceTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
		FTransform EndTraceTransform = UKismetMathLibrary::TLerp(PreviousTraceTransform, CurrentTraceTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
		FTransform AverageTraceTransform = UKismetMathLibrary::TLerp(StartTraceTransform, EndTraceTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);

		FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
		Params.bReturnPhysicalMaterial = true;

		TArray<AActor*> IgnoredActors = { WeaponActor.Get(), WeaponActor->GetOwner() };
		Params.AddIgnoredActors(IgnoredActors);

		TArray<FHitResult> HitResults;
		MeshComponent->GetWorld()->ComponentSweepMulti(HitResults, WeaponActor->MeshComponent, StartTraceTransform.GetLocation(), EndTraceTransform.GetLocation(), AverageTraceTransform.GetRotation(), Params);

		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActors.Contains(HitActor) == false)
			{
				HitActors.Add(HitActor);
				FinalHitResults.Add(HitResult);
			}
		}

#if UE_EDITOR
		if (GIsEditor)
		{
			const UA1DeveloperSettings* DeveloperSettings = GetDefault<UA1DeveloperSettings>();
			if (DeveloperSettings->bForceDisableDebugTrace == false && bDrawDebugShape)
			{
				FColor Color = (HitResults.Num() > 0) ? HitColor : TraceColor;

				FTransform StartDebugTransform = UKismetMathLibrary::TLerp(PreviousDebugTransform, CurrentDebugTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
				FTransform EndDebugTransform = UKismetMathLibrary::TLerp(PreviousDebugTransform, CurrentDebugTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
				FTransform AverageDebugTransform = UKismetMathLibrary::TLerp(StartDebugTransform, EndDebugTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);

				DrawDebugSweptBox(MeshComponent->GetWorld(), StartDebugTransform.GetLocation(), EndDebugTransform.GetLocation(), AverageDebugTransform.GetRotation().Rotator(), WeaponActor->TraceDebugCollision->GetScaledBoxExtent(), Color, false, 2.f);
			}
		}
#endif
	}

	PreviousTraceTransform = CurrentTraceTransform;
	PreviousDebugTransform = CurrentDebugTransform;
	PreviousSocketTransform = CurrentSocketTransform;

	if (FinalHitResults.Num() > 0)
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle;

		for (const FHitResult& HitResult : FinalHitResults)
		{
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = HitResult;
			TargetDataHandle.Add(NewTargetData);
		}

		FGameplayEventData EventData;
		EventData.TargetData = TargetDataHandle;
		EventData.Instigator = WeaponActor.Get();

		if (EventTag.IsValid())
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComponent->GetOwner(), EventTag, EventData);
		}
	}
}
