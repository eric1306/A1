// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Score/A1ScoreData.h"
#include "UI/LyraActivatableWidget.h"
#include "A1ScoreDetailWidget.generated.h"

struct FA1ScoreData;
class UScrollBox;
class UComboBoxString;
class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class A1GAME_API UA1ScoreDetailWidget : public ULyraActivatableWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    // UI ��ҵ� (Blueprint���� ���ε�)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* GameTitleText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* TotalScoreText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* SurvivalScoreText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* ItemScoreText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* ItemDetailText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* EscapeScoreText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* RepairScoreText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* FuelScoreText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* FuelDetailText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DateText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* CloseButton;

public:
    // ���� ���
    UFUNCTION(BlueprintCallable)
    void SetScoreData(const FA1ScoreData& ScoreData);

protected:
    // �̺�Ʈ �ڵ鷯
    UFUNCTION()
    void OnCloseClicked();

private:
    // ����� ������
    UPROPERTY()
    FA1ScoreData CurrentScoreData;

    // ��ƿ��Ƽ �Լ�
    FString FormatScore(int32 Score) const;
};
