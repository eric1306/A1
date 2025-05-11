// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A1RandomMapGenerator.generated.h"

class AA1RoomBridge;
DECLARE_LOG_CATEGORY_EXTERN(LogMap, Log, All);

// Network Log Macro
#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT_ID%d"), static_cast<int32>(GPlayInEditorID)) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER")))
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
#define MAP_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] %s %s"), LOG_NETMODEINFO, LOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))

//forward declare
class AA1MasterRoom;
class AA1EndWall;

UCLASS()
class A1GAME_API AA1RandomMapGenerator : public AActor
{
    GENERATED_BODY()

public:
    AA1RandomMapGenerator();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

    //Call by GA_Interaction_RescueSignal
    UFUNCTION(BlueprintImplementableEvent)
    void StartRandomMap();

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void Server_SetSeed();

    UFUNCTION(Server, Reliable)
    void Server_SpawnStartRoom();

    UFUNCTION(Server, Reliable)
    void Server_StartDungeonTimer();

    UFUNCTION(Server, Reliable)
    void Server_SpawnNextRoom();

    UFUNCTION(Server, Reliable)
    void Server_CheckDungeonComplete();

    UFUNCTION(Server, Reliable)
    void Server_AddOverlappingRoomsFromList();

    UFUNCTION(Server, Reliable)
    void Server_CheckForOverlap();

    UFUNCTION(Server,Reliable)
    void Server_ResetMap();

    UFUNCTION(Server, Reliable)
    void Server_ResetAndRegenerateMap();

    UFUNCTION(Server, Reliable)
    void Server_CloseHoles();

    UFUNCTION(Server, Reliable)
    void Server_EnableReplicationForAllActors();

    UFUNCTION(Server, Reliable)
    void Server_SpawnEnemy();

    UFUNCTION(Server, Reliable)
    void Server_SpawnItem();

    //RPC Functions

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlaySiren();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_StopSiren();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_CloseHoles();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnDungeonGenerateComplete();

    //Client Verify functions
    UFUNCTION(Client, Reliable)
    void Client_VerifyMapGeneration();

    UFUNCTION(Server, Reliable)
    void Server_CheckAndRepairRooms();

    UFUNCTION(Client, Reliable)
    void Client_RepairInvalidRoom(int32 RoomIndex, FTransform RoomTransform, TSubclassOf<AA1MasterRoom> RoomClass);

    UFUNCTION(Client, Reliable)
    void Client_RepairInvalidWall(int32 WallIndex, FTransform WallTransform);

    uint8 GetbDungeonGenerateComplete() const { return bDungeonGenerateComplete; }

protected:
    void SetupNetworkProperties(AActor* Actor);


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", meta = (AllowPrivateAccess = "true"))
    TArray<TObjectPtr<USceneComponent>> ExitsList;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
    TArray<TSubclassOf<AA1MasterRoom>> RoomList;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Generator", meta = (AllowPrivateAccess = "true"), Replicated)
    TObjectPtr<AA1MasterRoom> LatestRoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", meta = (AllowPrivateAccess = "true"), Replicated)
    int32 RoomAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator", meta = (AllowPrivateAccess = "true"), Replicated)
    int32 MaxRoomAmount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", meta = (AllowPrivateAccess = "true"))
    TArray<TObjectPtr<class UPrimitiveComponent>> OverlappedList;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_DungeonGenerateComplete)
    uint8 bDungeonGenerateComplete : 1;

    UFUNCTION()
    void OnRep_DungeonGenerateComplete();

    UPROPERTY(EditDefaultsOnly, Category = "Generator|StartRoom")
    TSubclassOf<AA1RoomBridge> DockingBridge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator", meta = (AllowPrivateAccess = "true"))
    float MaxDungeonTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator", meta = (AllowPrivateAccess = "true"), Replicated)
    int32 Seed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", meta = (AllowPrivateAccess = "true"), Replicated)
    FRandomStream Stream;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> SelectedExitPoint;

    //For Replication
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", Replicated)
    TArray<TObjectPtr<AA1MasterRoom>> SpawnedRooms;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", Replicated)
    TArray<TObjectPtr<class AA1EndWall>> SpawnedEndWalls;

    //For Recovery
    UPROPERTY(Replicated)
    TArray<FTransform> RoomTransforms;

    UPROPERTY(Replicated)
    TArray<FString> RoomClassPaths;

    UPROPERTY(Replicated)
    TArray<FTransform> WallTransforms;

    //Check Server Make Random Map
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generator", meta = (AllowPrivateAccess = "true"))
    uint8 bIsServerTraveling : 1;

    //Map Reset Flag
    UPROPERTY(Replicated)
    uint8 bIsResettingMap : 1;

    UPROPERTY(Replicated)
    int32 ExpectedExitCount;

    UPROPERTY(Replicated)
    int32 ExpectedEndWallCount;

    //Verify Check Flag
    UPROPERTY(Replicated)
    uint8 bVerificationComplete : 1;

    UPROPERTY(EditDefaultsOnly, Category = "SFX")
    TObjectPtr<USoundBase> SirenSound;

    UPROPERTY()
    TObjectPtr<UAudioComponent> AudioComp;

    UPROPERTY(EditDefaultsOnly, Category = "Generator")
    TSubclassOf<AA1EndWall> EndWallClass;

private:
    FTimerHandle GenerateTimer;
    FTimerHandle RepairTimer;

    float StartTimer;

    int32 RepairAttempts;
    static const int32 MaxRepairAttempts = 3;
};
