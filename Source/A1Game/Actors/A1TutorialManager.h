// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "A1TutorialManager.generated.h"

class AA1EquipmentBase;
class UMediaSource;
class UMediaPlayer;
class UA1TutorialHelperWidget;
class ALyraPlayerController;
class FText;

USTRUCT(BlueprintType)
struct FA1TutorialUIMessage
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    FText Title;

    UPROPERTY(BlueprintReadWrite)
    FText Content;
};

// 튜토리얼 액션 타입 - 모든 가능한 동작을 정의
UENUM(BlueprintType)
enum class ETutorialActionType : uint8
{
    None,
    FixCamera,          // 카메라 고정
    PlayVideo,          // 영상 재생
    ShowMessage,        // 메시지 표시
    HighlightActors,    // 액터들 하이라이트
    SpawnEffects,       // 이펙트 스폰
    PlaySound,          // 사운드 재생
    CheckStorage,       // 창고에 아이템 유무 검사
    WaitForCondition,   // 조건 대기
    ChangeLevel,        // 레벨 변경
    FadeScreen,         // 화면 페이드
    Custom              // 커스텀 액션 (Blueprint 구현)
};

UENUM(BlueprintType)
enum class ETutorialStep : uint8
{
    None,           
	VideoPlayBack,      //비디오 재생
    CarryItems,         //아이템 옮기기
    Emergency,          //비상 사태
    Repair,             //수리
    Store,              //창고에 아이템 저장
    Escape,             //탈출
    Collapse,           //붕괴
    End                 //튜토리얼 종료
};

// 튜토리얼 스텝 데이터 - 모든 스텝 정보를 담는 단일 구조체
USTRUCT(BlueprintType)
struct FA1TutorialStepData : public FTableRowBase
{
    GENERATED_BODY()

    // 기본 정보
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText StepName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    // 실행할 액션들 (순차 실행)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ETutorialActionType> Actions;

    // 액션별 파라미터 (JSON 문자열로 저장)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<ETutorialActionType, FString> ActionParams;

    // 완료 조건
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag CompletionCondition;

    // 자동 진행 시간 (0이면 조건 대기)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AutoProgressTime = 0.0f;

    FA1TutorialStepData()
    {
        StepName = FText::GetEmpty();
        Description = FText::GetEmpty();
        AutoProgressTime = 0.0f;
    }
};

UCLASS()
class AA1TutorialManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AA1TutorialManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
    // 핵심 기능
    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void StartTutorial();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void NextStep();

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void GoToStep(int32 StepIndex);

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void OnConditionMet(FGameplayTag ConditionTag);

    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    void EndTutorial();

    // 현재 상태 조회
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
    int32 GetCurrentStep() const { return CurrentStepIndex; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
    bool IsActive() const { return bIsActive; }

    UFUNCTION(BlueprintCallable)
    void SendUIMessage(FText Title, FText Content);

    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void PlayTutorialVideo();

    UFUNCTION(BlueprintCallable)
    FORCEINLINE ALyraPlayerController* GetPlayerController() { return PlayerController; }

    UFUNCTION()
    void OnItemFill(AA1EquipmentBase* CachedItem);

protected:
    // 액션 실행 함수들 - 각 액션 타입별 처리
    void ExecuteAction(ETutorialActionType ActionType, const FString& Params);
    void ChangeTutorialStep(ETutorialStep CurrentStep);
    FORCEINLINE void SetTutorialStep(ETutorialStep InStep) { TutorialStep = InStep; }

    void DoPlayVideo(const FString& Params);
    void DoShowMessage(const FString& Params);
    void DoHighlightActors(const FString& Params);
    void DoSpawnEffects(const FString& Params);
    void DoPlaySound(const FString& Params);
    void DoCheckStorage(const FString& Params);
    void DoWaitForCondition(const FString& Params);
    void DoChangeLevel(const FString& Params);
    UFUNCTION(BlueprintImplementableEvent)
    void DoFadeScreen(const FString& Params);

    UFUNCTION()
    void OnItemPickedUp();

    UFUNCTION()
    void OnRepaired();

    UFUNCTION(BlueprintImplementableEvent)
    void OpenWidget();


    // 유틸리티 함수들
    FVector ParseVector(const FString& VectorString);
    FRotator ParseRotator(const FString& RotatorString);
    TArray<FString> ParseStringArray(const FString& ArrayString);

    // Blueprint 확장 포인트
    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void OnStepStarted(const FA1TutorialStepData& StepData);

    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void OnStepCompleted(int32 CompletedStepIndex);

    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void OnTutorialCompleted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void ExecuteCustomAction(const FString& Params);

protected:
    // 설정 데이터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    TObjectPtr<UDataTable> TutorialStepsTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    TSubclassOf<UUserWidget> UIWidgetClass;

    // 런타임 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tutorial")
    int32 CurrentStepIndex = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tutorial")
    bool bIsActive = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tutorial")
    ETutorialStep TutorialStep;

    // 캐시된 참조들
    UPROPERTY()
    TObjectPtr<ALyraPlayerController> PlayerController;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UUserWidget> UIWidget;

    uint32 ItemCount = 0;
    uint32 RepairCount = 0;
    int32 ItemStoredCount = 0;

    // 런타임 데이터
    TArray<FA1TutorialStepData*> StepDataArray;
    FGameplayTag CurrentWaitingCondition;

    // 타이머
    FTimerHandle AutoProgressTimer;
    FTimerHandle ActionDelayTimer;
};

// 튜토리얼 상호작용 컴포넌트 - 기존 액터에 추가만 하면 되는 컴포넌트
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class A1GAME_API UA1TutorialInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UA1TutorialInteractionComponent();

protected:
    virtual void BeginPlay() override;

public:
    // 상호작용 처리
    UFUNCTION(BlueprintCallable, Category = "Tutorial Interaction")
    void OnInteracted();

    UFUNCTION(BlueprintCallable, Category = "Tutorial Interaction")
    void TriggerCondition();

    // 하이라이트 제어
    UFUNCTION(BlueprintCallable, Category = "Tutorial Interaction")
    void SetHighlight(bool bEnabled);

    // 상태 설정
    UFUNCTION(BlueprintCallable, Category = "Tutorial Interaction")
    void SetInteractionEnabled(bool bEnabled) { bIsEnabled = bEnabled; }

protected:
    // 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Interaction")
    FGameplayTag TriggerConditionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Interaction")
    bool bIsEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Interaction")
    float InteractionDelay = 0.0f;

private:
    UPROPERTY()
    TObjectPtr<AA1TutorialManager> TutorialManager;

    void FindTutorialManager();
};

// 간단한 튜토리얼 설정용 Blueprint 함수 라이브러리
UCLASS()
class A1GAME_API UA1TutorialBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // 빠른 설정 함수들
    UFUNCTION(BlueprintCallable, Category = "Tutorial Setup", CallInEditor)
    static UDataTable* CreateBasicTutorialData();

    UFUNCTION(BlueprintCallable, Category = "Tutorial Setup")
    static FString MakeVectorParam(FVector Vector);

    UFUNCTION(BlueprintCallable, Category = "Tutorial Setup")
    static FString MakeRotatorParam(FRotator Rotator);

    UFUNCTION(BlueprintCallable, Category = "Tutorial Setup")
    static FString MakeArrayParam(const TArray<FString>& StringArray);

    UFUNCTION(BlueprintCallable, Category = "Tutorial Setup")
    static FString MakeFloatParam(float Value);

    // 조건 체크 헬퍼
    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    static void TriggerTutorialCondition(UObject* WorldContext, FGameplayTag ConditionTag);
};
