// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "UI/A1ScoreWidget.h"

#include "Components/TextBlock.h"
#include "Score/A1ScoreBlueprintFunctionLibrary.h"
#include "Score/A1ScoreData.h"
#include "Score/A1ScoreManager.h"

void UA1ScoreWidget::SetEndingText()
{
	const FA1ScoreData& Data = UA1ScoreManager::Get()->GetCurrentScore();
	Survival->SetText(FText::FromString(IntegertoString(Survival->GetText(), Data.CalculateSurvivalScore(Data.DaysSurvived))));

	int32 TotalItemCount = (Data.InventoryItems + Data.StorageItems) * 100;
	Item->SetText(FText::FromString(IntegertoString(Item->GetText(), TotalItemCount)));

	UsedItem->SetText(FText::FromString(IntegertoString(UsedItem->GetText(), Data.ConsumedItems * 120)));

	int32 BonusValue = Data.bRepairedBeforeEscape ? 5000 : 0;
	Escape->SetText(FText::FromString(IntegertoString(Escape->GetText(), BonusValue)));

	Repair->SetText(FText::FromString(IntegertoString(Repair->GetText(), Data.CompleteRepair * 20)));

	Fuel->SetText(FText::FromString(IntegertoString(Fuel->GetText(), (Data.RemainingFuel/1000)*100)));

	Total->SetText(FText::FromString(IntegertoString(Total->GetText(), Data.TotalScore)));
}

FString UA1ScoreWidget::IntegertoString(FText InText, int32 value)
{
	FString CurrentString = InText.ToString();
	return CurrentString + FString::FromInt(value);
}

FString UA1ScoreWidget::FloatToString(FText InText, float value)
{
	FString CurrentString = InText.ToString();
	return CurrentString + FString::SanitizeFloat(value);
}

TOptional<FUIInputConfig> UA1ScoreWidget::GetDesiredInputConfig() const
{
	return Super::GetDesiredInputConfig();
}
