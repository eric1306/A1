// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1TutorialManager.h"

#include "A1CMDBase.h"
#include "A1EquipmentBase.h"
#include "A1GameplayTags.h"
#include "A1LogChannels.h"
#include "MediaPlayer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameModes/LyraGameMode.h"
#include "Interaction/Abilities/A1GameplayAbility_Interact_Cmd.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LyraPlayerController.h"
#include "MediaSource.h"
#include "Blueprint/UserWidget.h"
#include "Character/LyraCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1TutorialManager)

//==============================================================================
// AA1TutorialManager - ���� Ʃ�丮�� �ý���
//==============================================================================

AA1TutorialManager::AA1TutorialManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AA1TutorialManager::BeginPlay()
{
	Super::BeginPlay();
    // �÷��̾� ��Ʈ�ѷ� ã��
    PlayerController = Cast<ALyraPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

    // ���� ������ �ε�
    if (TutorialStepsTable)
    {
        TutorialStepsTable->GetAllRows<FA1TutorialStepData>(TEXT("TutorialSteps"), StepDataArray);
        UE_LOG(LogA1System, Log, TEXT("Loaded %d tutorial steps"), StepDataArray.Num());
    }
}

void AA1TutorialManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Ÿ�̸� ����
    GetWorld()->GetTimerManager().ClearTimer(AutoProgressTimer);
    GetWorld()->GetTimerManager().ClearTimer(ActionDelayTimer);
    FText EmptyTitle = FText::FromString(TEXT(""));
    FText EmptyContent = FText::FromString(TEXT(""));

    SendUIMessage(EmptyTitle, EmptyContent);

    Super::EndPlay(EndPlayReason);
}

void AA1TutorialManager::StartTutorial()
{
    if (bIsActive || StepDataArray.Num() == 0)
    {
        return;
    }

    bIsActive = true;
    CurrentStepIndex = -1;

    UE_LOG(LogA1System, Log, TEXT("Starting compact tutorial with %d steps"), StepDataArray.Num());

    NextStep();
}

void AA1TutorialManager::NextStep()
{
    if (!bIsActive)
    {
        return;
    }

    CurrentStepIndex++;

    if (!StepDataArray.IsValidIndex(CurrentStepIndex))
    {
        EndTutorial();
        return;
    }

    FA1TutorialStepData* StepData = StepDataArray[CurrentStepIndex];
    if (!StepData)
    {
        NextStep();
        return;
    }

    UE_LOG(LogA1System, Log, TEXT("Starting step %d: %s"), CurrentStepIndex, *StepData->StepName.ToString());

    // Blueprint �̺�Ʈ ȣ��
    OnStepStarted(*StepData);

    // �׼ǵ� ���� ����
    for (int32 i = 0; i < StepData->Actions.Num(); i++)
    {
        ETutorialActionType ActionType = StepData->Actions[i];
        FString Params = StepData->ActionParams.FindRef(ActionType);

        // �׼� �� ������ ���� Ÿ�̸� ���
        float Delay = (i+1) * 0.1f; // �� �׼Ǹ���ExecuteAction( 0.1�� ����)
        UE_LOG(LogA1System, Log, TEXT("Action Type: %hhd"), ActionType);
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [this, ActionType, Params]() { ExecuteAction(ActionType, Params); },
            Delay,
            false
        );
    }

    // �Ϸ� ���� ����
    CurrentWaitingCondition = StepData->CompletionCondition;

    // �ڵ� ���� ����
    if (StepData->AutoProgressTime > 0.0f)
    {
        
        GetWorld()->GetTimerManager().SetTimer(
            AutoProgressTimer,
            this,
            &AA1TutorialManager::NextStep,
            StepData->AutoProgressTime,
            false
        );
    }
}

void AA1TutorialManager::GoToStep(int32 StepIndex)
{
    if (!StepDataArray.IsValidIndex(StepIndex))
    {
        return;
    }

    CurrentStepIndex = StepIndex - 1; // NextStep���� +1 �ǹǷ�
    NextStep();
}

