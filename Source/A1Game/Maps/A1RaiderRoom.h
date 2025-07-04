// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Maps/A1MasterRoom.h"
#include "A1RaiderRoom.generated.h"

class UDynamicMeshComponent;
class AA1CreatureBase;
class AA1ChestBase;
class AA1EquipmentBase;
class AA1RaiderBase;

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Rounge,
	Storage,
	Container,
	Master,
	Bridge,
	LBridge,
	BedRoom,
	SecondFloor
};
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
	TSubclassOf<AA1CreatureBase> EnemyClass;
	int32 ItemIndex;

	FSpawnQueueItem()
	{
		Type = Enemy;
		Transform = FTransform::Identity;
		EnemyClass = nullptr;
		ItemIndex = 0;
	}

	FSpawnQueueItem(ESpawnType InType, const FTransform& InTransform, TSubclassOf<AA1CreatureBase> InEnemyClass = nullptr, int32 InItemIndex = 0)
		: Type(InType), Transform(InTransform), EnemyClass(InEnemyClass), ItemIndex(InItemIndex) {
	}
};

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
	void SpawnEnemies(TSubclassOf<AA1CreatureBase> CreatureClass);

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy(FTransform SpawnTransform, TSubclassOf<AA1CreatureBase> EnemyClass);

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

	UFUNCTION(BlueprintImplementableEvent)
	void MakeCliff();

	void SetRoomType(int32 RoomIndex);

	FORCEINLINE ERoomType GetRoomType() const { return RoomType; }

private:

	// 큐에 추가하는 헬퍼 함수들
	void AddEnemyToQueue(TSubclassOf<AA1CreatureBase> EnemyClass, const FTransform& Transform);
	void AddItemToQueue(int32 ItemIndex);
	void AddChestToQueue(const FTransform& Transform);

	// 큐에서 처리하는 함수
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
	TArray<TObjectPtr<AA1CreatureBase>> SpawnedCreatures;

	UPROPERTY(VisibleAnywhere, Category = "Raider", Replicated)
	TArray<TObjectPtr<AA1EquipmentBase>> SpawnedItems;

	UPROPERTY(VisibleAnywhere, Category = "Raider", Replicated)
	TArray<TObjectPtr<AA1ChestBase>> SpawnedChests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class UA1ItemTemplate>> CachedItemTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicMesh")
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RoomType")
	ERoomType RoomType = ERoomType::Rounge;

private:
	// 스폰 큐
	TQueue<FSpawnQueueItem> SpawnQueue;

	// 프레임 시간 체크
	float LastFrameTime = 0.0f;
	float TargetFrameTime = 0.016f; // 60 FPS 기준

	// 스폰 간격
	UPROPERTY(EditAnywhere, Category = "Spawn Optimization")
	float SpawnInterval = 0.3f; // 0.1초마다 큐 처리

	float SpawnTimer = 0.0f;

	// 틱당 스폰 개수
	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 SpawnPerTick = 1;
};
