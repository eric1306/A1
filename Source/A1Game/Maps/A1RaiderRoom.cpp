// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Maps/A1RaiderRoom.h"

#include "Character/Raider/A1RaiderBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1RaiderRoom)

AA1RaiderRoom::AA1RaiderRoom(const FObjectInitializer& ObjectInitializer)
{
	EssentialSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Essential Spawn Area"));
	EssentialSpawn->SetupAttachment(GetRootComponent());

	OptionalSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Optional Spawn Area"));
	OptionalSpawn->SetupAttachment(GetRootComponent());
}

void AA1RaiderRoom::BeginPlay()
{
	Super::BeginPlay();
}

void AA1RaiderRoom::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AA1RaiderRoom, SpawnedRaiders);
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