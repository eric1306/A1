// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Maps/A1RaiderRoom.h"

#include "Actors/A1ChestBase.h"
#include "Actors/A1EquipmentBase.h"
#include "Character/Raider/A1RaiderBase.h"
#include "Data/A1ItemData.h"
#include "Item/A1ItemInstance.h"
#include "Item/Fragments/A1ItemFragment_Equipable_Attachment.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LyraCollisionChannels.h"
#include "Engine/OverlapResult.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1RaiderRoom)

AA1RaiderRoom::AA1RaiderRoom(const FObjectInitializer& ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

	EssentialSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Essential Spawn Area"));
	EssentialSpawn->SetupAttachment(GetRootComponent());

	OptionalSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Optional Spawn Area"));
	OptionalSpawn->SetupAttachment(GetRootComponent());

    ItemSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSpawnLocation"));
    ItemSpawnLocation->SetupAttachment(GetRootComponent());

    ItemBoxSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ChestLocation"));
    ItemBoxSpawnLocation->SetupAttachment(GetRootComponent());

    RemoveItemPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Remove Pos Location"));
    RemoveItemPosition->SetupAttachment(GetRootComponent());
}

void AA1RaiderRoom::BeginPlay()
{
	Super::BeginPlay();
}

void AA1RaiderRoom::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AA1RaiderRoom, SpawnedRaiders);
	DOREPLIFETIME(AA1RaiderRoom, SpawnedItems);
}

void AA1RaiderRoom::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

    if (!HasAuthority())
        return;

    // 스폰 간격 체크
    SpawnTimer += DeltaSeconds;
    if (SpawnTimer < SpawnInterval)
        return;

    SpawnTimer = 0.0f;

    // 프레임 시간에 따라 스폰 개수 동적 조정
    float CurrentFrameTime = FApp::GetDeltaTime();
    if (CurrentFrameTime > TargetFrameTime * 1.5f) // 40 FPS 이하
    {
        SpawnPerTick = MinSpawnPerTick;
    }
    else if (CurrentFrameTime < TargetFrameTime) // 60 FPS 이상
    {
        SpawnPerTick = MaxSpawnPerTick;
    }

    ProcessSpawnQueue();
}

void AA1RaiderRoom::SpawnEnemys()
{
    if (!HasAuthority())
        return;

    //Essential
	auto EssentialSpot = EssentialSpawn;
    TArray<USceneComponent*> ChildrenComp;
    EssentialSpot->GetChildrenComponents(false, ChildrenComp);

    for (auto child : ChildrenComp)
    {
        if (child)
        {
            AddEnemyToQueue(child->GetComponentTransform());
        }
    }

    //Optional
    TArray<USceneComponent*> ChildrenComp2;
    auto OptionalSpot = OptionalSpawn;
    OptionalSpot->GetChildrenComponents(false, ChildrenComp2);
	int32 SpawnOptionalMonsterRate = FMath::RandRange(MinOptionalMonster, MaxOptionalMonster);

    UE_LOG(LogTemp, Log, TEXT("OptionalSpot count: %d"), ChildrenComp2.Num());
    for (auto child : ChildrenComp2)
    {
        //Spawn or Pass?
        int32 SpawnRate = FMath::RandRange(1, 101);

        if (SpawnPercentage < SpawnRate)
        {
            continue;
        }

        if (child && SpawnOptionalMonsterRate--)
        {
            //Spawn Raider
            AddEnemyToQueue(child->GetComponentTransform());
        }
    }
}

void AA1RaiderRoom::SpawnEnemy(FTransform SpawnTransform)
{
    AA1RaiderBase* Raider = GetWorld()->SpawnActorDeferred<AA1RaiderBase>(RaiderClass, SpawnTransform, this);
    if (Raider)
    {
        //UE_LOG(LogTemp, Log, TEXT("Spawn Raider Complete!"));

        //Raider->SetReplicates(true);
        //Raider->bAlwaysRelevant = false;
        //Raider->NetUpdateFrequency = 30.0f;
        //Raider->MinNetUpdateFrequency = 10.0f;
        //Raider->NetPriority = 2.0f; // 우선순위 감소

        //// 초기에는 Dormant 상태로 설정
        //Raider->SetNetDormancy(ENetDormancy::DORM_DormantAll);

        UGameplayStatics::FinishSpawningActor(Raider, SpawnTransform);

        //Replicated
        SpawnedRaiders.Add(Raider);
    }
}
void AA1RaiderRoom::SpawnItems()
{
    //Spawn Items
    TArray<USceneComponent*> ChildrenComp;
    ItemSpawnLocation->GetChildrenComponents(false, ChildrenComp);
    ItemSpawnLocations.Append(ChildrenComp);
    int32 SpawnItemRate = FMath::RandRange(MinOptionalItemCount, MaxOptionalItemCount);

    UE_LOG(LogTemp, Log, TEXT("ItemSpawnLocations size: %d"), ItemSpawnLocations.Num());

    UA1ItemData::Get().GetAllItemTemplateClasses(OUT CachedItemTemplates);

    UE_LOG(LogTemp, Log, TEXT("SpawnItemRate: %d"), SpawnItemRate);

    for (int i = 0; i < SpawnItemRate; i++)
    {
        // 큐에 추가
        AddItemToQueue(i);
    }

    //Spawn Chest
    TArray<USceneComponent*> ChestSpawnComponent;
    ItemBoxSpawnLocation->GetChildrenComponents(false, ChestSpawnComponent);
    ItemBoxSpawnLocations.Append(ChestSpawnComponent);

    for (auto child : ChestSpawnComponent)
    {
        if (child)
        {
            AddChestToQueue(child->GetComponentTransform());
        }
    }
}

