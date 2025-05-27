// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1SpaceshipBase.h"

#include "A1BedBase.h"
#include "A1DoorBase.h"
#include "A1FuelBase.h"
#include "A1DockingSignalHandlerBase.h"
#include "A1ShipOutputBase.h"
#include "A1SignalDetectionBase.h"
#include "A1StorageBase.h"
#include "GameModes/LyraGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Maps/A1RandomMapGenerator.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1SpaceshipBase)

AA1SpaceshipBase::AA1SpaceshipBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AA1SpaceshipBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FindComponentsByTags();

		if (!DockingSignalHandler || !CacheDoor || !FuelSystem || !ShipOutput || Beds.IsEmpty()/*|| Storages.IsEmpty()*/)
		{
			FindSpaceshipComponents();
		}
	}

	FTimerDelegate FuelConsumeDelegate = FTimerDelegate::CreateUObject(
		this,
		&ThisClass::ConsumeFuel,
		1.f);

	GetWorldTimerManager().SetTimer(FuelConsumeTimer, FuelConsumeDelegate, 1.f, true);
}

void AA1SpaceshipBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (HasAuthority() && !bGameEndHandled)
	{
		if (CurrentFuelAmount <= 0.0f)
		{
			HandleGameOver();
		}

		if (bMeetRescueShip)
		{
			HandleRescue();
		}
	}
}

void AA1SpaceshipBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AA1SpaceshipBase, CurrentFuelAmount);
	DOREPLIFETIME(AA1SpaceshipBase, bIsExternalMapActive);
	DOREPLIFETIME(AA1SpaceshipBase, GameEndState);
	DOREPLIFETIME(AA1SpaceshipBase, bMeetRescueShip);

}

void AA1SpaceshipBase::RegisterDoor(AA1DoorBase* Door)
{
	if (Door && HasAuthority())
	{
		CacheDoor = Door;
	}
}

void AA1SpaceshipBase::RegisterDockingSignalHandler(AA1DockingSignalHandlerBase* Signal)
{
	if (Signal && HasAuthority() && !DockingSignalHandler)
	{
		DockingSignalHandler = Signal;
	}
}

void AA1SpaceshipBase::RegisterBed(AA1BedBase* Bed)
{
	if (Bed && HasAuthority())
	{
		Beds.AddUnique(Bed);
	}
}

void AA1SpaceshipBase::RegisterFuelSystem(AA1FuelBase* Fuel)
{
	if (Fuel && HasAuthority() && !FuelSystem)
	{
		FuelSystem = Fuel;
	}
}

void AA1SpaceshipBase::RegisterStorage(AA1StorageBase* Storage)
{
	if (Storage && HasAuthority())
	{
		Storages.AddUnique(Storage);
	}
}

void AA1SpaceshipBase::RegisterShipOutput(AA1ShipOutputBase* Output)
{
	if (Output && HasAuthority() && !ShipOutput)
	{
		ShipOutput = Output;
	}
}

void AA1SpaceshipBase::RegisterSignalDetection(AA1SignalDetectionBase* Output)
{
	if (Output && HasAuthority() && !SignalDetection)
	{
		SignalDetection = Output;
	}
}

void AA1SpaceshipBase::HandleGameOver()
{
	if (!HasAuthority() || bGameEndHandled)
		return;

	bGameEndHandled = true;
	GameEndState = EGameEndState::GameOver;

	OnGameEndEvent.Broadcast(GameEndState);

	GetWorldTimerManager().ClearTimer(FuelConsumeTimer);

	
	if (ALyraGameMode* GameMode = Cast<ALyraGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->HandleGameEnd(this, false);
	}
}

void AA1SpaceshipBase::HandleRescue()
{
	if (!HasAuthority() || bGameEndHandled)
		return;

	bGameEndHandled = true;

	GameEndState = EGameEndState::Rescued;

	OnGameEndEvent.Broadcast(GameEndState);

	GetWorldTimerManager().ClearTimer(FuelConsumeTimer);

	if (ALyraGameMode* GameMode = Cast<ALyraGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->HandleGameEnd(this, true);
	}
}

bool AA1SpaceshipBase::IsRescued() const
{
	return GameEndState == EGameEndState::Rescued;
}

void AA1SpaceshipBase::SetMeetRescueShip(bool bMeetRescue)
{
	if (HasAuthority() && !bGameEndHandled)
	{
		bMeetRescueShip = bMeetRescue;

		if (bMeetRescueShip)
		{
			HandleRescue();
		}
	}
}

