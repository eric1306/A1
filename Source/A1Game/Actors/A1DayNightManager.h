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
//�ð� ���� �˸� ��������Ʈ
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

	// ���� �ð� ���� ȹ��
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	FORCEINLINE EDayPhase GetCurrentPhase() const { return CurrentPhase; }

	// ���� �ϼ� ȹ��
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	FORCEINLINE int32 GetCurrentDay() const { return CurrentDay; }

	// �Ϸ� �� ����� �ð� ���� (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	FORCEINLINE float GetDayProgress() const { return DayProgress; }

	// �� ��ŵ �õ�
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void TrySkipNight();

	// �÷��̾� ���� ���� ����
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void SetPlayerSleeping(AActor* Player, bool bIsSleeping);

	// ���� ���� �� �ð��� ��ȯ�ϴ� �Լ� (24�ð� ����)
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void GetGameTime(int32& OutHours, int32& OutMinutes) const;

	// ���� �ð��� ���ڿ��� ��ȯ
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	FString GetTimeString() const;

	// ��� �÷��̾� ����� �޼���
	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void WakeAllPlayers();

protected:
	// �ð� ������Ʈ �Լ�
	void UpdateTime(float DeltaTime);

	// �ð� ���� ���� �Լ�
	UFUNCTION()
	void ChangePhase(EDayPhase NewPhase);

	// ��� �÷��̾ �ڰ� �ִ��� Ȯ��
	bool AreAllPlayersSleeping() const;

	// ���ø����̼� �̺�Ʈ
	UFUNCTION()
	void OnRep_CurrentPhase();

	UFUNCTION()
	void OnRep_CurrentDay();

	UFUNCTION(BlueprintCallable, Category = "Server|Players", meta = (WorldContext = "WorldContextObject"))
	int32 GetActivePlayerCount(const UObject* WorldContextObject) const;

public:
	// ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnDayPhaseChanged OnDayPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnDayChanged OnDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnTimeChanged OnTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "DayNight")
	FOnWakeUpPlayers OnWakeUpPlayers;

protected:
	// �ð� ���� ����
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase)
	EDayPhase CurrentPhase;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentDay)
	int32 CurrentDay;

	UPROPERTY(Replicated)
	float DayProgress;

	// ������
	UPROPERTY(EditDefaultsOnly, Category = "DayNight Settings")
	float DayDurationMinutes;

	UPROPERTY(EditDefaultsOnly, Category = "DayNight Settings")
	float DayPhaseDurationMinutes;

	// ���� ����
	float ElapsedTime;
	float PhaseChangeDuration;

	// �̱��� �ν��Ͻ�
	static AA1DayNightManager* DayNightInstance;

	// �÷��̾� ���� ���� ����
	UPROPERTY()
	TMap<AActor*, bool> SleepingPlayers;

	// ��� ���� ħ�� ���
	UPROPERTY()
	TArray<TObjectPtr<AA1BedBase>> OccupiedBeds;

	// ���������� ������Ʈ�� �ð�
	int32 LastUpdatedHour;
	int32 LastUpdatedMinute;

	bool SkipNightflag = false;
};