void AA1TutorialManager::OnConditionMet(FGameplayTag ConditionTag)
{
    if (!bIsActive || CurrentWaitingCondition != ConditionTag)
    {
        return;
    }

    UE_LOG(LogA1System, Log, TEXT("Condition met: %s"), *ConditionTag.ToString());

    // �ڵ� ���� Ÿ�̸� ����
    GetWorld()->GetTimerManager().ClearTimer(AutoProgressTimer);

    // Blueprint �̺�Ʈ ȣ��
    OnStepCompleted(CurrentStepIndex);

    // ���� �������� ����
    NextStep();
}

void AA1TutorialManager::EndTutorial()
{
    if (!bIsActive)
    {
        return;
    }

    UE_LOG(LogA1System, Log, TEXT("Tutorial completed"));

    bIsActive = false;
    CurrentStepIndex = -1;

    // Ÿ�̸� ����
    GetWorld()->GetTimerManager().ClearTimer(AutoProgressTimer);
    GetWorld()->GetTimerManager().ClearTimer(ActionDelayTimer);

    // Blueprint �̺�Ʈ ȣ��
    OnTutorialCompleted();
}

//==============================================================================
// Active Function
//==============================================================================

void AA1TutorialManager::SendUIMessage(FText Title, FText Content)
{
    FA1TutorialUIMessage Message;
    Message.Title = Title;
    Message.Content = Content;

    UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(PlayerController);
    MessageSystem.BroadcastMessage(A1GameplayTags::Message_Tutorial_Notice, Message);
}

void AA1TutorialManager::ExecuteAction(ETutorialActionType ActionType, const FString& Params)
{
    switch (ActionType)
    {
    case ETutorialActionType::PlayVideo:        DoPlayVideo(Params); break;
    case ETutorialActionType::ShowMessage:      DoShowMessage(Params); break;
    case ETutorialActionType::HighlightActors:  DoHighlightActors(Params); break;
    case ETutorialActionType::SpawnEffects:     DoSpawnEffects(Params); break;
    case ETutorialActionType::PlaySound:        DoPlaySound(Params); break;
    case ETutorialActionType::WaitForCondition: DoWaitForCondition(Params); break;
    case ETutorialActionType::ChangeLevel:      DoChangeLevel(Params); break;
    case ETutorialActionType::FadeScreen:       DoFadeScreen(Params); break;
    case ETutorialActionType::Custom:           ExecuteCustomAction(Params); break;
    default: break;
    }
}

void AA1TutorialManager::DoPlayVideo(const FString& Params)
{
    // Blueprint���� ������ ���� ��� �Լ� ȣ��
    // Param : "VideoPath=/Game/Videos/Briefing.mp4;AutoProgress=true"
    UE_LOG(LogA1System, Log, TEXT("Playing video with params: %s"), *Params);

    UIWidget = CreateWidget<UUserWidget>(PlayerController, UIWidgetClass);

    if (UIWidget)
    {
        UIWidget->AddToViewport();
        FInputModeUIOnly UIMode;
        PlayerController->SetInputMode(UIMode);
    }

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            UIWidget->RemoveFromParent();
            FInputModeGameOnly  GUMode;
            PlayerController->SetInputMode(GUMode);
        }, 20.f, false);
}

void AA1TutorialManager::DoShowMessage(const FString& Params)
{
    // Param: "Title=Warning;Content=Emergency Detected;Duration=3.0"
    UE_LOG(LogA1System, Log, TEXT("Showing message: %s"), *Params);
    FText Title;
    FText Content;
    float Duration = 2.0f;
    // UI ������ �޽��� ǥ�� (Blueprint �̺�Ʈ�� ó��)
    TArray<FString> ParamPairs;
    Params.ParseIntoArray(ParamPairs, TEXT(";"));

    for (const FString& Pair : ParamPairs)
    {
        FString Key, Value;
        if (Pair.Split(TEXT("="), &Key, &Value))
        {
            if (Key == TEXT("Title"))
            {
                Title = FText::FromString(Value);
            }
            else if (Key == TEXT("Content"))
            {
                Content = FText::FromString(Value);
            }
            else if (Key == TEXT("Duration"))
            {
                Duration = FCString::Atof(*Value);
            }
        }
    }
    SendUIMessage(Title, Content);
}

