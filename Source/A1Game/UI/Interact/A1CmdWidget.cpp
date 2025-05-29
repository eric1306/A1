// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1CmdWidget.h"

#include "A1LogChannels.h"
#include "A1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Data/A1CmdData.h"

UA1CmdWidget::UA1CmdWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UA1CmdWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UA1CmdWidget::NativeDestruct()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(MessageListenerHandle);
}

void UA1CmdWidget::ConstructUI(FGameplayTag Channel, const FASCInitializeMessage& Message)
{
	if (Message.ASC == nullptr)
		return;

	ASC = Message.ASC;
}

void UA1CmdWidget::DestructUI()
{
    MenuBox->SetVisibility(ESlateVisibility::Visible);
    SuperviseText->SetText(FText::FromString(""));
}

void UA1CmdWidget::InputEnded(FText InText)
{
    if (InText.ToString() == TEXT(""))
        return;

    UE_LOG(LogA1System, Log, TEXT("%s"), *InText.ToString());

    // Map Open
    if (InText.ToString() == TEXT("Map"))
    {
        if (ASC)
        {
            FGameplayEventData Payload;
            ASC->HandleGameplayEvent(A1GameplayTags::GameplayEvent_Cmd_Map, &Payload);
        }

    }

    // 현 상황에 맞는 도움말 제공
    else if (InText.ToString() == TEXT("Help"))
    {
        const FCmdTextSet* TextSet = UA1CmdData::Get().GetTextSetByLabel("Help");
        const FString* Text = TextSet->TextEntries.Find("Tutorial");

        if (Text != nullptr)
            SuperviseText->SetText(FText::FromString(*Text));

        MenuBox->SetVisibility(ESlateVisibility::Hidden);
    }
    else if (InText.ToString() == TEXT("Document"))
    {

    }
    else if (InText.ToString() == TEXT("Exit"))
    {
        DestructUI();
        Deactivate();
    }

    // 없는 명령어 입력
    else
    {
        const FCmdTextSet* TextSet = UA1CmdData::Get().GetTextSetByLabel("Error");
        const FString* Text = TextSet->TextEntries.Find("Invalid");

        if (Text != nullptr)
            SuperviseText->SetText(FText::FromString(*Text));
    }

    InputText->SetText(FText::FromString(""));
}

