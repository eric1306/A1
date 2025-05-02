// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1RescueSignalBase.h"

#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Maps/A1RandomMapGenerator.h"
#include "Math/TransformCalculus3D.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(A1RescueSignalBase)

AA1RescueSignalBase::AA1RescueSignalBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;


	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	MeshComponent->SetCanEverAffectNavigation(true);
}

void AA1RescueSignalBase::BeginPlay()
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

void AA1RescueSignalBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SignalState);
}

FA1InteractionInfo AA1RescueSignalBase::GetPreInteractionInfo(const FA1InteractionQuery& InteractionQuery) const
{
	switch (SignalState)
	{
	case ESignalState::Pressed:		return PressedInteractionInfo;
	case ESignalState::Released:	return ReleasedInteractionInfo;
	default:						return FA1InteractionInfo();
	}
}

void AA1RescueSignalBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	OutMeshComponents.Add(MeshComponent);
}

void AA1RescueSignalBase::RegisterWithSpaceship(class AA1SpaceshipBase* Spaceship)
{
	if (Spaceship && HasAuthority())
	{
		OwningSpaceship = Spaceship;
		Spaceship->RegisterRescueSignal(this);
	}
}

void AA1RescueSignalBase::SetSignalState(ESignalState NewSignalState)
{
	if (HasAuthority() == false || NewSignalState == SignalState)
		return;

	SignalState = NewSignalState;
	OnRep_SignalState();
}

void AA1RescueSignalBase::SetupTags()
{
	Tags.AddUnique("SpaceshipComponent");
	Tags.AddUnique("RescueSignal");
}

AA1SpaceshipBase* AA1RescueSignalBase::FindSpaceshipOwner() const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1SpaceshipBase::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		return Cast<AA1SpaceshipBase>(FoundActors[0]);
	}

	return nullptr;
}

void AA1RescueSignalBase::OnRep_SignalState()
{
	OnSignalStateChanged(SignalState);
}