void AA1RaiderRoom::SpawnItem(int32 idx)
{
    int32 ItemtoSpawn = FMath::RandRange(2, CachedItemTemplates.Num() - 5   );
    TSubclassOf<UA1ItemTemplate> ItemTemplateClass = CachedItemTemplates[ItemtoSpawn];
    int32 ItemTemplateId = UA1ItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
    UA1ItemInstance* AddedItemInstance = NewObject<UA1ItemInstance>();
    AddedItemInstance->Init(ItemTemplateId, EItemRarity::Poor);
    const UA1ItemFragment_Equipable_Attachment* AttachmentFragment = AddedItemInstance->FindFragmentByClass<UA1ItemFragment_Equipable_Attachment>();
    const FA1ItemAttachInfo& AttachInfo = AttachmentFragment->ItemAttachInfo;

    if (AttachInfo.SpawnItemClass)
    {
        AA1EquipmentBase* NewSpawnedItem = GetWorld()->SpawnActorDeferred<AA1EquipmentBase>(AttachInfo.SpawnItemClass, FTransform::Identity, this);
        NewSpawnedItem->Init(AddedItemInstance->GetItemTemplateID(), EEquipmentSlotType::Count, AddedItemInstance->GetItemRarity());
        NewSpawnedItem->SetActorRelativeTransform(ItemSpawnLocations[idx]->GetComponentTransform());
        NewSpawnedItem->SetActorScale3D(FVector(1.f, 1.f, 1.f));
        NewSpawnedItem->SetPickup(false);
        NewSpawnedItem->SetActorHiddenInGame(false);
        NewSpawnedItem->FinishSpawning(FTransform::Identity, true);
    }
}

void AA1RaiderRoom::SpawnChest(FTransform SpawnTransform)
{
    AA1ChestBase* Chest = GetWorld()->SpawnActorDeferred<AA1ChestBase>(ChestClass, SpawnTransform, this);
    if (Chest)
    {
        UGameplayStatics::FinishSpawningActor(Chest, SpawnTransform);
        //Replicated
        SpawnedChests.Add(Chest);
    }
}

void AA1RaiderRoom::RemoveEnemy()
{
    for (auto Raider : SpawnedRaiders)
    {
        if (Raider)
        {
            Raider->Destroy();
        }
    }

    SpawnedRaiders.Empty();
}

void AA1RaiderRoom::RemoveItem()
{
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    TArray<FOverlapResult> OverlapResults;
    
    bool bResult = GetWorld()->OverlapMultiByChannel(
        OUT OverlapResults,
        RemoveItemPosition->GetComponentLocation(),
        FQuat::Identity,
        A1_TraceChannel_AimAssist,
        FCollisionShape::MakeBox(FVector(1500.f,1500.f,1000.f)),
        QueryParams
    );
    
    if (bResult)
    {
        UE_LOG(LogTemp, Log, TEXT("Start Remove Outer map Items and chest"));
        UE_LOG(LogTemp, Log, TEXT("Find %d Actors!"), OverlapResults.Num());
        UE_LOG(LogTemp, Log, TEXT("*****************************"));
        for (const FOverlapResult& Overlap : OverlapResults)
        {
            AActor* OverlappedActor = Overlap.GetActor();

            if (!OverlappedActor)
                continue;

            UE_LOG(LogTemp, Log, TEXT("%s"), *OverlappedActor->GetName());

            if (AA1EquipmentBase* Equipment = Cast<AA1EquipmentBase>(OverlappedActor))
            {
                UE_LOG(LogTemp, Log, TEXT("[Remove] %s"), *Equipment->GetName());
                Equipment->SetLifeSpan(0.01f);
                Equipment->Destroy(true);
            }
        }
        UE_LOG(LogTemp, Log, TEXT("*****************************"));
    }
}

void AA1RaiderRoom::RemoveChest()
{
    for (auto c : SpawnedChests)
    {
	    if (c)
	    {
            c->SetLifeSpan(0.01f);
            c->Destroy(true);
	    }
    }
}

void AA1RaiderRoom::AddEnemyToQueue(const FTransform& Transform)
{
    SpawnQueue.Enqueue(FSpawnQueueItem(FSpawnQueueItem::Enemy, Transform));
}

void AA1RaiderRoom::AddItemToQueue(int32 ItemIndex)
{
    SpawnQueue.Enqueue(FSpawnQueueItem(FSpawnQueueItem::Item, FTransform::Identity, ItemIndex));
}

void AA1RaiderRoom::AddChestToQueue(const FTransform& Transform)
{
    SpawnQueue.Enqueue(FSpawnQueueItem(FSpawnQueueItem::Chest, Transform));
}

void AA1RaiderRoom::ProcessSpawnQueue()
{
    if (!HasAuthority())
        return;

    int32 SpawnedThisTick = 0;
    FSpawnQueueItem Item;

    // 틱당 최대 SpawnPerTick 개수만큼 스폰
    while (SpawnedThisTick < SpawnPerTick && SpawnQueue.Dequeue(Item))
    {
        switch (Item.Type)
        {
        case FSpawnQueueItem::Enemy:
            SpawnEnemy(Item.Transform);
            break;
        case FSpawnQueueItem::Item:
            SpawnItem(Item.ItemIndex);
            break;
        case FSpawnQueueItem::Chest:
            SpawnChest(Item.Transform);
            break;
        }

        SpawnedThisTick++;
    }
}
