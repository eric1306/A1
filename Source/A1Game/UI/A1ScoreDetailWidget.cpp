// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "UI/A1ScoreDetailWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Score/A1ScoreBlueprintFunctionLibrary.h"

void UA1ScoreDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();

    // 버튼 이벤트 바인딩
    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UA1ScoreDetailWidget::OnCloseClicked);
    }
}

void UA1ScoreDetailWidget::NativeDestruct()
{
	Super::NativeDestruct();

    if (CloseButton)
    {
        CloseButton->OnClicked.Clear();
    }
}

void UA1ScoreDetailWidget::SetScoreData(const FA1ScoreData& ScoreData)
{
    CurrentScoreData = ScoreData;

    // 기본 정보 설정
    if (GameTitleText)
    {
        GameTitleText->SetText(FText::FromString(FString::Printf(TEXT("%s Details"), *ScoreData.GetGameName())));
    }

    if (TotalScoreText)
    {
        TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score: %s"), *FormatScore(ScoreData.TotalScore))));
    }

    // 생존 점수 계산 및 표시
    int32 SurvivalScore = UA1ScoreBlueprintFunctionLibrary::CalculateSurvivalScore(ScoreData.DaysSurvived);
    if (SurvivalScoreText)
    {
        SurvivalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Survival Score: %s"), *FormatScore(SurvivalScore))));
    }

    if (SurvivalDetailText)
    {
        SurvivalDetailText->SetText(FText::FromString(GetSurvivalBreakdown(ScoreData.DaysSurvived)));
    }

    // 아이템 점수 계산 및 표시
    int32 StoredItemScore = FMath::Min((ScoreData.InventoryItems + ScoreData.StorageItems) * 100, 5200);
    int32 ConsumedItemScore = ScoreData.ConsumedItems * 120;
    int32 TotalItemScore = StoredItemScore + ConsumedItemScore;

    if (ItemScoreText)
    {
        ItemScoreText->SetText(FText::FromString(FString::Printf(TEXT("Item Score: %s"), *FormatScore(TotalItemScore))));
    }

    if (ItemDetailText)
    {
        FString ItemDetail = FString::Printf(
            TEXT("Saved Item: %s (%d)\nUsed Item: %s (%d)"),
            *FormatScore(StoredItemScore), ScoreData.InventoryItems + ScoreData.StorageItems,
            *FormatScore(ConsumedItemScore), ScoreData.ConsumedItems
        );
        ItemDetailText->SetText(FText::FromString(ItemDetail));
    }

    // 탈출/사망 보너스
    int32 EscapeBonus = (ScoreData.GameEndReason == EGameEndReason::Escape) ? 5000 : 0;
    if (EscapeScoreText)
    {
        FString EscapeText;
        if (ScoreData.GameEndReason == EGameEndReason::Escape)
        {
            EscapeText = TEXT("Escape Bonus: +5,000");
        }
        else
        {
            EscapeText = TEXT("Escape Bonus: 0 (Dead)");
        }
        EscapeScoreText->SetText(FText::FromString(EscapeText));
    }

    // 수리 점수 계산 및 표시
    int32 BaseRepairScore = (ScoreData.RepairRate / 4.0f) * 40;
    int32 RepairBonus = (ScoreData.bRepairedBeforeEscape && ScoreData.GameEndReason == EGameEndReason::Escape) ? 1000 : 0;
    int32 TotalRepairScore = BaseRepairScore + RepairBonus;

    if (RepairScoreText)
    {
        RepairScoreText->SetText(FText::FromString(FString::Printf(TEXT("Repair Score: %s"), *FormatScore(TotalRepairScore))));
    }

    if (RepairDetailText)
    {
        FString RepairDetail = FString::Printf(TEXT("Base Repair: %s (%.1f%%)"), *FormatScore(BaseRepairScore), ScoreData.RepairRate);

        if (RepairBonus > 0)
        {
            RepairDetail += FString::Printf(TEXT("\nEscape After Repair: +%s"), *FormatScore(RepairBonus));
        }
        RepairDetailText->SetText(FText::FromString(RepairDetail));
    }

    // 연료 점수 계산 및 표시
    int32 FuelScore = FMath::Min((ScoreData.RemainingFuel / 1000) * 100, 20000);
    if (FuelScoreText)
    {
        FuelScoreText->SetText(FText::FromString(FString::Printf(TEXT("Fuel Score: %s"), *FormatScore(FuelScore))));
    }

    if (FuelDetailText)
    {
        FuelDetailText->SetText(FText::FromString(FString::Printf(TEXT("Less Fuel Score: %d (%d×100)"), ScoreData.RemainingFuel, ScoreData.RemainingFuel / 1000)));
    }

    // 날짜 정보
    if (DateText)
    {
        DateText->SetText(FText::FromString(FString::Printf(TEXT("Play Date: %s"), *ScoreData.Timestamp.ToString(TEXT("%Y-%m-%d %H:%M")))));
    }
}

void UA1ScoreDetailWidget::OnCloseClicked()
{
    RemoveFromParent();
}

FString UA1ScoreDetailWidget::FormatScore(int32 Score) const
{
    // 천 단위 콤마 추가
    FString ScoreString = FString::FromInt(Score);
    FString FormattedScore;

    int32 CommaCount = 0;
    for (int32 i = ScoreString.Len() - 1; i >= 0; i--)
    {
        if (CommaCount == 3)
        {
            FormattedScore = TEXT(",") + FormattedScore;
            CommaCount = 0;
        }
        FormattedScore = FString(1, &ScoreString[i]) + FormattedScore;
        CommaCount++;
    }

    return FormattedScore;
}

FString UA1ScoreDetailWidget::GetSurvivalBreakdown(int32 Days) const
{
    if (Days <= 0)
    {
        return TEXT("Survival Day :  0");
    }

    FString Breakdown;

    // Day 1~10: 일 당 50점
    if (Days >= 1)
    {
        int32 Phase1Days = FMath::Min(Days, 10);
        int32 Phase1Score = Phase1Days * 50;
        Breakdown += FString::Printf(TEXT("Day 1~10: %d×50 = %d\n"), Phase1Days, Phase1Score);
    }

    // Day 11~20: 일 당 100점
    if (Days >= 11)
    {
        int32 Phase2Days = FMath::Min(Days - 10, 10);
        int32 Phase2Score = Phase2Days * 100;
        Breakdown += FString::Printf(TEXT("Day 11~20: %d×100 = %d\n"), Phase2Days, Phase2Score);
    }

    // Day 21~30: 일 당 150점
    if (Days >= 21)
    {
        int32 Phase3Days = FMath::Min(Days - 20, 10);
        int32 Phase3Score = Phase3Days * 150;
        Breakdown += FString::Printf(TEXT("Day 21~30: %d×150 = %d\n"), Phase3Days, Phase3Score);
    }

    // Day 31~40: 일 당 300점
    if (Days >= 31)
    {
        int32 Phase4Days = FMath::Min(Days - 30, 10);
        int32 Phase4Score = Phase4Days * 300;
        Breakdown += FString::Printf(TEXT("Day 31~40: %d×300 = %d\n"), Phase4Days, Phase4Score);
    }

    // Day 41~: 존버 방지
    if (Days >= 41)
    {
        int32 ExtraDays = Days - 40;
        Breakdown += FString::Printf(TEXT("Day 41~: %d×0 = 0"), ExtraDays);
    }

    return Breakdown;
}
