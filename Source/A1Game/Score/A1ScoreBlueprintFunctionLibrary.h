// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "A1ScoreData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "A1ScoreBlueprintFunctionLibrary.generated.h"

enum class EGameEndReason : uint8;
/**
 * 
 */
UCLASS()
class A1GAME_API UA1ScoreBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    // === ���� ���� ���� ===
    UFUNCTION(BlueprintCallable, Category = "Score")
    static void StartNewGame();

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void EndGame(EGameEndReason EndReason);

    // === ���� ������ ���� ===
    UFUNCTION(BlueprintCallable, Category = "Score")
    static void AddDaySurvived(int32 Count = 1);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void AddInventoryItems(int32 Count = 1);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void SetInventoryItems(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static int32 GetStorageItems();

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void SetStorageItems(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void AddConsumedItems(int32 Count = 1);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static float GetTotalRepair();

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void SetTotalRepair(int32 count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static float GetCompleteRepair();

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void SetCompleteRepair(int32 count);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void SetRepairedBeforeEscape(bool bRepaired);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void SetRemainingFuel(int32 Fuel);

    // === ���� �Լ��� ===
    UFUNCTION(BlueprintCallable, Category = "Score")
    static void UpdateItemCounts(int32 InventoryCount, int32 StorageCount);

    UFUNCTION(BlueprintCallable, Category = "Score")
    static void ConsumeItem(int32 Count = 1);

    // === ������ �������� ===
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static FA1ScoreData GetCurrentScore();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static TArray<FA1ScoreData> GetAllScores();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static int32 GetHighestScore();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static int32 GetNextGameNumber();

    // === ���� �̸����� ===
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static int32 CalculateCurrentScore();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static FString GetCurrentScoreBreakdown();

    // === ��ƿ��Ƽ ===
    UFUNCTION(BlueprintCallable, Category = "Score")
    static void ClearAllScores();

    // === ���� �Ŵ��� ���� �������� (��� ����ڿ�) ===
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static class UA1ScoreManager* GetScoreManager();

    // === ���� ��� ���� ���� ===
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static FString GetScoreFormula();

    // === ���� ���� ��� (�̸������) ===
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static int32 CalculateSurvivalScore(int32 Days);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
    static FString GetSurvivalScoreBreakdown(int32 Days);
};