void AA1TutorialManager::DoHighlightActors(const FString& Params)
{
    // Param: "Actors=Actor1,Actor2,Actor3;Enable=true"
    //��� ���� -> ���� �̸����� ���� ���� 
    TArray<FString> ActorNames = ParseStringArray(Params);

    for (const FString& ActorName : ActorNames)
    {
        TArray<AActor*> OutActor;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1EquipmentBase::StaticClass(), OutActor);
        if (OutActor.Num() > 0)
        {
            for (AActor* EquipActor : OutActor)
            {
                if (AA1EquipmentBase* Equip = Cast<AA1EquipmentBase>(EquipActor))
                {
                    if (Equip->GetPickup()) continue;

                    Equip->OnItemPickupChanged.AddUniqueDynamic(this, &AA1TutorialManager::OnItemPickedUp);
                }
                // ���� ���̶���Ʈ ����
                TArray<UPrimitiveComponent*> PrimitiveComponents;
                EquipActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

                for (UPrimitiveComponent* Component : PrimitiveComponents)
                {
                    Component->SetRenderCustomDepth(true);
                    Component->SetCustomDepthStencilValue(250);
                }
            }
        }
    }
}

void AA1TutorialManager::DoSpawnEffects(const FString& Params)
{
    // �Ķ����: "Effect=/Game/Effects/Explosion;Location=100,200,300"
    UE_LOG(LogA1System, Log, TEXT("Spawning effects: %s"), *Params);
}

void AA1TutorialManager::DoPlaySound(const FString& Params)
{
    // �Ķ����: "Sound=/Game/Audio/Alarm;Volume=1.0;Loop=true"
    UE_LOG(LogA1System, Log, TEXT("Playing sound: %s"), *Params);
}

void AA1TutorialManager::DoWaitForCondition(const FString& Params)
{
    // �Ķ���Ϳ��� ���� �±� ����
    CurrentWaitingCondition = FGameplayTag::RequestGameplayTag(*Params);
}

void AA1TutorialManager::DoChangeLevel(const FString& Params)
{
    // �Ķ����: "Level=MainGame;FadeTime=2.0"
    UGameplayStatics::OpenLevel(GetWorld(), *Params);
}

void AA1TutorialManager::OnItemPickedUp()
{
    ItemCount++;
    UE_LOG(LogA1, Log, TEXT("ItemCount: %d"), ItemCount);

    ETutorialActionType ActionType = StepDataArray[CurrentStepIndex]->Actions[1];
    FString Params = StepDataArray[CurrentStepIndex]->ActionParams.FindRef(ActionType);

    FText Title;
    FText Content;
    TArray<FString> ParamPairs;
    Params.ParseIntoArray(ParamPairs, TEXT(";"));

    for (const FString& Pair : ParamPairs)
    {
        FString Key, Value;
        if (Pair.Split(TEXT("="), &Key, &Value))
        {
            if (Key == TEXT("Title"))
            {
                Title = FText::FromString(Value);
            }
            else if (Key == TEXT("Content"))
            {
                Content = FText::FromString(FString::Printf(TEXT("%s %d/3"), *Value, ItemCount));
            }
        }
    }

    SendUIMessage(Title, Content);

    if (ItemCount == 3)
    {
        NextStep();
    }
}

FVector AA1TutorialManager::ParseVector(const FString& VectorString)
{
    // "X,Y,Z" ���� �Ľ�
    TArray<FString> Components;
    VectorString.ParseIntoArray(Components, TEXT(","));

    if (Components.Num() >= 3)
    {
        return FVector(
            FCString::Atof(*Components[0]),
            FCString::Atof(*Components[1]),
            FCString::Atof(*Components[2])
        );
    }

    return FVector::ZeroVector;
}

