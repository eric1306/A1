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
	EssentialSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Essential Spawn Area"));
	EssentialSpawn->SetupAttachment(GetRootComponent());

	OptionalSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Optional Spawn Area"));
	OptionalSpawn->SetupAttachment(GetRootComponent());

    ItemSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSpawnLocation"));
    ItemSpawnLocation->SetupAttachment(GetRootComponent());

    ItemBoxSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ChestLocation"));
    ItemBoxSpawnLocation->SetupAttachment(GetRootComponent());
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

void AA1RaiderRoom::SpawnEnemy()
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
            //Spawn Raider
            FTransform childTransform = child->GetComponentTransform();
            AA1RaiderBase* Raider = GetWorld()->SpawnActorDeferred<AA1RaiderBase>(RaiderClass, childTransform, this);
            if (Raider)
            {
                //Net Update
                Raider->SetReplicates(true);
                Raider->bAlwaysRelevant = true;
                Raider->NetUpdateFrequency = 60.0f;
                Raider->MinNetUpdateFrequency = 30.0f;
                Raider->NetPriority = 3.0f;

                UGameplayStatics::FinishSpawningActor(Raider, childTransform);

                Raider->SetReplicates(true);
                Raider->SetReplicateMovement(true);
                Raider->SetActorTickEnabled(true);
                Raider->bAlwaysRelevant = true;
                Raider->NetUpdateFrequency = 60.0f;
                Raider->MinNetUpdateFrequency = 30.0f;
                Raider->NetPriority = 3.0f;
                Raider->SetNetDormancy(ENetDormancy::DORM_Awake);

                Raider->ForceNetUpdate();

                //Replicated
                SpawnedRaiders.Add(Raider);
            }
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
            
            FTransform childTransform = child->GetComponentTransform();
            AA1RaiderBase* Raider = GetWorld()->SpawnActorDeferred<AA1RaiderBase>(RaiderClass, childTransform, this);
            if (Raider)
            {
                UE_LOG(LogTemp, Log, TEXT("Spawn Raider Complete!"));

                Raider->SetReplicates(true);
                Raider->bAlwaysRelevant = true;
                Raider->NetUpdateFrequency = 60.0f;
                Raider->MinNetUpdateFrequency = 30.0f;
                Raider->NetPriority = 3.0f;

                UGameplayStatics::FinishSpawningActor(Raider, childTransform);

                Raider->SetReplicates(true);
                Raider->SetReplicateMovement(true);
                Raider->SetActorTickEnabled(true);
                Raider->bAlwaysRelevant = true;
                Raider->NetUpdateFrequency = 60.0f;
                Raider->MinNetUpdateFrequency = 30.0f;
                Raider->NetPriority = 3.0f;
                Raider->SetNetDormancy(ENetDormancy::DORM_Awake);

                Raider->ForceNetUpdate();

                //Replicated
                SpawnedRaiders.Add(Raider);
            }
        }
    }

}

void AA1RaiderRoom::SpawnItem()
{
    TArray<USceneComponent*> ChildrenComp;
    ItemSpawnLocation->GetChildrenComponents(false, ChildrenComp);
    ItemSpawnLocations.Append(ChildrenComp);
    int32 SpawnItemRate = FMath::RandRange(MinOptionalItemCount, MaxOptionalItemCount);

    UE_LOG(LogTemp, Log, TEXT("ItemSpawnLocations size: %d"), ItemSpawnLocations.Num());

    UA1ItemData::Get().GetAllItemTemplateClasses(OUT CachedItemTemplates);

    UE_LOG(LogTemp, Log, TEXT("SpawnItemRate: %d"), SpawnItemRate);

    for (int i = 0; i < SpawnItemRate; i++)
    {
        int32 ItemtoSpawn = FMath::RandRange(2, CachedItemTemplates.Num() - 1);
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
            NewSpawnedItem->SetActorRelativeTransform(ItemSpawnLocations[i]->GetComponentTransform());
            NewSpawnedItem->SetActorScale3D(FVector(1.f, 1.f, 1.f));
            NewSpawnedItem->SetPickup(false);
            NewSpawnedItem->SetActorHiddenInGame(false);
            NewSpawnedItem->FinishSpawning(FTransform::Identity, true);
        }
    }

    TArray<USceneComponent*> ChestSpawnComponent;
    ItemBoxSpawnLocation->GetChildrenComponents(false, ChestSpawnComponent);
    ItemBoxSpawnLocations.Append(ChestSpawnComponent);

    for (auto child : ChestSpawnComponent)
    {
        if (child)
        {
            //Spawn Raider
            FTransform childTransform = child->GetComponentTransform();
            AA1ChestBase* Chest = GetWorld()->SpawnActorDeferred<AA1ChestBase>(ChestClass, childTransform, this);
            if (Chest)
            {
                //Net Update
                /*Chest->SetReplicates(true);
                Chest->bAlwaysRelevant = true;
                Chest->NetUpdateFrequency = 60.0f;
                Chest->MinNetUpdateFrequency = 30.0f;
                Chest->NetPriority = 3.0f;*/

                UGameplayStatics::FinishSpawningActor(Chest, childTransform);

                /*Chest->SetReplicates(true);
                Chest->SetReplicateMovement(true);
                Chest->SetActorTickEnabled(true);
                Chest->bAlwaysRelevant = true;
                Chest->NetUpdateFrequency = 60.0f;
                Chest->MinNetUpdateFrequency = 30.0f;
                Chest->NetPriority = 3.0f;
                Chest->SetNetDormancy(ENetDormancy::DORM_Awake);

                Chest->ForceNetUpdate();*/

                //Replicated
                SpawnedChests.Add(Chest);
            }
        }
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
    FCollisionShape CollisionShape;
	CollisionShape.SetBox(FVector3f(3000.f, 3000.f, 1000.f));

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    TArray<FOverlapResult> OverlapResults;
    bool bResult = GetWorld()->OverlapMultiByChannel(
        OUT OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        A1_TraceChannel_AimAssist,
        CollisionShape,
        QueryParams
    );

    if (bResult)
    {
        UE_LOG(LogTemp, Log, TEXT("Find %d Actors!"), OverlapResults.Num());
        UE_LOG(LogTemp, Log, TEXT("*****************************"));
        for (const FOverlapResult& Overlap : OverlapResults)
        {
            AActor* OverlappedActor = Overlap.GetActor();
            UE_LOG(LogTemp, Log, TEXT("%s Find!"), *OverlappedActor->GetName());
            if (!OverlappedActor)
                continue;

            if (AA1EquipmentBase* Equipment = Cast<AA1EquipmentBase>(OverlappedActor))
            {
                Equipment->Destroy();
            }

            if (AA1ChestBase* Chest = Cast<AA1ChestBase>(OverlappedActor))
            {
                Chest->Destroy();
            }
        }
        UE_LOG(LogTemp, Log, TEXT("*****************************"));
    }
}
