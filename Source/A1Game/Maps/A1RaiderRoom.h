// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Maps/A1MasterRoom.h"
#include "A1RaiderRoom.generated.h"

USTRUCT()
struct FSpawnQueueItem
{
	GENERATED_BODY()

	enum ESpawnType
	{
		Enemy,
		Item,
		Chest
	};

	ESpawnType Type;
	FTransform Transform;
	int32 ItemIndex;

	FSpawnQueueItem()
	{
		Type = Enemy;
		Transform = FTransform::Identity;
		ItemIndex = 0;
	}

	FSpawnQueueItem(ESpawnType InType, const FTransform& InTransform, int32 InItemIndex = 0)
		: Type(InType), Transform(InTransform), ItemIndex(InItemIndex) {
	}
};

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
	virtual void Tick(float DeltaSeconds) override;
	UFUNCTION(BlueprintCallable)
	void SpawnEnemys();

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy(FTransform SpawnTransform);

	UFUNCTION(BlueprintCallable)
	void SpawnItems();

	UFUNCTION(BlueprintCallable)
	void SpawnItem(int32 idx);

	UFUNCTION(BlueprintCallable)
	void SpawnChest(FTransform SpawnTransform);

	UFUNCTION(BlueprintCallable)
	void RemoveEnemy();

	UFUNCTION(BlueprintCallable)
	void RemoveItem();

	UFUNCTION(BlueprintCallable)
	void RemoveChest();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnPlundererSpawner();

private:

	// ť�� �߰��ϴ� ���� �Լ���
	void AddEnemyToQueue(const FTransform& Transform);
	void AddItemToQueue(int32 ItemIndex);
	void AddChestToQueue(const FTransform& Transform);

	// ť���� ó���ϴ� �Լ�
	void ProcessSpawnQueue();

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

private:
	// ���� ť
	TQueue<FSpawnQueueItem> SpawnQueue;

	// ���� ���� ����
	UPROPERTY(EditAnywhere, Category = "Spawn Optimization")
	int32 MinSpawnPerTick = 1;

	UPROPERTY(EditAnywhere, Category = "Spawn Optimization")
	int32 MaxSpawnPerTick = 5;

	// ������ �ð� üũ
	float LastFrameTime = 0.0f;
	float TargetFrameTime = 0.016f; // 60 FPS ����

	// ���� ����
	UPROPERTY(EditAnywhere, Category = "Spawn Optimization")
	float SpawnInterval = 0.1f; // 0.1�ʸ��� ť ó��

	float SpawnTimer = 0.0f;

	// ƽ�� ���� ����
	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 SpawnPerTick = 5;
};
