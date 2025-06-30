// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "UI/A1ActivatableWidget.h"

#include "Components/TextBlock.h"

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

void UA1ActivatableWidget::AffectTypingEffect(UTextBlock* TargetTextBlock, FString InText, float delta, float startdelay)
{
    if (TypingSound)
        PlaySound(TypingSound);

    FString FormattedText = InText.Replace(TEXT("\\n"), TEXT("\n")); // Ȥ�� �׳� "\n"

    FTimerHandle TypingHandle;
    FTypingState* State = new FTypingState();
    State->FullText = FormattedText;
    State->CurrentText = "";
    State->CurrentIndex = 0;
    State->TimerHandle = TypingHandle;
    State->TargetTextBlock = TargetTextBlock;

    // ���� ����
    FTimerDelegate TypingDelegate = FTimerDelegate::CreateLambda([this, State]()
        {
            // ��ȿ�� �˻�
            if (!State->TargetTextBlock)
            {
                GetWorld()->GetTimerManager().PauseTimer(State->TimerHandle);
                delete State;
                return;
            }

            // �Ϸ� üũ
            if (State->CurrentIndex >= State->FullText.Len())
            {
                GetWorld()->GetTimerManager().PauseTimer(State->TimerHandle);
                delete State;
                return;
            }

            // ���� �ϳ� �߰�
            State->CurrentText.AppendChar(State->FullText[State->CurrentIndex]);
            State->TargetTextBlock->SetText(FText::FromString(State->CurrentText));
            State->CurrentIndex++;
        });

    // Ÿ�̸� ���
    GetWorld()->GetTimerManager().SetTimer(
        State->TimerHandle,
        TypingDelegate,
        delta,
        true,
        startdelay
    );
}