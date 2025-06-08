// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1EmptyBase.h"

#include "Components/ArrowComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1EmptyBase)

AA1EmptyBase::AA1EmptyBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	MeshComponent->SetCanEverAffectNavigation(true);
}

void AA1EmptyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AA1EmptyBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FA1InteractionInfo AA1EmptyBase::GetPreInteractionInfo(const FA1InteractionQuery& InteractionQuery) const
{
	return InteractionInfo;
}

void AA1EmptyBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	if (MeshComponent->GetStaticMesh())
	{
		OutMeshComponents.Add(MeshComponent);
	}
}