FRotator AA1TutorialManager::ParseRotator(const FString& RotatorString)
{
    // "Pitch,Yaw,Roll" ���� �Ľ�
    TArray<FString> Components;
    RotatorString.ParseIntoArray(Components, TEXT(","));

    if (Components.Num() >= 3)
    {
        return FRotator(
            FCString::Atof(*Components[0]),
            FCString::Atof(*Components[1]),
            FCString::Atof(*Components[2])
        );
    }

    return FRotator::ZeroRotator;
}

TArray<FString> AA1TutorialManager::ParseStringArray(const FString& ArrayString)
{
    TArray<FString> Result;
    ArrayString.ParseIntoArray(Result, TEXT(","));
    return Result;
}

/******************************************************
 *
 * UA1TutorialInteractionComponent
 * 
 ******************************************************/

UA1TutorialInteractionComponent::UA1TutorialInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UA1TutorialInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
    FindTutorialManager();
}

void UA1TutorialInteractionComponent::OnInteracted()
{
    if (!bIsEnabled)
    {
        return;
    }

    if (InteractionDelay > 0.0f)
    {
        // ���� �� Ʈ����
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TriggerCondition(); });
    }
    else
    {
        TriggerCondition();
    }
}

void UA1TutorialInteractionComponent::TriggerCondition()
{
    if (TutorialManager && TriggerConditionTag.IsValid())
    {
        TutorialManager->OnConditionMet(TriggerConditionTag);
    }
}

void UA1TutorialInteractionComponent::SetHighlight(bool bEnabled)
{
    if (AActor* Owner = GetOwner())
    {
        TArray<UPrimitiveComponent*> PrimitiveComponents;
        Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

        for (UPrimitiveComponent* Component : PrimitiveComponents)
        {
            Component->SetRenderCustomDepth(bEnabled);
            if (bEnabled)
            {
                Component->SetCustomDepthStencilValue(250);
            }
        }
    }
}

void UA1TutorialInteractionComponent::FindTutorialManager()
{
    if (TutorialManager)
    {
        return;
    }

    TutorialManager = Cast<AA1TutorialManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AA1TutorialManager::StaticClass())
    );
}

/******************************************************
 *
 * UA1TutorialBlueprintLibrary
 *
 ******************************************************/

UDataTable* UA1TutorialBlueprintLibrary::CreateBasicTutorialData()
{
    // �����Ϳ��� �⺻ Ʃ�丮�� ������ ���̺� ����
    // ���� ������ ������ ���� �ڵ忡�� ó��
    return nullptr;
}

FString UA1TutorialBlueprintLibrary::MakeVectorParam(FVector Vector)
{
    return FString::Printf(TEXT("%.2f,%.2f,%.2f"), Vector.X, Vector.Y, Vector.Z);
}

FString UA1TutorialBlueprintLibrary::MakeRotatorParam(FRotator Rotator)
{
    return FString::Printf(TEXT("%.2f,%.2f,%.2f"), Rotator.Pitch, Rotator.Yaw, Rotator.Roll);
}

FString UA1TutorialBlueprintLibrary::MakeArrayParam(const TArray<FString>& StringArray)
{
    return FString::Join(StringArray, TEXT(","));
}

FString UA1TutorialBlueprintLibrary::MakeFloatParam(float Value)
{
    return FString::Printf(TEXT("%.2f"), Value);
}

void UA1TutorialBlueprintLibrary::TriggerTutorialCondition(UObject* WorldContext, FGameplayTag ConditionTag)
{
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (AA1TutorialManager* TutorialManager = Cast<AA1TutorialManager>(
            UGameplayStatics::GetActorOfClass(World, AA1TutorialManager::StaticClass())))
        {
            TutorialManager->OnConditionMet(ConditionTag);
        }
    }
}