void AA1SpaceshipBase::FindSpaceshipComponents()
{
	if (!HasAuthority())
		return;

	// 이미 할당된 참조가 있으면 재할당하지 않음
	if (!DockingSignalHandler)
	{
		TArray<AActor*> FoundDockingSignalHandlers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1DockingSignalHandlerBase::StaticClass(), FoundDockingSignalHandlers);
		if (FoundDockingSignalHandlers.Num() > 0)
		{
			DockingSignalHandler = Cast<AA1DockingSignalHandlerBase>(FoundDockingSignalHandlers[0]);
		}
	}

	// Doors가 비어있는 경우에만 찾기
	if (!CacheDoor)
	{
		TArray<AActor*> FoundDoor;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1DoorBase::StaticClass(), FoundDoor);
		if (FoundDoor.Num() > 0)
		{
			CacheDoor = Cast<AA1DoorBase>(FoundDoor[0]);
		}

	}

	if (!FuelSystem)
	{
		TArray<AActor*> FoundFuelSystems;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1FuelBase::StaticClass(), FoundFuelSystems);
		if (FoundFuelSystems.Num() > 0)
		{
			FuelSystem = Cast<AA1FuelBase>(FoundFuelSystems[0]);
		}
	}

	if (!ShipOutput)
	{
		TArray<AActor*> FoundShipOutputs;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1ShipOutputBase::StaticClass(), FoundShipOutputs);
		if (FoundShipOutputs.Num() > 0)
		{
			ShipOutput = Cast<AA1ShipOutputBase>(FoundShipOutputs[0]);
		}
	}

	if (Beds.IsEmpty())
	{
		TArray<AActor*> FoundBeds;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1BedBase::StaticClass(), FoundBeds);
		for (AActor* Actor : FoundBeds)
		{
			if (AA1BedBase* Bed = Cast<AA1BedBase>(Actor))
			{
				Beds.AddUnique(Bed);
			}
		}
	}

	if (Storages.IsEmpty())
	{
		TArray<AActor*> FoundStorages;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1StorageBase::StaticClass(), FoundStorages);
		for (AActor* Actor : FoundStorages)
		{
			if (AA1StorageBase* Storage = Cast<AA1StorageBase>(Actor))
			{
				Storages.AddUnique(Storage);
			}
		}
	}

	if (!SignalDetection)
	{
		TArray<AActor*> FoundStorages;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1SignalDetectionBase::StaticClass(), FoundStorages);
		for (AActor* Actor : FoundStorages)
		{
			if (AA1SignalDetectionBase* FoundSignalDetection = Cast<AA1SignalDetectionBase>(Actor))
			{
				SignalDetection = Cast<AA1SignalDetectionBase>(FoundSignalDetection);
			}
		}
	}
}

void AA1SpaceshipBase::FindComponentsByTags()
{
	if (!HasAuthority())
		return;

	TArray<AActor*> TaggedActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpaceshipComponentTag, TaggedActors);
	for (AActor* Actor : TaggedActors)
	{
		if (!IsValid(Actor))
			continue;
		if (Actor->ActorHasTag(DoorTag))
		{
			CacheDoor = Cast<AA1DoorBase>(Actor);;
		}
		else if (Actor->ActorHasTag(DockingSignalHandlerTag) && !DockingSignalHandler)
		{
			DockingSignalHandler = Cast<AA1DockingSignalHandlerBase>(Actor);
		}
		else if (Actor->ActorHasTag(BedTag))
		{
			AA1BedBase* Bed = Cast<AA1BedBase>(Actor);
			if (Bed)
			{
				Beds.AddUnique(Bed);
			}
		}
		else if (Actor->ActorHasTag(FuelSystemTag) && !FuelSystem)
		{
			FuelSystem = Cast<AA1FuelBase>(Actor);
		}
		else if (Actor->ActorHasTag(StorageTag))
		{
			AA1StorageBase* Storage = Cast<AA1StorageBase>(Actor);
			if (Storage)
			{
				Storages.AddUnique(Storage);
			}
		}
		else if (Actor->ActorHasTag(ShipOutputTag) && !ShipOutput)
		{
			ShipOutput = Cast<AA1ShipOutputBase>(Actor);
		}
	}
}

void AA1SpaceshipBase::OnRep_CurrentFuel()
{
	OnFuelChanged.Broadcast(CurrentFuelAmount);
}

void AA1SpaceshipBase::OnRep_GameEndState()
{
	// 클라이언트에서 게임 종료 이벤트 발생
	OnGameEndEvent.Broadcast(GameEndState);
}

void AA1SpaceshipBase::AddFuel(float AmountToAdd)
{
	if (!HasAuthority()/*Only Server*/)
		return;
	if (AmountToAdd <= 0.f)
		return;

	const float PreviousFuel = CurrentFuelAmount;
	CurrentFuelAmount = FMath::Min(CurrentFuelAmount + AmountToAdd, MaxFuelAmount);

	if (PreviousFuel != CurrentFuelAmount)
	{
		OnRep_CurrentFuel();
	}

}

void AA1SpaceshipBase::ConsumeFuel(float AmountToConsume)
{
	if (!HasAuthority()/*Only Server*/)
		return;

	if (AmountToConsume <= 0.f)
		return;

	const float PreviousFuel = CurrentFuelAmount;
	CurrentFuelAmount = FMath::Max(CurrentFuelAmount - AmountToConsume, 0.0f);

	if (PreviousFuel != CurrentFuelAmount)
	{
		OnRep_CurrentFuel();
	}
}

void AA1SpaceshipBase::ActivateExternalMap()
{
	if (!HasAuthority()/*Only Server*/)
		return;

	bIsExternalMapActive = true;

}

void AA1SpaceshipBase::DeactivateExternalMap()
{
	if (!HasAuthority()/*Only Server*/)
		return;

	bIsExternalMapActive = false;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1RandomMapGenerator::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AA1RandomMapGenerator* TargetActor = Cast<AA1RandomMapGenerator>(Actor);
		if (TargetActor && TargetActor->GetbDungeonGenerateComplete())
		{
			TargetActor->Server_ResetMap();
			DockingSignalHandler->SetSignalState(ESignalState::Released);
			break;
		}
	}
}

bool AA1SpaceshipBase::IsGameOver() const
{
	return GameEndState == EGameEndState::GameOver;
}

bool AA1SpaceshipBase::HasEnoughFuelToSurvive() const
{
	constexpr float MinimumFuelToSurvive = 100.0f;
	return CurrentFuelAmount >= MinimumFuelToSurvive;
}

