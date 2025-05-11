// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "A1SpaceshipBase.generated.h"

class AA1FuelDisplayUI;
class AA1StorageBase;
class AA1ShipOutputBase;
class AA1FuelBase;
class AA1BedBase;
class AA1RescueSignalBase;
class AA1DoorBase;

UENUM(BlueprintType)
enum class ESpaceshipComponentType : uint8
{
    Door,
    RescueSignal,
    Bed,
    Fuel,
    Storage,
    ShipOutput
};

UENUM(BlueprintType)
enum class EGameEndState : uint8
{
    None,
    GameOver,
    Rescued
};

//SpaceshipInterface
UINTERFACE(MinimalAPI, BlueprintType)
class UA1SpaceshipComponent : public UInterface
{
    GENERATED_BODY()
};

class IA1SpaceshipComponent
{
    GENERATED_BODY()

public:
    virtual void RegisterWithSpaceship(class AA1SpaceshipBase* Spaceship) = 0;
    virtual ESpaceshipComponentType GetComponentType() const = 0;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEndEvent, EGameEndState, EndState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFuelChanged, float, NewFuelAmount);

UCLASS()
class AA1SpaceshipBase : public AActor
{
    GENERATED_BODY()
public:
    AA1SpaceshipBase();

protected:
    virtual void BeginPlay() override;
public:
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Components")
    void RegisterDoor(AA1DoorBase* Door);

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Components")
    void RegisterRescueSignal(AA1RescueSignalBase* Signal);

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Components")
    void RegisterBed(AA1BedBase* Bed);

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Components")
    void RegisterFuelSystem(AA1FuelBase* Fuel);

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Components")
    void RegisterStorage(AA1StorageBase* Storage);

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Components")
    void RegisterShipOutput(AA1ShipOutputBase* Output);


    UFUNCTION(BlueprintCallable, Category = "Spaceship|GameState", BlueprintAuthorityOnly)
    void HandleGameOver();

    
    UFUNCTION(BlueprintCallable, Category = "Spaceship|GameState", BlueprintAuthorityOnly)
    void HandleRescue();

    
    UFUNCTION(BlueprintPure, Category = "Spaceship|GameState")
    bool IsRescued() const;

    
    UFUNCTION(BlueprintPure, Category = "Spaceship|GameState")
    EGameEndState GetGameEndState() const { return GameEndState; }

    
    UFUNCTION(BlueprintPure, Category = "Spaceship|GameState")
    bool IsGameOver() const;

    
    UFUNCTION(BlueprintCallable, Category = "Spaceship|Fuel")
    float GetCurrentFuelAmount() const { return CurrentFuelAmount; }

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Fuel")
    float GetMaxFuelAmount() const { return MaxFuelAmount; }

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Fuel")
    float GetFuelPercentage() const { return (MaxFuelAmount > 0.0f) ? (CurrentFuelAmount / MaxFuelAmount) : 0.0f; }

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Fuel", BlueprintAuthorityOnly)
    void AddFuel(float AmountToAdd);

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Fuel", BlueprintAuthorityOnly)
    void ConsumeFuel(float AmountToConsume);

    
    UFUNCTION(BlueprintCallable, Category = "Spaceship|ExternalMap", BlueprintAuthorityOnly)
    void ActivateExternalMap();

    UFUNCTION(BlueprintCallable, Category = "Spaceship|ExternalMap", BlueprintAuthorityOnly)
    void DeactivateExternalMap();

    UFUNCTION(BlueprintPure, Category = "Spaceship|ExternalMap")
    bool IsExternalMapActive() const { return bIsExternalMapActive; }

    
    UFUNCTION(BlueprintCallable, Category = "Spaceship|GameState")
    bool HasEnoughFuelToSurvive() const;

    UFUNCTION()
    void OnRep_CurrentFuel();

    UFUNCTION()
    void OnRep_GameEndState();

    FORCEINLINE bool GetIsExternalMapActive() const { return bIsExternalMapActive; }
    FORCEINLINE void SetIsExternamMapActive(bool InExternalMapActive) { bIsExternalMapActive = InExternalMapActive; }
    FORCEINLINE AA1DoorBase* GetCachedDoor() const { return CacheDoor; }

    UFUNCTION(BlueprintCallable, Category = "Spaceship|GameState", BlueprintAuthorityOnly)
    void SetMeetRescueShip(bool bMeetRescue);

protected:
    UFUNCTION(BlueprintCallable, Category = "Spaceship|Components")
    void FindSpaceshipComponents();

    UFUNCTION(BlueprintCallable, Category = "Spaceship|Components")
    void FindComponentsByTags();

public:
    //Fuel Change Delegate
    UPROPERTY(BlueprintAssignable, Category = "Spaceship|Fuel")
    FOnFuelChanged OnFuelChanged;

    UPROPERTY(BlueprintAssignable, Category = "Spaceship|GameState")
    FOnGameEndEvent OnGameEndEvent;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Fuel")
    float MaxFuelAmount = 200000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Fuel", ReplicatedUsing = OnRep_CurrentFuel)
    float CurrentFuelAmount = 5000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Fuel")
    float FuelConsumptionRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Interactables")
    TObjectPtr<AA1DoorBase> CacheDoor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Interactables")
    TObjectPtr<AA1RescueSignalBase> RescueSignal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Interactables")
    TArray<TObjectPtr<AA1BedBase>> Beds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Interactables")
    TObjectPtr<AA1FuelBase> FuelSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Interactables")
    TArray<TObjectPtr<AA1StorageBase>> Storages;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|Interactables")
    TObjectPtr<AA1ShipOutputBase> ShipOutput;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spaceship|ExternalMap", Replicated)
    bool bIsExternalMapActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "Spaceship|GameState", ReplicatedUsing = OnRep_GameEndState)
    EGameEndState GameEndState = EGameEndState::None;

    UPROPERTY(EditDefaultsOnly, Category = "Spaceship|Tags")
    FName SpaceshipComponentTag = "SpaceshipComponent";

    UPROPERTY(EditDefaultsOnly, Category = "Spaceship|Tags")
    FName DoorTag = "Door";

    UPROPERTY(EditDefaultsOnly, Category = "Spaceship|Tags")
    FName RescueSignalTag = "RescueSignal";

    UPROPERTY(EditDefaultsOnly, Category = "Spaceship|Tags")
    FName BedTag = "Bed";

    UPROPERTY(EditDefaultsOnly, Category = "Spaceship|Tags")
    FName FuelSystemTag = "FuelSystem";

    UPROPERTY(EditDefaultsOnly, Category = "Spaceship|Tags")
    FName FuelDisplayTag = "FuelDisplay";

    UPROPERTY(EditDefaultsOnly, Category = "Spaceship|Tags")
    FName StorageTag = "Storage";

    UPROPERTY(EditDefaultsOnly, Category = "Spaceship|Tags")
    FName ShipOutputTag = "ShipOutput";

    UPROPERTY(Replicated)
    bool bMeetRescueShip = false;

private:
    FTimerHandle FuelConsumeTimer;

    bool bGameEndHandled = false;
};
