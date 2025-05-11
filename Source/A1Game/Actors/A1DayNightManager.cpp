// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1DayNightManager.h"

#include "A1BedBase.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AA1DayNightManager* AA1DayNightManager::DayNightInstance = nullptr;

AA1DayNightManager::AA1DayNightManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	/*
	 * 기본 설정
	 * - 20분 = 1일
	 * - 10분 낫/밤
	 */
	DayDurationMinutes = 1.f;
	DayPhaseDurationMinutes = 0.5f;

	CurrentPhase = EDayPhase::Day;
	CurrentDay = 1;
	DayProgress = 0.f;
	ElapsedTime = 0.f;

	//단위 초 변환
	PhaseChangeDuration = DayPhaseDurationMinutes * 60.f;

	//초기화.
	LastUpdatedHour = -1;
	LastUpdatedMinute = -1;
}

void AA1DayNightManager::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 싱글톤 인스턴스 설정
	if (HasAuthority())
	{
		DayNightInstance = this;

		//World 상의 모든 Bed 등록
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1BedBase::StaticClass(), OUT FoundActors);
		for (auto Actor : FoundActors)
		{
			if (AA1BedBase* BedActor = Cast<AA1BedBase>(Actor))
			{
				OccupiedBeds.Add(BedActor);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Find %d Beds!"), OccupiedBeds.Num());
	}


}

void AA1DayNightManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AA1DayNightManager, CurrentPhase);
	DOREPLIFETIME(AA1DayNightManager, CurrentDay);
	DOREPLIFETIME(AA1DayNightManager, DayProgress);
}

void AA1DayNightManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 서버에서만 시간 업데이트
	if (!HasAuthority())
		return;
	
	UpdateTime(DeltaTime);

	//서버에서 스킵 조건 체크
	// - 모든 플레이어가 자고 있다면 스킵
	if (!SkipNightflag && AreAllPlayersSleeping()) 
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this]()
			{
				TrySkipNight();
			}, 10.f, false); //TODO eric1306 Original Value : 10.f(10 second)
		SkipNightflag = true;
	}
	

}

AA1DayNightManager* AA1DayNightManager::Get(UWorld* World)
{
	if (DayNightInstance == nullptr) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, AA1DayNightManager::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0) {
			DayNightInstance = Cast<AA1DayNightManager>(FoundActors[0]);
		}
	}

	return DayNightInstance;
}

void AA1DayNightManager::TrySkipNight()
{
	if (!HasAuthority()) {
		return;
	}

	// 모든 플레이어가 자고 있는지 확인
	if (AreAllPlayersSleeping()) {
		if (CurrentPhase == EDayPhase::Day)
		{
			// 낮 스킵: 밤으로 이동
			ElapsedTime = DayDurationMinutes * 30.f;
			DayProgress = 0.5f;
			CurrentPhase = EDayPhase::Night;
		}
		else
		{
			// 밤 스킵: 다음 날 아침으로 이동
			ElapsedTime = 0.0f;
			DayProgress = 0.0f;
			CurrentDay++;
			CurrentPhase = EDayPhase::Day;
		}

		// 이벤트 발생
		OnDayChanged.Broadcast(CurrentDay);
		OnDayPhaseChanged.Broadcast(CurrentPhase, CurrentDay);

		// 모든 플레이어 깨우기
		WakeAllPlayers();
	}
}

void AA1DayNightManager::SetPlayerSleeping(AActor* Player, bool bIsSleeping)
{
	if (!HasAuthority() || Player == nullptr) {
		return;
	}
	if (SleepingPlayers.Find(Player))
	{
		SleepingPlayers[Player] = bIsSleeping;
	}
	else
	{
		SleepingPlayers.Add(Player, bIsSleeping);
	}
}

void AA1DayNightManager::GetGameTime(int32& OutHours, int32& OutMinutes) const
{
	// 전체 하루(24시간) 중 진행된 비율을 기반으로 시간 계산
	constexpr float TotalDayMinutes = 24.0f * 60.0f; // 하루의 총 분 수
	const float ElapsedMinutes = DayProgress * TotalDayMinutes;

	// 시간과 분 계산
	OutHours = FMath::FloorToInt(ElapsedMinutes / 60.0f);
	OutMinutes = FMath::FloorToInt(FMath::Fmod(ElapsedMinutes, 60.0f));
}

