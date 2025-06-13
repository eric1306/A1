// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1CmdWidget.h"

#include "A1LogChannels.h"
#include "A1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/A1RepairBase.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/ProgressBar.h"
#include "Data/A1CmdData.h"
#include "Data/A1UIData.h"
#include "EngineUtils.h"
#include "Actors/A1TutorialManager.h"
#include "GameModes/LyraGameMode.h"
#include "Kismet/GameplayStatics.h"

UA1CmdWidget::UA1CmdWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    EscapeMode = false;
}

void UA1CmdWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &UA1CmdWidget::ConstructUI);
    
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
    TutoMode = Message.bTutorial;

    ShowMenu();
    InputText->SetFocus();

    if (AActor* Manager = UGameplayStatics::GetActorOfClass(GetWorld(), AA1TutorialManager::StaticClass()))
    {
        if (AA1TutorialManager* TutorialMaanger = Cast<AA1TutorialManager>(Manager)) TutoMode = true;
        else TutoMode = false;
    }
}

void UA1CmdWidget::DestructUI()
{
    HiddenMenu();
    SuperviseText->SetText(FText::FromString(""));
    EscapeGuideTxt->SetText(FText::FromString(""));
    EscapeKeyTxt->SetText(FText::FromString(""));
}

void UA1CmdWidget::InputEnded(FText InText)
{
    if (InText.ToString() == TEXT(""))
        return;

    UE_LOG(LogA1System, Log, TEXT("%s"), *InText.ToString());

    SuperviseText->SetVisibility(ESlateVisibility::Hidden);
    SuperviseText->SetText(FText::FromString(""));
    InputText->SetText(FText::FromString(""));

    if (EscapeMode)
    {
        if (InText.ToString() == TEXT("Confirm"))
        {
            if (TutoMode)    // tutorial
            {
                // TODO eric1306
                // Fade Out / Temp
                DestructUI();
                UE_LOG(LogA1, Log, TEXT("Fade Out!"));
                if (ALyraGameMode* GameMode = Cast<ALyraGameMode>(GetWorld()->GetAuthGameMode()))
                {
                    GameMode->TriggerFadeOnAllPlayer(0.f, 1.f, 1.f, FLinearColor::White);
                    LoadStory();
                }
            }
            else             // InGame
            {

                TSubclassOf<UUserWidget> EndingCutSceneClass = UA1UIData::Get().EndingCutSceneClass;
                UUserWidget* EndingCutScene = CreateWidget<UUserWidget>(GetWorld(), EndingCutSceneClass);
                if (EndingCutScene)
                    EndingCutScene->AddToViewport();

                // TEMP
                DestructUI();
                if (ASC)
                {
                    FGameplayEventData Payload;
                    ASC->HandleGameplayEvent(A1GameplayTags::GameplayEvent_Cmd_Exit, &Payload);
                }
                
                // // TODO eric1306 
                // GameOver ó�� 
            }
        }
        else if (InText.ToString() == TEXT("Deny"))
        {
            ShowMenu();
        }
        EscapeMode = false;
    }

    if (InText.ToString() == TEXT("Menu"))
    {
        if (MenuBox->GetVisibility() != ESlateVisibility::Visible)
            ShowMenu();
    }

    // Map Open
    else if (InText.ToString() == TEXT("Map"))
    {
        if (ASC)
        {
            FGameplayEventData Payload;
            ASC->HandleGameplayEvent(A1GameplayTags::GameplayEvent_Cmd_Map, &Payload);
        }
    }
    else
    {
        HiddenMenu();
        SuperviseText->SetVisibility(ESlateVisibility::Visible);

        // �� ��Ȳ�� �´� ���� ����
        if (InText.ToString() == TEXT("Help"))
        {
            const FCmdTextSet* TextSet = UA1CmdData::Get().GetTextSetByLabel("Help");
            const FString* Text = TextSet->TextEntries.Find("Tutorial");

            if (Text != nullptr)
                AffectTypingEffect(SuperviseText, *const_cast<FString*>(Text), TypingDelta / 4, 0.0f);
        }
        //else if (InText.ToString() == TEXT("Document"))
        //{
        //
        //}
        else if (InText.ToString() == TEXT("Escape"))
        {
            EscapeMode = true;
            EscapeScreen->SetVisibility(ESlateVisibility::Visible);

            UWorld* World = GetWorld();

            int Total = 0;
            int32 Count = 0;
            for (TActorIterator<AA1RepairBase> It(World); It; ++It)
            {     
                if (It->CurrentState == RepairState::NotBroken)
                    continue;
                
                Total++;

                if(It->CurrentState == RepairState::Complete)
                    ++Count;
            }
               
            float percent = (Total == 0) ? 0 : ((float)Count / Total);
            ShowRepairPercent(percent);

            EscapeGuideTxt->SetVisibility(ESlateVisibility::Visible);
            EscapeKeyTxt->SetVisibility(ESlateVisibility::Visible);

            FName EntryLable = "";
            FString KeyText = "";
            if (Total != Count)
            {
                EntryLable = "Cannot";
                KeyText = ">  Menu";
            }
            else
            {
                EntryLable = "Can";
                KeyText = ">  Confirm / Deny";
            }

            const FCmdTextSet* TextSet = UA1CmdData::Get().GetTextSetByLabel("Escape");
            const FString* Text = TextSet->TextEntries.Find(EntryLable);
            if (Text != nullptr)
            {
                AffectTypingEffect(EscapeGuideTxt, *const_cast<FString*>(Text), TypingDelta, 0.0f);
                AffectTypingEffect(EscapeKeyTxt, KeyText, TypingDelta, Text->Len() * TypingDelta);
            }
        }
        else if (InText.ToString() == TEXT("Exit"))
        {
            DestructUI();
            if (ASC)
            {
                FGameplayEventData Payload;
                ASC->HandleGameplayEvent(A1GameplayTags::GameplayEvent_Cmd_Exit, &Payload);
            }

            return;
        }

        // ���� ��ɾ� �Է�
        else
        {
            SuperviseText->SetVisibility(ESlateVisibility::Visible);

            const FCmdTextSet* TextSet = UA1CmdData::Get().GetTextSetByLabel("Error");
            const FString* Text = TextSet->TextEntries.Find("Invalid");

            if (Text != nullptr)
                AffectTypingEffect(SuperviseText, *const_cast<FString*>(Text), TypingDelta, 0.0f);
        }
    }
   
    InputText->SetFocus();
}

