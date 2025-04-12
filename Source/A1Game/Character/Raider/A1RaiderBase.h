// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "A1RaiderBase.generated.h"

UCLASS()
class A1GAME_API AA1RaiderBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AA1RaiderBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
