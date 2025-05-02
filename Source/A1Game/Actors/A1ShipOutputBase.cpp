// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1ShipOutputBase.h"

#include "A1SpaceshipBase.h"
#include "Components/ArrowComponent.h"
#include "GameModes/LyraGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1ShipOutputBase)

AA1ShipOutputBase::AA1ShipOutputBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutputMeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	MeshComponent->SetCanEverAffectNavigation(true);
}

void AA1ShipOutputBase::BeginPlay()
{
	Super::BeginPlay();

    
    SetupTags();

    if (HasAuthority())
    {
        OwningSpaceship = FindSpaceshipOwner();

        if (OwningSpaceship.IsValid())
        {
            RegisterWithSpaceship(OwningSpaceship.Get());
        }
    }
}

void AA1ShipOutputBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AA1ShipOutputBase, OutputState);
}

FA1InteractionInfo AA1ShipOutputBase::GetPreInteractionInfo(const FA1InteractionQuery& InteractionQuery) const
{
    switch (OutputState)
    {
    case EOutputState::Idle:
        return IdleInteractionInfo;
    case EOutputState::Ready:
        return ReadyInteractionInfo;
    case EOutputState::Active:
        return ActiveInteractionInfo;
    default:
        return FA1InteractionInfo();
    }
}

void AA1ShipOutputBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
    if (MeshComponent->GetStaticMesh())
    {
        OutMeshComponents.Add(MeshComponent);
    }
}

void AA1ShipOutputBase::RegisterWithSpaceship(class AA1SpaceshipBase* Spaceship)
{
    if (Spaceship && HasAuthority())
    {
        OwningSpaceship = Spaceship;
        Spaceship->RegisterShipOutput(this);
    }
}

void AA1ShipOutputBase::SetOutputState(EOutputState NewOutputState)
{
    if (HasAuthority() == false || NewOutputState == OutputState)
        return;

    OutputState = NewOutputState;
    OnRep_OutputState();
}

void AA1ShipOutputBase::DeactivateExternalMap()
{
    if(!HasAuthority())
        return;

    if (!OwningSpaceship.IsValid())
    {
        OwningSpaceship = FindSpaceshipOwner();
        if (!OwningSpaceship.IsValid())
            return;
    }

    if (OwningSpaceship->GetCurrentFuelAmount() >= FuelCostToDeactivateMap)
    {
        OwningSpaceship->ConsumeFuel(FuelCostToDeactivateMap);

        auto GameMode = Cast<ALyraGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        if (GameMode)
        {
            GameMode->TriggerFadeOnAllPlayer(0.0f, 1.f);
        }

        OwningSpaceship->DeactivateExternalMap();

        SetOutputState(EOutputState::Idle);

        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [=]()
            {
                
                if (GameMode)
                {
                    GameMode->TriggerFadeOnAllPlayer(1.f, 0.f);
                }
            }, 3.0f, false);
    }
}

void AA1ShipOutputBase::SetupTags()
{
    Tags.AddUnique("SpaceshipComponent");
    Tags.AddUnique("ShipOutput");
}

void AA1ShipOutputBase::OnRep_OutputState()
{
	OnOutputStateChanged(OutputState);
}

AA1SpaceshipBase* AA1ShipOutputBase::FindSpaceshipOwner() const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1SpaceshipBase::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		return Cast<AA1SpaceshipBase>(FoundActors[0]);
	}

	return nullptr;
}
