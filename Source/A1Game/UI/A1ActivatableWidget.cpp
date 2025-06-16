// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "UI/A1ActivatableWidget.h"

UA1ActivatableWidget::UA1ActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UA1ActivatableWidget::Deactivate()
{
    DeactivateWidget();
}

FReply UA1ActivatableWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
    UE_LOG(LogTemp, Log, TEXT("Activate Key: %s"), *DeactivateKey.ToString())
	if (DeactivateKey.IsValid() && InKeyEvent.GetKey() == DeactivateKey && InKeyEvent.IsRepeat() == false)
	{
		Deactivate();
        return FReply::Handled();
	}

    return Reply;
}

void UA1ActivatableWidget::NativeOnActivated()
{
 
     Super::NativeOnActivated();

     APlayerController* PC = GetOwningPlayer();
     if (PC)
     {
         FInputModeGameAndUI InputMode;
         InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
         InputMode.SetHideCursorDuringCapture(false);
         InputMode.SetWidgetToFocus(TakeWidget());

         PC->SetInputMode(InputMode);
         PC->bShowMouseCursor = true;
     }
    
}

void UA1ActivatableWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
}
