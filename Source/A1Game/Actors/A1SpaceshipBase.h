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

// 게임 상태를 나타내는 열거형
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

// 게임 종료 이벤트를 위한 델리게이트 추가
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

    // 컴포넌트 등록 메서드
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

    // 게임 오버 처리 함수
    UFUNCTION(BlueprintCallable, Category = "Spaceship|GameState", BlueprintAuthorityOnly)
    void HandleGameOver();

    // 구조 처리 함수
    UFUNCTION(BlueprintCallable, Category = "Spaceship|GameState", BlueprintAuthorityOnly)
    void HandleRescue();

    // 구조 상태 확인 함수
    UFUNCTION(BlueprintPure, Category = "Spaceship|GameState")
    bool IsRescued() const;

    // 게임 종료 상태 확인 함수
    UFUNCTION(BlueprintPure, Category = "Spaceship|GameState")
    EGameEndState GetGameEndState() const { return GameEndState; }

    // 게임 상태 확인 함수
    UFUNCTION(BlueprintPure, Category = "Spaceship|GameState")
    bool IsGameOver() const;

    // 연료 관련 함수
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

    // 맵 관련 함수
    UFUNCTION(BlueprintCallable, Category = "Spaceship|ExternalMap", BlueprintAuthorityOnly)
    void ActivateExternalMap();

    UFUNCTION(BlueprintCallable, Category = "Spaceship|ExternalMap", BlueprintAuthorityOnly)
    void DeactivateExternalMap();

    UFUNCTION(BlueprintPure, Category = "Spaceship|ExternalMap")
    bool IsExternalMapActive() const { return bIsExternalMapActive; }

    // 생존 조건 확인
    UFUNCTION(BlueprintCallable, Category = "Spaceship|GameState")
    bool HasEnoughFuelToSurvive() const;

    UFUNCTION()
    void OnRep_CurrentFuel();

    UFUNCTION()
    void OnRep_GameEndState();

    FORCEINLINE bool GetIsExternalMapActive() const { return bIsExternalMapActive; }
    FORCEINLINE void SetIsExternamMapActive(bool InExternalMapActive) { bIsExternalMapActive = InExternalMapActive; }
    FORCEINLINE AA1DoorBase* GetCachedDoor() const { return CacheDoor; }

    // 구조선 만남 여부 설정 함수
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

    // 게임 종료 이벤트
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

    // 게임 종료 상태를 복제하기 위한 변수
    UPROPERTY(BlueprintReadOnly, Category = "Spaceship|GameState", ReplicatedUsing = OnRep_GameEndState)
    EGameEndState GameEndState = EGameEndState::None;

    // 컴포넌트 테그
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

    // 구조선 만남 여부
    UPROPERTY(Replicated)
    bool bMeetRescueShip = false;

private:
    FTimerHandle FuelConsumeTimer;

    // 게임 종료가 이미 처리되었는지 확인하는 플래그
    bool bGameEndHandled = false;
};
