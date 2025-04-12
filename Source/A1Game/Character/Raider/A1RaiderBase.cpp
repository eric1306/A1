// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1RaiderBase.h"

// Sets default values
AA1RaiderBase::AA1RaiderBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AA1RaiderBase::BeginPlay()
{
	Super::BeginPlay();
	
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

