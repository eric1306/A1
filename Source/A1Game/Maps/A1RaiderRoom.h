// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Maps/A1MasterRoom.h"
#include "A1RaiderRoom.generated.h"

class AA1RaiderBase;
/**
 * 
 */
UCLASS()
class AA1RaiderRoom : public AA1MasterRoom
{
	GENERATED_BODY()
public:
	AA1RaiderRoom(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|EssentialSpawn")
	TObjectPtr<USceneComponent> EssentialSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|optionalSpawn")
	TObjectPtr<USceneComponent> OptionalSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Raider | Pawn")
	TSubclassOf<AA1RaiderBase> RaiderClass;

	UPROPERTY(EditDefaultsOnly)
	int32 MinOptionalMonster;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxOptionalMonster;

	UPROPERTY(EditDefaultsOnly)
	int32 SpawnPercentage;

};
