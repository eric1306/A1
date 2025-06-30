// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "A1ScoreEntryButtonWidget.generated.h"

class UBorder;
class UButton;
class UTextBlock;
class USizeBox;
class UA1ScoreDetailWidget;

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
    TObjectPtr<UBorder> Border_BG;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> EntryButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> ScoreInfoText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<USizeBox> DetailScoreBox;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    TObjectPtr<UA1ScoreDetailWidget> ScoreDetailWidget;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UMaterialInstance> EscapeMaterial;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UMaterialInstance> DeadMaterial;

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
