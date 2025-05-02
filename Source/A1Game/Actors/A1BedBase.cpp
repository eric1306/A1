// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1BedBase.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1BedBase)

AA1BedBase::AA1BedBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BedMeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	MeshComponent->SetCanEverAffectNavigation(true);
}

void AA1BedBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AA1BedBase, BedState);
	DOREPLIFETIME(AA1BedBase, StoredPlayerTransform);
}

void AA1BedBase::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AA1BedBase::InBedState, 30.f, false);

	SetupTags();

	if (HasAuthority())
	{
		TArray<AActor*> FoundSpaceships;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1SpaceshipBase::StaticClass(), FoundSpaceships);

		for (AActor* Actor : FoundSpaceships)
		{
			if (AA1SpaceshipBase* Spaceship = Cast<AA1SpaceshipBase>(Actor))
			{
				RegisterWithSpaceship(Spaceship);
				break;
			}
		}
	}
}

FA1InteractionInfo AA1BedBase::GetPreInteractionInfo(const FA1InteractionQuery& InteractionQuery) const
{
	switch (BedState)
	{
	case EBedState::Empty: return EmptyInteractionInfo;
	case EBedState::Occupied: return OccupiedInteractionInfo;
	default: return FA1InteractionInfo();
	}
}

void AA1BedBase::GetMeshComponents(OUT TArray<UMeshComponent*>& OutMeshComponents) const
{

	if (MeshComponent->GetStaticMesh())
	{
		OutMeshComponents.Add(MeshComponent);
	}
}

void AA1BedBase::RegisterWithSpaceship(class AA1SpaceshipBase* Spaceship)
{
	if (Spaceship && HasAuthority())
	{
		Spaceship->RegisterBed(this);
	}
}

void AA1BedBase::SetBedState(EBedState NewBedState)
{
	if (HasAuthority() == false || NewBedState == BedState)
		return;

	BedState = NewBedState;
	OnRep_BedState();
}

FTransform AA1BedBase::GetLayDownTransform() const
{
	return GetActorTransform() + LayDownOffset;
}

FRotator AA1BedBase::GetArrowComponentVector() const
{
	return ArrowComponent->GetComponentRotation();
}

void AA1BedBase::StorePlayerReturnTransform(const FTransform& PlayerTransform)
{
	if (HasAuthority())
	{
		StoredPlayerTransform = PlayerTransform;
	}
}

FTransform AA1BedBase::GetPlayerReturnTransform() const
{
	return StoredPlayerTransform;
}

void AA1BedBase::RecoverPlayerFatigue(AActor* Player, float DeltaTime)
{
	if (!HasAuthority() || !Player || BedState != EBedState::Occupied)
		return;

	//TODO eric1306
	//Heal Fatigue
}

void AA1BedBase::OnRep_BedState()
{
	OnBedStateChanged(BedState);
}

void AA1BedBase::InBedState()
{
	if (BedState == EBedState::Occupied)
	{
		//TODO eric1306
		/*
		 * down 1 Weight / 30 seconds
		 */
	}
}

void AA1BedBase::SetupTags()
{
	Tags.AddUnique("SpaceshipComponent");
	Tags.AddUnique("Bed");
}
