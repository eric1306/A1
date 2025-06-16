// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "A1ScoreEntryButtonWidget.generated.h"

class UButton;
class UTextBlock;
struct FA1ScoreData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreEntryClicked, int32, ScoreIndex);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class A1GAME_API UA1ScoreEntryButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // ��������Ʈ
    UPROPERTY(BlueprintAssignable)
    FOnScoreEntryClicked OnScoreEntryClicked;

protected:
    // UI ��ҵ�
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> EntryButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> ScoreInfoText;

public:
    // ���� �Լ�
    UFUNCTION(BlueprintCallable)
    void SetupScoreEntry(const FA1ScoreData& ScoreData, int32 Rank, int32 Index);

protected:
    // ��ư Ŭ�� �̺�Ʈ
    UFUNCTION()
    void OnButtonClicked();

private:
    // ����� �ε���
    int32 StoredIndex;
};
