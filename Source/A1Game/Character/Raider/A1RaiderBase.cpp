// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1RaiderBase.h"

#include "A1LogChannels.h"
#include "Controller/Raider/A1RaiderController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1RaiderBase)

// Sets default values
AA1RaiderBase::AA1RaiderBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AA1RaiderController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AA1RaiderBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogA1Raider, Log, TEXT("RaiderBase: Beginplay Call"));
	
}

// Called every frame
void AA1RaiderBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AA1RaiderBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

