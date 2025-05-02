// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1RaiderBase.h"

#include "A1LogChannels.h"
#include "Controller/Raider/A1RaiderController.h"
#include "AbilitySystem/Attributes/A1CharacterAttributeSet.h"
#include "GameplayEffect.h"
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
	HealthSet->OnOutOfHealth.AddUObject(this, &AA1RaiderBase::HandleOutOfHealth);
}

// Called when the game starts or when spawned
void AA1RaiderBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogA1Raider, Log, TEXT("RaiderBase: Beginplay Call"));
	
}

// Called every frame
void AA1RaiderBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AA1RaiderBase::HandleOutOfHealth(float OldValue, float NewValue)
{
	
	// �ִϸ��̼��� ó���� �ð� ��ٸ���
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AA1RaiderBase::DestroyDueToDeath, 1.5f, false);


	GetController()->UnPossess();  // AI�� �� �̻� ĳ���͸� �������� �ʵ��� ��
	
	
	if (DeadMontage)
	{
		StopAnimMontage();
		PlayAnimMontage(DeadMontage, 1.0f);
	}
}

void AA1RaiderBase::DestroyDueToDeath()
{
	Destroy();

	// õõ�� ������� ȿ��(������)
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
	//			delete CurrentFade;				// �Ҵ� ����
	//			this->Destroy();
	//		}
	//	});
	//
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, FadeDelegate, TickInterval, true);


	// ������ ����
	int ItemNum = dropItems.Num();

	FVector ItemSpawnLocation = GetActorLocation();
	ItemSpawnLocation.Z = 0;

	//Ȯ���� ���� ������ ����(�ϴ� ������ ����)
	//Ȯ�� �������� ���� �ϵ��ڵ�
	int32 index = FMath::RandRange(0, 100);
	if (index >= 50)
		GetWorld()->SpawnActor<AA1EquipmentBase>(dropItems[ItemNum-1], ItemSpawnLocation, GetActorRotation());
	else
		GetWorld()->SpawnActor<AA1EquipmentBase>(dropItems[0], ItemSpawnLocation, GetActorRotation());

}

