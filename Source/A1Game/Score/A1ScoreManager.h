// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "A1ScoreData.h"
#include "UObject/NoExportTypes.h"
#include "A1ScoreManager.generated.h"

enum class EGameEndReason : uint8;
class UA1ScoreSaveGame;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, const FA1ScoreData&, CurrentScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, const FA1ScoreData&, FinalScore);

/**
 * 
 */
UCLASS(BlueprintType)
class A1GAME_API UA1ScoreManager : public UObject
{
	GENERATED_BODY()
public:
    // �̱��� �ν��Ͻ� ��������
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score", meta = (CallInEditor = "true"))
    static UA1ScoreManager* Get();


public:
    // �̺�Ʈ ��������Ʈ
    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreChanged OnScoreChanged;

    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnGameEnded OnGameEnded;

public:
    // === ���� ���� ���� ===
    UFUNCTION(BlueprintCallable, Category = "Score")
    void StartNewGame();

    UFUNCTION(BlueprintCallable, Category = "Score")
    void EndGame(EGameEndReason EndReason);

    // === ���� ������ ���� ===
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddDaySurvived(int32 Count = 1);

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddInventoryItems(int32 Count = 1);

    UFUNCTION(BlueprintCallable, Category = "Score")
    void SetInventoryItems(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetStorageItems() const;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void SetStorageItems(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddConsumedItems(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    float GetTotalRepair() const;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void SetTotalRepair(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    float GetCompleteRepair() const;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void SetCompleteRepair(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    void SetRepairedBeforeEscape(bool bRepaired);

    UFUNCTION(BlueprintCallable, Category = "Score")
    void SetRemainingFuel(int32 Fuel);

    // === ������ ����/�ε� ===
    UFUNCTION(BlueprintCallable, Category = "Score")
    bool SaveScores();

    UFUNCTION(BlueprintCallable, Category = "Score")
    bool LoadScores();

    // === ������ �������� ===
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    TArray<FA1ScoreData> GetAllScores() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    int32 GetHighestScore() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    FA1ScoreData GetCurrentScore() const { return CurrentGameScore; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    int32 GetNextGameNumber() const;

    // === ��ƿ��Ƽ ===
    UFUNCTION(BlueprintCallable, Category = "Score")
    void ClearAllData();

    // === ���� �̸����� (���� �� Ȯ�ο�) ===
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    int32 CalculateCurrentScore() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    FString GetCurrentScoreBreakdown() const;

    bool GetDoTutorial() const { return bDoTutorial; }

    UFUNCTION(BlueprintCallable)
    void SetDoTutorial(bool InDoTutorial);

protected:
    // ���� ���� ���� ����
    UPROPERTY(BlueprintReadOnly, Category = "Score")
    FA1ScoreData CurrentGameScore;

    // ����� ��� ����
    UPROPERTY(BlueprintReadOnly, Category = "Score")
    TObjectPtr<UA1ScoreSaveGame> SavedScores;

private:
    static UA1ScoreManager* Instance;

    static const FString SaveSlotName;

    bool bDoTutorial = false;
};
