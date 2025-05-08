// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1RaiderBase.h"

#include "A1LogChannels.h"
#include "Controller/Raider/A1RaiderController.h"
#include "AbilitySystem/Attributes/A1CharacterAttributeSet.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"
#include "Actors/A1EquipmentBase.h"
#include "Actors/A1PickupableItemBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1RaiderBase)

// Sets default values
AA1RaiderBase::AA1RaiderBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AA1RaiderController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	HealthSet = CreateDefaultSubobject<UA1CharacterAttributeSet>(TEXT("AttributeSet"));

	// Register to listen for attribute changes.
	HealthSet->OnHealthChanged.AddUObject(this, &AA1RaiderBase::BeAttacked);
	HealthSet->OnOutOfHealth.AddUObject(this, &AA1RaiderBase::HandleOutOfHealth);
}

// Called when the game starts or when spawned
void AA1RaiderBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogA1Raider, Log, TEXT("RaiderBase: Beginplay Call"));

	int i = 0;
	for (auto Ability : Abilities)
	{
		FGameplayAbilitySpec StartSpec(Ability);
		StartSpec.InputID = i++;
		ASC->GiveAbility(StartSpec);
	}
}

// Called every frame
void AA1RaiderBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AA1RaiderBase::BeAttacked(AActor* InInstigator, float OldValue, float NewValue)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBlackboardComponent())
	{
		UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent();
		if (BlackBoard->GetValueAsBool(AA1RaiderController::CanAttackKey) == false)
		{
			BlackBoard->SetValueAsBool(AA1RaiderController::CanAttackKey, true);
			BlackBoard->SetValueAsObject(AA1RaiderController::AggroTargetKey, InInstigator);
		}
	}
}

void AA1RaiderBase::HandleOutOfHealth(AActor* InActor, float OldValue, float NewValue)
{
	
	// 애니메이션이 처리될 시간 기다리기
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AA1RaiderBase::DestroyDueToDeath, 1.5f, false);


	GetController()->UnPossess();  // AI가 더 이상 캐릭터를 제어하지 않도록 함

	SetDead();
	//GetMesh()->SetAnimInstanceClass(nullptr);
	//
	//if (DeadMontage)
	//{
	//	GetMesh()->PlayAnimation(DeadMontage, false);
	//}
}

void AA1RaiderBase::DestroyDueToDeath()
{
	// 천천히 사라지는 효과(연구중)
	//int32 NumMaterials = GetMesh()->GetNumMaterials();
	//TArray<UMaterialInstanceDynamic*> DynamicMats;
	//
	//for (int32 i = 0; i < NumMaterials; ++i)
	//{
	//	UMaterialInstanceDynamic* DynMat = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
	//	if (DynMat)
	//		DynamicMats.Add(DynMat);
	//}
	//
	//if (DynamicMats.Num() == 0) return;
	//
	//float FadeDuration = 1.5f;
	//float TickInterval = 0.05f;
	//int32 TotalSteps = FadeDuration / TickInterval;
	//float FadeStep = 1.0f / TotalSteps;
	//
	//float* CurrentFade = new float(1.0f);
	//
	//FTimerDelegate FadeDelegate;
	//
	//FadeDelegate = FTimerDelegate::CreateLambda([=, this]() mutable
	//	{
	//		*CurrentFade -= FadeStep;
	//		float FadeValue = FMath::Clamp(*CurrentFade, 0.0f, 1.0f);
	//
	//		for (UMaterialInstanceDynamic* Mat : DynamicMats)
	//		{
	//			if (Mat)
	//				Mat->SetScalarParameterValue(FName("Opacity"), FadeValue);
	//		}
	//
	//		if (FadeValue <= 0.0f)
	//		{
	//			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	//			delete CurrentFade;				// 할당 해제
	//			this->Destroy();
	//		}
	//	});
	//
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, FadeDelegate, TickInterval, true);


	// 아이템 스폰
	int ItemNum = dropItems.Num();

	FVector ItemSpawnLocation = GetActorLocation();
	ItemSpawnLocation.Z = 0;

	//확률에 따라 아이템 설정(일단 무조건 스폰)
	//확률 정해진게 없어 하드코딩
	int32 index = FMath::RandRange(0, 100);
	if (index >= 50)
		GetWorld()->SpawnActor<AA1EquipmentBase>(dropItems[ItemNum-1], ItemSpawnLocation, GetActorRotation());
	else
		GetWorld()->SpawnActor<AA1EquipmentBase>(dropItems[0], ItemSpawnLocation, GetActorRotation());

}

