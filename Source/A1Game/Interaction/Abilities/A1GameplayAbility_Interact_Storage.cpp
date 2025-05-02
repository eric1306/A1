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

    // â�� ���� ��������
    AA1StorageBase* StorageActor = Cast<AA1StorageBase>(InteractableActor);
    if (StorageActor == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    // ��ȣ�ۿ��ϴ� �÷��̾�
    AActor* Interactor = GetAvatarActorFromActorInfo();
    if (Interactor == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    // ���� Ʈ���̽��� ������ ã��
    PerformPickupLineTrace();

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UA1GameplayAbility_Interact_Storage::PerformPickupLineTrace()
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    ACharacter* Character = Cast<ACharacter>(Avatar);

    if (!Character)
        return;

    // ĳ������ �ü� ���� ���ϱ�
    FVector CameraLocation;
    FRotator CameraRotation;
    Character->GetActorEyesViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation;
    FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * PickupRange);

    // ���� Ʈ���̽��� ���� ������Ʈ Ÿ�� ����
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

    // ������ ���� ����
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Character);

    // ���� Ʈ���̽� ����
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

    // ��Ʈ ��� ó��
    if (bHit && HitResult.GetActor())
    {
        AActor* HitActor = HitResult.GetActor();

        // ���丮�� �ڽ��� �ƴ��� Ȯ��
        if (!HitActor->IsA<AA1StorageBase>())
        {
            // ���������� Ȯ��
            TryPickupItem(HitActor);
        }
    }
}

void UA1GameplayAbility_Interact_Storage::TryPickupItem(AActor* ItemActor)
{
    if (!ItemActor)
        return;

    // ���������� ó��
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar || !Avatar->HasAuthority())
        return;

    // ���������� Ȯ�� (IA1Pickupable �������̽� Ȯ��)
    IA1Pickupable* PickupableItem = Cast<IA1Pickupable>(ItemActor);
    if (!PickupableItem)
        return;

    // �÷��̾��� �κ��丮 ������Ʈ ã��
    UA1InventoryManagerComponent* InventoryManager = Avatar->FindComponentByClass<UA1InventoryManagerComponent>();
    UA1ItemManagerComponent* ItemManager = Avatar->FindComponentByClass<UA1ItemManagerComponent>();

    if (!InventoryManager || !ItemManager)
        return;

    // ������ ���� ��������
    const FA1PickupInfo& PickupInfo = PickupableItem->GetPickupInfo();

    bool bAddedSuccessfully = false;

    if (PickupInfo.PickupInstance.ItemInstance)
    {
        // ������ �ν��Ͻ��� �ִ� ���
        UA1ItemInstance* ItemInstance = PickupInfo.PickupInstance.ItemInstance;
        int32 ItemCount = PickupInfo.PickupInstance.ItemCount;

        // �κ��丮�� �߰�
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
        // ������ ���ø��� �ִ� ���
        int32 ItemTemplateID = UA1ItemData::Get().FindItemTemplateIDByClass(PickupInfo.PickupTemplate.ItemTemplateClass);
        EItemRarity ItemRarity = PickupInfo.PickupTemplate.ItemRarity;
        int32 ItemCount = PickupInfo.PickupTemplate.ItemCount;

        // �κ��丮�� �߰�
        bAddedSuccessfully = InventoryManager->TryAddItemByRarity(PickupInfo.PickupTemplate.ItemTemplateClass,
            ItemRarity,
            ItemCount) > 0;
    }

    // ���������� �߰��Ǹ� ������ ���� ����
    if (bAddedSuccessfully)
    {
        ItemActor->Destroy();
    }
}
