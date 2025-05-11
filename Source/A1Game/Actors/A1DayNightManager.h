// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A1DayNightManager.generated.h"

class AA1BedBase;

UENUM(BlueprintType)
enum class EDayPhase : uint8
{
	Day,
	Night
};
//시간 변경 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDayPhaseChanged, EDayPhase, NewPhase, int32, CurrentDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChanged, int32, NewDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeChanged, int32, Hour, int32, Minute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWakeUpPlayers);

UCLASS()
class A1GAME_API AA1DayNightManager : public AActor
{
	GENERATED_BODY()

public:
	AA1DayNightManager();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;

	//Get Singleton Instance
	static AA1DayNightManager* Get(UWorld* World);

	// 현재 시간 상태 획득
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	FORCEINLINE EDayPhase GetCurrentPhase() const { return CurrentPhase; }

	// 현재 일수 획득
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	FORCEINLINE int32 GetCurrentDay() const { return CurrentDay; }

	// 하루 중 진행된 시간 비율 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	FORCEINLINE float GetDayProgress() const { return DayProgress; }

	// 밤 스킵 시도
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void TrySkipNight();

	// 플레이어 수면 상태 설정
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void SetPlayerSleeping(AActor* Player, bool bIsSleeping);

	// 현재 게임 내 시간을 반환하는 함수 (24시간 형식)
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void GetGameTime(int32& OutHours, int32& OutMinutes) const;

	// 현재 시간을 문자열로 반환
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	FString GetTimeString() const;

	// 모든 플레이어 깨우기 메서드
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void WakeAllPlayers();

protected:
	// 시간 업데이트 함수
	void UpdateTime(float DeltaTime);

	// 시간 상태 변경 함수
	UFUNCTION()
	void ChangePhase(EDayPhase NewPhase);

	// 모든 플레이어가 자고 있는지 확인
	bool AreAllPlayersSleeping() const;

	// 리플리케이션 이벤트
	UFUNCTION()
	void OnRep_CurrentPhase();

	UFUNCTION()
	void OnRep_CurrentDay();

	UFUNCTION(BlueprintCallable, Category = "Server|Players", meta = (WorldContext = "WorldContextObject"))
	int32 GetActivePlayerCount(const UObject* WorldContextObject) const;

public:
	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnDayPhaseChanged OnDayPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnDayChanged OnDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnTimeChanged OnTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnWakeUpPlayers OnWakeUpPlayers;

protected:
	// 시간 관련 변수
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase)
	EDayPhase CurrentPhase;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentDay)
	int32 CurrentDay;

	UPROPERTY(Replicated)
	float DayProgress;

	// 설정값
	UPROPERTY(EditDefaultsOnly, Category = "DayNight Settings")
	float DayDurationMinutes;

	UPROPERTY(EditDefaultsOnly, Category = "DayNight Settings")
	float DayPhaseDurationMinutes;

	// 내부 변수
	float ElapsedTime;
	float PhaseChangeDuration;

	// 싱글톤 인스턴스
	static AA1DayNightManager* DayNightInstance;

	// 플레이어 수면 상태 추적
	UPROPERTY()
	TMap<AActor*, bool> SleepingPlayers;

	// 사용 중인 침대 목록
	UPROPERTY()
	TArray<TObjectPtr<AA1BedBase>> OccupiedBeds;

	// 마지막으로 업데이트된 시간
	int32 LastUpdatedHour;
	int32 LastUpdatedMinute;

	bool SkipNightflag = false;
};