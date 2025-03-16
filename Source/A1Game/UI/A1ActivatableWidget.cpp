// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "UI/A1ActivatableWidget.h"

UA1ActivatableWidget::UA1ActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FReply UA1ActivatableWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (DeactivateKey.IsValid() && InKeyEvent.GetKey() == DeactivateKey && InKeyEvent.IsRepeat() == false)
	{
		DeactivateWidget();
		return FReply::Handled();
	}

	return Reply;
}
