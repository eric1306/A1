// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1StorageBase.h"

#include "A1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "Data/A1ItemData.h"
#include "Physics/LyraCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1StorageBase)

AA1StorageBase::AA1StorageBase(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    SetRootComponent(ArrowComponent);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BedMeshComponent"));
    MeshComponent->SetupAttachment(GetRootComponent());
    MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
    MeshComponent->SetCanEverAffectNavigation(true);

    StoreLocation = CreateDefaultSubobject<USceneComponent>(TEXT("StoreLocation"));
    StoreLocation->SetupAttachment(GetRootComponent());

    ItemDetectionRadius = 300.f;

    MaxStorageSize = 16;

    //Interact ����� �ƴϱ� ������
    bCanUsed = false;

    //ó���� idx�� 0
    LatestIdx = 0;
}

void AA1StorageBase::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
        GetWorldTimerManager().SetTimer(ItemDetectTimerHandle, this, &AA1StorageBase::TryDetectItem, 3.f, true);

    //�������� ��ġ��ų Location ���� �޾ƿ���(Scene Component)
    TArray<USceneComponent*> TmpList;
    StoreLocation->GetChildrenComponents(false, TmpList);
    StoreLocations.Append(TmpList);

}

void AA1StorageBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AA1StorageBase::RegisterWithSpaceship(AA1SpaceshipBase* Spaceship)
{
    if (Spaceship && HasAuthority())
    {
        OwningSpaceship = Spaceship;
        Spaceship->RegisterStorage(this);
    }
}

void AA1StorageBase::SetupTags()
{
    Tags.AddUnique("SpaceshipComponent");
    Tags.AddUnique("Storage");
}

AA1SpaceshipBase* AA1StorageBase::FindSpaceshipOwner() const
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1SpaceshipBase::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        return Cast<AA1SpaceshipBase>(FoundActors[0]);
    }

    return nullptr;
}

void AA1StorageBase::TryDetectItem()
{
    UE_LOG(LogTemp, Log, TEXT("Start Detect Item!"));
    TArray<FOverlapResult> Overlaps;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // â�� ��ü�� ����
    QueryParams.AddIgnoredActors(SavedEquips);

    // ��ü ���·� ������ �˻�
    FCollisionShape CollisionShape;
    CollisionShape.SetSphere(ItemDetectionRadius);

    // ������ �˻� ����
    bool bResult = GetWorld()->OverlapMultiByChannel(
        Overlaps,
        GetActorLocation(),
        FQuat::Identity,
        A1_TraceChannel_AimAssist, // ���� ��ü ���� ä��
        CollisionShape,
        QueryParams
    );

    if (bResult)
    {
        UE_LOG(LogTemp, Log, TEXT("Find %d Actors!"), Overlaps.Num());
        UE_LOG(LogTemp, Log, TEXT("============================"));
        for (const FOverlapResult& Overlap : Overlaps)
        {
            AActor* OverlappedActor = Overlap.GetActor();
            UE_LOG(LogTemp, Log, TEXT("%s Find!"), *OverlappedActor->GetName());
            if (!OverlappedActor)
                continue;

            if (AA1EquipmentBase* Equipment = Cast<AA1EquipmentBase>(OverlappedActor))
            {
                if (SavedEquips.Find(Equipment) && !Equipment->GetPickup())
                {
                    SavedEquips.Add(Equipment);

                    UE_LOG(LogTemp, Log, TEXT("%s added to Saved Equips"), *Equipment->GetName());

                    TryStoreItem(Equipment);
                }
            }
        }
        UE_LOG(LogTemp, Log, TEXT("============================"));
    }

#if ENABLE_DRAW_DEBUG

    DrawDebugSphere(
        GetWorld(),
        GetActorLocation(),
        ItemDetectionRadius,
        16,
        FColor::Blue,
        false,
        0.5f
    );
#endif
}

void AA1StorageBase::TryStoreItem(AA1EquipmentBase* Equip)
{
    if (LatestIdx >= MaxStorageSize)
        return;

    FVector Location = StoreLocations[LatestIdx++]->GetComponentLocation();
    Equip->SetActorRelativeRotation(FRotator(0.f, 0.f, 0.f));
    Equip->SetActorRelativeLocation(Location);

}
