// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1FuelBase.h"

#include "A1FuelDisplayUI.h"
#include "A1SpaceshipBase.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "Item/A1ItemTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1FuelBase)

AA1FuelBase::AA1FuelBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	MeshComponent->SetCanEverAffectNavigation(true);

	UILocation = FVector(0.f, 0.f, 180.f);
	UIRotation = GetActorRotation();
}

void AA1FuelBase::BeginPlay()
{
	Super::BeginPlay();

	OwningSpaceship = FindSpaceshipOwner();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnedFuelUI = GetWorld()->SpawnActor<AA1FuelDisplayUI>(FuelUI, UILocation, UIRotation, SpawnParams);

	if (SpawnedFuelUI && SpawnedFuelUI->GetRootComponent())
	{
		SpawnedFuelUI->GetRootComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		SpawnedFuelUI->SetActorRelativeLocation(FVector(0.f, 0.f, 250.f));
		SpawnedFuelUI->SetActorRelativeRotation(FRotator(0.f, 0.f, 0.f));
	}

	if (OwningSpaceship.IsValid())
	{
		//Init Fuel Setting
		SpawnedFuelUI->InitSetting(OwningSpaceship->GetMaxFuelAmount(), OwningSpaceship->GetCurrentFuelAmount());

		//Subscribe Delegate
		OwningSpaceship->OnFuelChanged.AddDynamic(SpawnedFuelUI, &AA1FuelDisplayUI::UpdateFuelLevel);
	}

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

void AA1FuelBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FA1InteractionInfo AA1FuelBase::GetPreInteractionInfo(const FA1InteractionQuery& InteractionQuery) const
{
	return InteractionInfo;
}

void AA1FuelBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	if (MeshComponent->GetStaticMesh())
	{
		OutMeshComponents.Add(MeshComponent);
	}
}

void AA1FuelBase::RegisterWithSpaceship(class AA1SpaceshipBase* Spaceship)
{
	if (Spaceship && HasAuthority())
	{
		OwningSpaceship = Spaceship;
		Spaceship->RegisterFuelSystem(this);
	}
}

bool AA1FuelBase::IsFuelItem(AActor* Item) const
{
	if (!IsValid(Item))
		return false;
	
	/*
	 * TODO eric1306
	 * Fuel Item Find Logic
	 */

	return Item->ActorHasTag(FName("Fuel"));
}

void AA1FuelBase::DetectAndAbsorbFuelItems()
{
	if (!HasAuthority())
		return;

	// Initialize Detected Item List
	DetectedFuelItems.Empty();

	//Detect Item Actor -> Overlap Event
	TArray<FOverlapResult> OverlapResults;

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(DetectionRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	UE_LOG(LogTemp, Log, TEXT("Find Fuel...."));
	bool bResult = GetWorld()->OverlapMultiByChannel(
		OUT OverlapResults, 
		GetActorLocation(), 
		FQuat::Identity, 
		ECC_WorldDynamic,
		CollisionShape,
		QueryParams);
	UE_LOG(LogTemp, Log, TEXT("Complete. %d Actors find!"), OverlapResults.Num());

	if (bResult)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *Result.GetActor()->GetName());

			if (Result.GetActor() /*&& IsFuelItem(Result.GetActor())*/)
			{
				UE_LOG(LogTemp, Log, TEXT("Find Fuel!: %s"), *Result.GetActor()->GetName());

				DetectedFuelItems.Add(Result.GetActor());

				//TODO eric1306: cast to fuel and get fuel amount
				//TempCode
				{
					float FuelAmount = 500.0f;

					AddFuel(FuelAmount);

					//Result.GetActor()->Destroy();
				}
			}
		}
	}
}

void AA1FuelBase::AddFuel(float FuelAmount)
{
	if (!HasAuthority())
		return;

	if (!OwningSpaceship.IsValid())
	{
		OwningSpaceship = FindSpaceshipOwner();
		if (!OwningSpaceship.IsValid())
			return;
	}

	OwningSpaceship->AddFuel(FuelAmount);
}

void AA1FuelBase::SetupTags()
{
	Tags.AddUnique("SpaceshipComponent");
	Tags.AddUnique("FuelSystem");
}

AA1SpaceshipBase* AA1FuelBase::FindSpaceshipOwner() const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1SpaceshipBase::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Find Spaceship!"));
		return Cast<AA1SpaceshipBase>(FoundActors[0]);
	}

	return nullptr;
}
