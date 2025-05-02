// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Interaction/Abilities/A1GameplayAbility_Interact_Storage.h"

#include "Actors/A1StorageBase.h"
#include "Data/A1ItemData.h"
#include "GameFramework/Character.h"
#include "Interaction/A1Pickupable.h"
#include "Item/A1ItemInstance.h"
#include "Item/A1ItemTemplate.h"
#include "Item/Managers/A1InventoryManagerComponent.h"
#include "Item/Managers/A1ItemManagerComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_Interact_Storage)

UA1GameplayAbility_Interact_Storage::UA1GameplayAbility_Interact_Storage(const FObjectInitializer& ObjectInitializer)
{
}

void UA1GameplayAbility_Interact_Storage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

    // 창고 액터 가져오기
    AA1StorageBase* StorageActor = Cast<AA1StorageBase>(InteractableActor);
    if (StorageActor == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    // 상호작용하는 플레이어
    AActor* Interactor = GetAvatarActorFromActorInfo();
    if (Interactor == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    // 라인 트레이스로 아이템 찾기
    PerformPickupLineTrace();

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UA1GameplayAbility_Interact_Storage::PerformPickupLineTrace()
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    ACharacter* Character = Cast<ACharacter>(Avatar);

    if (!Character)
        return;

    // 캐릭터의 시선 방향 구하기
    FVector CameraLocation;
    FRotator CameraRotation;
    Character->GetActorEyesViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation;
    FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * PickupRange);

    // 라인 트레이스를 위한 오브젝트 타입 설정
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

    // 무시할 액터 설정
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Character);

    // 라인 트레이스 수행
    FHitResult HitResult;
    bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
        GetWorld(),
        TraceStart,
        TraceEnd,
        ObjectTypes,
        false,
        ActorsToIgnore,
        bShowDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
        HitResult,
        true
    );

    // 히트 결과 처리
    if (bHit && HitResult.GetActor())
    {
        AActor* HitActor = HitResult.GetActor();

        // 스토리지 박스가 아닌지 확인
        if (!HitActor->IsA<AA1StorageBase>())
        {
            // 아이템인지 확인
            TryPickupItem(HitActor);
        }
    }
}

void UA1GameplayAbility_Interact_Storage::TryPickupItem(AActor* ItemActor)
{
    if (!ItemActor)
        return;

    // 서버에서만 처리
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar || !Avatar->HasAuthority())
        return;

    // 아이템인지 확인 (IA1Pickupable 인터페이스 확인)
    IA1Pickupable* PickupableItem = Cast<IA1Pickupable>(ItemActor);
    if (!PickupableItem)
        return;

    // 플레이어의 인벤토리 컴포넌트 찾기
    UA1InventoryManagerComponent* InventoryManager = Avatar->FindComponentByClass<UA1InventoryManagerComponent>();
    UA1ItemManagerComponent* ItemManager = Avatar->FindComponentByClass<UA1ItemManagerComponent>();

    if (!InventoryManager || !ItemManager)
        return;

    // 아이템 정보 가져오기
    const FA1PickupInfo& PickupInfo = PickupableItem->GetPickupInfo();

    bool bAddedSuccessfully = false;

    if (PickupInfo.PickupInstance.ItemInstance)
    {
        // 아이템 인스턴스가 있는 경우
        UA1ItemInstance* ItemInstance = PickupInfo.PickupInstance.ItemInstance;
        int32 ItemCount = PickupInfo.PickupInstance.ItemCount;

        // 인벤토리에 추가
        TArray<FIntPoint> ToItemSlotPoses;
        TArray<int32> ToItemCounts;

        int32 MovableCount = InventoryManager->CanAddItem(ItemInstance->GetItemTemplateID(),
            ItemInstance->GetItemRarity(),
            ItemCount,
            ToItemSlotPoses,
            ToItemCounts);

        if (MovableCount > 0)
        {
            for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
            {
                //InventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], ItemInstance, ToItemCounts[i]);
            }

            bAddedSuccessfully = true;
        }
    }
    else if (PickupInfo.PickupTemplate.ItemTemplateClass)
    {
        // 아이템 템플릿이 있는 경우
        int32 ItemTemplateID = UA1ItemData::Get().FindItemTemplateIDByClass(PickupInfo.PickupTemplate.ItemTemplateClass);
        EItemRarity ItemRarity = PickupInfo.PickupTemplate.ItemRarity;
        int32 ItemCount = PickupInfo.PickupTemplate.ItemCount;

        // 인벤토리에 추가
        bAddedSuccessfully = InventoryManager->TryAddItemByRarity(PickupInfo.PickupTemplate.ItemTemplateClass,
            ItemRarity,
            ItemCount) > 0;
    }

    // 성공적으로 추가되면 아이템 액터 제거
    if (bAddedSuccessfully)
    {
        ItemActor->Destroy();
    }
}