FString AA1DayNightManager::GetTimeString() const
{
	int32 Hours, Minutes;
	GetGameTime(Hours, Minutes);

	return FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);

}

void AA1DayNightManager::WakeAllPlayers()
{
	// 서버에서만 실행
	if (!HasAuthority()) {
		return;
	}

	// 모든 플레이어의 수면 상태를 깨어난 상태로 변경
	for (auto& Pair : SleepingPlayers) {
		Pair.Value = false;
	}

	// 등록된 모든 침대의 캐릭터 깨우기
	for (AA1BedBase* Bed : OccupiedBeds)
	{
		if (Bed && Bed->GetBedState() == EBedState::Occupied)
		{
			//Bed->Multicast_Wakeup();
			//Temp Code eric1306 for standalone
			Bed->WakeUpOccupyingCharacter();
		}
	}

	// 깨우는 이벤트 발생 - 다른 컴포넌트들을 위한 알림
	OnWakeUpPlayers.Broadcast();

	//재설정 가능하게 하기 위한 flag값 설정
	SkipNightflag = false;
}

void AA1DayNightManager::UpdateTime(float DeltaTime)
{
	//Add Time
	ElapsedTime += DeltaTime;
	DayProgress = ElapsedTime / (DayDurationMinutes * 60.f);

	//하루 지나면 초기화
	if (DayProgress >= 1.f)
	{
		ElapsedTime = 0.f;
		DayProgress = 0.f;
		CurrentDay++;
		OnDayChanged.Broadcast(CurrentDay);
	}

	// 낮/밤 전환 체크
	if (CurrentPhase == EDayPhase::Day && ElapsedTime >= PhaseChangeDuration)
	{
		ChangePhase(EDayPhase::Night);
	}
	else if (CurrentPhase == EDayPhase::Night && ElapsedTime >= PhaseChangeDuration * 2.0f) {
		ChangePhase(EDayPhase::Day);
	}

	// 시간 업데이트 확인 및 이벤트 발생
	int32 CurrentHour, CurrentMinute;
	GetGameTime(CurrentHour, CurrentMinute);

	// 시간이 변경되었을 때만 이벤트 발생
	if (CurrentHour != LastUpdatedHour || CurrentMinute != LastUpdatedMinute)
	{
		LastUpdatedHour = CurrentHour;
		LastUpdatedMinute = CurrentMinute;
		OnTimeChanged.Broadcast(CurrentHour, CurrentMinute);
	}
}

void AA1DayNightManager::ChangePhase(EDayPhase NewPhase)
{
	if (CurrentPhase != NewPhase) {
		CurrentPhase = NewPhase;
		OnDayPhaseChanged.Broadcast(CurrentPhase, CurrentDay);
	}
}

bool AA1DayNightManager::AreAllPlayersSleeping() const
{
	if (SleepingPlayers.Num() == 0 || SleepingPlayers.Num() < GetActivePlayerCount(GetWorld())) {
		return false;
	}
	//UE_LOG(LogTemp, Log, TEXT("Connected Clients: %d"), GetActivePlayerCount(GetWorld()));
	for (const auto& Pair : SleepingPlayers) {
		if (!Pair.Value) {
			return false; // 자고 있지 않은 플레이어가 있음
		}
	}

	return true; // 모든 플레이어가 자고 있음
}

void AA1DayNightManager::OnRep_CurrentPhase()
{
	OnDayPhaseChanged.Broadcast(CurrentPhase, CurrentDay);
}

void AA1DayNightManager::OnRep_CurrentDay()
{
	OnDayChanged.Broadcast(CurrentDay);
}

int32 AA1DayNightManager::GetActivePlayerCount(const UObject* WorldContextObject) const
{
	if (WorldContextObject == nullptr)
		return 0;

	// 컨텍스트 오브젝트로부터 World 얻기
	UWorld* World = WorldContextObject->GetWorld();
	if (!World || World->GetNetMode() == NM_Client)
	{
		return 0;
	}

	AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	if (!GameState)
	{
		return 0;
	}

	return GameState->PlayerArray.Num();
}


