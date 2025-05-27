// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "UI/HUD/A1DayNightWidget.h"

#include "Actors/A1DayNightManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UA1DayNightWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Get DayNightManager Reference
	DayNightManager = AA1DayNightManager::Get(GetWorld());

	if (DayNightManager)
	{
		DayNightManager->OnDayPhaseChanged.AddDynamic(this, &UA1DayNightWidget::OnDayPhaseChanged);
		DayNightManager->OnDayChanged.AddDynamic(this, &UA1DayNightWidget::OnDayChanged);
		DayNightManager->OnTimeChanged.AddDynamic(this, &UA1DayNightWidget::OnTimeChanged);

		//초기 UI 설정
		UpdateUI();
		UpdateTimeDisplay(12, 0);
		Text_Time->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}
}

void UA1DayNightWidget::NativeDestruct()
{
	// 이벤트 구독 해제
	if (DayNightManager)
	{
		DayNightManager->OnDayPhaseChanged.RemoveDynamic(this, &UA1DayNightWidget::OnDayPhaseChanged);
		DayNightManager->OnDayChanged.RemoveDynamic(this, &UA1DayNightWidget::OnDayChanged);
		DayNightManager->OnTimeChanged.RemoveDynamic(this, &UA1DayNightWidget::OnTimeChanged);
	}

	Super::NativeDestruct();
}

void UA1DayNightWidget::OnDayPhaseChanged(EDayPhase NewPhase, int32 CurrentDay)
{
	UpdateUI();
}

void UA1DayNightWidget::OnDayChanged(int32 NewDay)
{
	UpdateUI();
}

void UA1DayNightWidget::OnTimeChanged(int32 Hour, int32 Minute)
{
	UpdateTimeDisplay(Hour, Minute);
}

void UA1DayNightWidget::UpdateUI()
{
	if (!DayNightManager)
		return;

	//Update Text
	if (DayCount)
	{
		DayCount->SetText(FText::Format(NSLOCTEXT("DayNight", "DayCount", "DAY {0}"), DayNightManager->GetCurrentDay()));
	}

	// 아이콘 업데이트
	if (DayNightIcon)
	{
		if (DayNightManager->GetCurrentPhase() == EDayPhase::Day)
		{
			DayNightIcon->SetBrushFromTexture(DayIcon);
		}
		else
		{
			DayNightIcon->SetBrushFromTexture(NightIcon);
		}
	}
}

void UA1DayNightWidget::UpdateTimeDisplay(int32 Hour, int32 Minute)
{
	if (!Text_Time)
		return;

	// 시간 문자열 생성
	FString TimeString = DayNightManager->GetTimeString();
	Text_Time->SetText(FText::FromString(TimeString));
}