void UA1CmdWidget::ShowMenu()
{
    MenuBox->SetVisibility(ESlateVisibility::Visible);
    EscapeScreen->SetVisibility(ESlateVisibility::Hidden);
    SuperviseText->SetVisibility(ESlateVisibility::Hidden);

    AffectTypingEffect(MenuText1, ">  Map", TypingDelta/6, 0.5f);
    AffectTypingEffect(MenuText2, ">  Help", TypingDelta/7, 0.5f);
    //AffectTypingEffect(MenuText3, ">  Documents", TypingDelta/12, 0.5f);
    AffectTypingEffect(MenuText3, ">  Escape", TypingDelta/9, 0.5f);
    AffectTypingEffect(MenuText4, ">  Exit", TypingDelta/7, 0.5f);

    EscapeGuideTxt->SetText(FText::FromString(""));
    EscapeKeyTxt->SetText(FText::FromString(""));
    SuperviseText->SetText(FText::FromString(""));
}

void UA1CmdWidget::HiddenMenu()
{
    MenuBox->SetVisibility(ESlateVisibility::Hidden);
    MenuText1->SetText(FText::FromString(""));
    MenuText2->SetText(FText::FromString(""));
    MenuText3->SetText(FText::FromString(""));
    MenuText4->SetText(FText::FromString(""));
    MenuText5->SetText(FText::FromString(""));
}

void UA1CmdWidget::AffectTypingEffect(UTextBlock* TargetTextBlock, FString InText, float delta, float startdelay)
{
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