// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "A1TutorialManager.generated.h"

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

// Ʃ�丮�� �׼� Ÿ�� - ��� ������ ������ ����
UENUM(BlueprintType)
enum class ETutorialActionType : uint8
{
    None,
    FixCamera,          // ī�޶� ����
    PlayVideo,          // ���� ���
    ShowMessage,        // �޽��� ǥ��
    HighlightActors,    // ���͵� ���̶���Ʈ
    SpawnEffects,       // ����Ʈ ����
    PlaySound,          // ���� ���
    WaitForCondition,   // ���� ���
    ChangeLevel,        // ���� ����
    FadeScreen,         // ȭ�� ���̵�
    Custom              // Ŀ���� �׼� (Blueprint ����)
};

// Ʃ�丮�� ���� ������ - ��� ���� ������ ��� ���� ����ü
USTRUCT(BlueprintType)
struct FA1TutorialStepData : public FTableRowBase
{
    GENERATED_BODY()

    // �⺻ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText StepName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    // ������ �׼ǵ� (���� ����)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ETutorialActionType> Actions;

    // �׼Ǻ� �Ķ���� (JSON ���ڿ��� ����)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<ETutorialActionType, FString> ActionParams;

    // �Ϸ� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag CompletionCondition;

    // �ڵ� ���� �ð� (0�̸� ���� ���)
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
    // �ٽ� ���
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

    // ���� ���� ��ȸ
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

protected:
    // �׼� ���� �Լ��� - �� �׼� Ÿ�Ժ� ó��
    void ExecuteAction(ETutorialActionType ActionType, const FString& Params);

    void DoPlayVideo(const FString& Params);
    void DoShowMessage(const FString& Params);
    void DoHighlightActors(const FString& Params);
    void DoSpawnEffects(const FString& Params);
    void DoPlaySound(const FString& Params);
    void DoWaitForCondition(const FString& Params);
    void DoChangeLevel(const FString& Params);
    UFUNCTION(BlueprintImplementableEvent)
    void DoFadeScreen(const FString& Params);

    UFUNCTION()
    void OnItemPickedUp();


    // ��ƿ��Ƽ �Լ���
    FVector ParseVector(const FString& VectorString);
    FRotator ParseRotator(const FString& RotatorString);
    TArray<FString> ParseStringArray(const FString& ArrayString);

    // Blueprint Ȯ�� ����Ʈ
    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void OnStepStarted(const FA1TutorialStepData& StepData);

    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void OnStepCompleted(int32 CompletedStepIndex);

    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void OnTutorialCompleted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void ExecuteCustomAction(const FString& Params);

protected:
    // ���� ������
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    TObjectPtr<UDataTable> TutorialStepsTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    TSubclassOf<UUserWidget> UIWidgetClass;

    // ��Ÿ�� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tutorial")
    int32 CurrentStepIndex = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tutorial")
    bool bIsActive = false;

    // ĳ�õ� ������
    UPROPERTY()
    TObjectPtr<ALyraPlayerController> PlayerController;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UUserWidget> UIWidget;

    uint32 ItemCount = 0;

    // ��Ÿ�� ������
    TArray<FA1TutorialStepData*> StepDataArray;
    FGameplayTag CurrentWaitingCondition;

    // Ÿ�̸�
    FTimerHandle AutoProgressTimer;
    FTimerHandle ActionDelayTimer;
};

// Ʃ�丮�� ��ȣ�ۿ� ������Ʈ - ���� ���Ϳ� �߰��� �ϸ� �Ǵ� ������Ʈ
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class A1GAME_API UA1TutorialInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UA1TutorialInteractionComponent();

protected:
    virtual void BeginPlay() override;

public:
    // ��ȣ�ۿ� ó��
    UFUNCTION(BlueprintCallable, Category = "Tutorial Interaction")
    void OnInteracted();

    UFUNCTION(BlueprintCallable, Category = "Tutorial Interaction")
    void TriggerCondition();

    // ���̶���Ʈ ����
    UFUNCTION(BlueprintCallable, Category = "Tutorial Interaction")
    void SetHighlight(bool bEnabled);

    // ���� ����
    UFUNCTION(BlueprintCallable, Category = "Tutorial Interaction")
    void SetInteractionEnabled(bool bEnabled) { bIsEnabled = bEnabled; }

protected:
    // ����
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

// ������ Ʃ�丮�� ������ Blueprint �Լ� ���̺귯��
UCLASS()
class A1GAME_API UA1TutorialBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ���� ���� �Լ���
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

    // ���� üũ ����
    UFUNCTION(BlueprintCallable, Category = "Tutorial")
    static void TriggerTutorialCondition(UObject* WorldContext, FGameplayTag ConditionTag);
};
