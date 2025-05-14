// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Maps/A1MasterRoom.h"
#include "A1RaiderRoom.generated.h"

class AA1ChestBase;
class AA1EquipmentBase;
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();

	UFUNCTION(BlueprintCallable)
	void SpawnItem();

	UFUNCTION(BlueprintCallable)
	void RemoveEnemy();

	UFUNCTION(BlueprintCallable)
	void RemoveItem();

	UFUNCTION(BlueprintCallable)
	void RemoveChest();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|EssentialSpawn")
	TObjectPtr<USceneComponent> EssentialSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|optionalSpawn")
	TObjectPtr<USceneComponent> OptionalSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|ItemSpawnLocations")
	TObjectPtr<USceneComponent> ItemSpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|RemoveItemPos")
	TObjectPtr<USceneComponent> RemoveItemPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|ItemSpawnLocations")
	TArray<TObjectPtr<USceneComponent>> ItemSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|ItemSpawnLocations")
	TObjectPtr<USceneComponent> ItemBoxSpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raider|ItemSpawnLocations")
	TArray<TObjectPtr<USceneComponent>> ItemBoxSpawnLocations;

	UPROPERTY(EditDefaultsOnly, Category = "Raider | Pawn")
	TSubclassOf<AA1RaiderBase> RaiderClass;
	UPROPERTY(EditDefaultsOnly, Category = "Raider | Chest")
	TSubclassOf<AA1ChestBase> ChestClass;

	UPROPERTY(EditDefaultsOnly)
	int32 MinOptionalMonster;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxOptionalMonster;

	UPROPERTY(EditDefaultsOnly)
	int32 MinOptionalItemCount;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxOptionalItemCount;

	UPROPERTY(EditDefaultsOnly)
	int32 SpawnPercentage;

	UPROPERTY(VisibleAnywhere, Category = "Raider", Replicated)
	TArray<TObjectPtr<AA1RaiderBase>> SpawnedRaiders;

	UPROPERTY(VisibleAnywhere, Category = "Raider", Replicated)
	TArray<TObjectPtr<AA1EquipmentBase>> SpawnedItems;

	UPROPERTY(VisibleAnywhere, Category = "Raider", Replicated)
	TArray<TObjectPtr<AA1ChestBase>> SpawnedChests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class UA1ItemTemplate>> CachedItemTemplates;
};