//void AA1RaiderBase::Server_PerformTrace(USkeletalMeshComponent* MeshComponent)
//{
//	FTransform CurrentSocketTransform = MeshComponent->GetSocketTransform(TraceSocketName);
//	float Distance = (PreviousSocketTransform.GetLocation() - CurrentSocketTransform.GetLocation()).Length();
//
//	int SubStepCount = FMath::CeilToInt(Distance / TargetDistance);
//	if (SubStepCount <= 0)
//		return;
//
//	float SubstepRatio = 1.f / SubStepCount;
//
//	TArray<FHitResult> FinalHitResults;
//
//	for (int32 i = 0; i < SubStepCount; i++)
//	{
//		FTransform StartTraceTransform = UKismetMathLibrary::TLerp(PreviousSocketTransform, CurrentSocketTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
//		FTransform EndTraceTransform = UKismetMathLibrary::TLerp(PreviousSocketTransform, CurrentSocketTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
//		FTransform AverageTraceTransform = UKismetMathLibrary::TLerp(StartTraceTransform, EndTraceTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);
//
//		FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
//		Params.bReturnPhysicalMaterial = true;
//
//		TArray<AActor*> IgnoredActors = { MeshComponent->GetOwner() };
//		Params.AddIgnoredActors(IgnoredActors);
//
//		TArray<FHitResult> HitResults;
//
//		bool bHit = MeshComponent->GetWorld()->SweepMultiByChannel(HitResults, StartTraceTransform.GetLocation(), EndTraceTransform.GetLocation(),
//			AverageTraceTransform.GetRotation(), A1_TraceChannel_Raider, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), Params);
//
//		for (const FHitResult& HitResult : HitResults)
//		{
//			AActor* HitActor = HitResult.GetActor();
//			if (HitActors.Contains(HitActor) == false)
//			{
//				HitActors.Add(HitActor);
//				FinalHitResults.Add(HitResult);
//			}
//		}
//
//#if UE_EDITOR
//		if (GIsEditor)
//		{
//			const UA1DeveloperSettings* DeveloperSettings = GetDefault<UA1DeveloperSettings>();
//			if (DeveloperSettings->bForceDisableDebugTrace == false && bDrawDebugShape)
//			{
//				FColor Color = (HitResults.Num() > 0) ? HitColor : TraceColor;
//
//				DrawDebugCapsule(MeshComponent->GetWorld(), AverageTraceTransform.GetLocation(), CapsuleHalfHeight, CapsuleRadius, AverageTraceTransform.GetRotation(), Color, false, 1.f);
//			}
//		}
//#endif
//	}
//
//	PreviousSocketTransform = CurrentSocketTransform;
//
//	if (FinalHitResults.Num() > 0)
//	{
//		FGameplayAbilityTargetDataHandle TargetDataHandle;
//
//		for (const FHitResult& HitResult : FinalHitResults)
//		{
//			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
//			NewTargetData->HitResult = HitResult;
//			TargetDataHandle.Add(NewTargetData);
//		}
//
//		FGameplayEventData EventData;
//		EventData.TargetData = TargetDataHandle;
//		EventData.Instigator = MeshComponent->GetOwner();
//
//		if (EventTag.IsValid())
//		{
//			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComponent->GetOwner(), EventTag, EventData);
//		}
//	}
//}