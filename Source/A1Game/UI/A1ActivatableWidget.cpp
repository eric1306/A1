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
