// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Actors/A1TutorialManager.h"

#include "A1CMDBase.h"
#include "A1EquipmentBase.h"
#include "A1GameplayTags.h"
#include "A1LogChannels.h"
#include "A1RepairBase.h"
#include "A1StorageBase.h"
#include "A1StorageEntryBase.h"
#include "MediaPlayer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameModes/LyraGameMode.h"
#include "Interaction/Abilities/A1GameplayAbility_Interact_Cmd.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LyraPlayerController.h"
#include "MediaSource.h"
#include "Blueprint/UserWidget.h"
#include "Character/LyraCharacter.h"
#include "Data/A1ItemData.h"
#include "UI/A1ActivatableWidget.h"
#include "Item/A1ItemTemplate.h"
#include "Item/Fragments/A1ItemFragment_Equipable_Utility.h"
#include "Item/Fragments/A1ItemFragment_Equipable_Weapon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1TutorialManager)

//==============================================================================
// AA1TutorialManager - 통합 튜토리얼 시스템
//==============================================================================

AA1TutorialManager::AA1TutorialManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AA1TutorialManager::BeginPlay()
{
	Super::BeginPlay();
    // 플레이어 컨트롤러 찾기
    PlayerController = Cast<ALyraPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

    // 스텝 데이터 로드
    if (TutorialStepsTable)
    {
        TutorialStepsTable->GetAllRows<FA1TutorialStepData>(TEXT("TutorialSteps"), StepDataArray);
        UE_LOG(LogA1System, Log, TEXT("Loaded %d tutorial steps"), StepDataArray.Num());
    }
}

void AA1TutorialManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 타이머 정리
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
    TutorialStep = ETutorialStep::None;

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

    // Blueprint 이벤트 호출
    OnStepStarted(*StepData);

    //Change Current Tutorial Step
    ChangeTutorialStep(TutorialStep);

    // 액션들 순차 실행
    for (int32 i = 0; i < StepData->Actions.Num(); i++)
    {
        ETutorialActionType ActionType = StepData->Actions[i];
        FString Params = StepData->ActionParams.FindRef(ActionType);

        // 액션 간 지연을 위한 타이머 사용
        float Delay = (i+1) * 0.1f; // 각 액션마다ExecuteAction( 0.1초 간격)
        UE_LOG(LogA1System, Log, TEXT("Action Type: %hhd"), ActionType);
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [this, ActionType, Params]() { ExecuteAction(ActionType, Params); },
            Delay,
            false
        );
    }

    // 완료 조건 설정
    CurrentWaitingCondition = StepData->CompletionCondition;

    // 자동 진행 설정
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

    CurrentStepIndex = StepIndex - 1; // NextStep에서 +1 되므로
    NextStep();
}

void AA1TutorialManager::OnConditionMet(FGameplayTag ConditionTag)
{
    if (!bIsActive || CurrentWaitingCondition != ConditionTag)
    {
        return;
    }

    UE_LOG(LogA1System, Log, TEXT("Condition met: %s"), *ConditionTag.ToString());

    // 자동 진행 타이머 정리
    GetWorld()->GetTimerManager().ClearTimer(AutoProgressTimer);

    // Blueprint 이벤트 호출
    OnStepCompleted(CurrentStepIndex);

    // 다음 스텝으로 진행
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

    // 타이머 정리
    GetWorld()->GetTimerManager().ClearTimer(AutoProgressTimer);
    GetWorld()->GetTimerManager().ClearTimer(ActionDelayTimer);

    // Blueprint 이벤트 호출
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

void AA1TutorialManager::OnItemFill(AA1EquipmentBase* CachedItem)
{
    bool bResult = CachedItem->GetName().Contains(TEXT("FoamGun")) || CachedItem->GetName().Contains(TEXT("OneHandSword"))|| CachedItem->GetName().Contains(TEXT("FlashLight"));
    if (bResult)
    {
        ItemStoredCount++;
    }
    if (ItemStoredCount == 3)
    {
        NextStep();
    }
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
    case ETutorialActionType::CheckStorage:     DoCheckStorage(Params); break;
    case ETutorialActionType::WaitForCondition: DoWaitForCondition(Params); break;
    case ETutorialActionType::ChangeLevel:      DoChangeLevel(Params); break;
    case ETutorialActionType::FadeScreen:       DoFadeScreen(Params); break;
    case ETutorialActionType::Custom:           ExecuteCustomAction(Params); break;
    default: break;
    }
}

void AA1TutorialManager::ChangeTutorialStep(ETutorialStep CurrentStep)
{
    switch (CurrentStep)
    {
    case ETutorialStep::None: SetTutorialStep(ETutorialStep::VideoPlayBack); break;
    case ETutorialStep::VideoPlayBack: SetTutorialStep(ETutorialStep::CarryItems); break;
    case ETutorialStep::CarryItems: SetTutorialStep(ETutorialStep::Emergency); break;
    case ETutorialStep::Emergency: SetTutorialStep(ETutorialStep::Repair); break;
    case ETutorialStep::Repair: SetTutorialStep(ETutorialStep::Store); break;
    case ETutorialStep::Store: SetTutorialStep(ETutorialStep::Collapse); break;
    case ETutorialStep::Collapse: SetTutorialStep(ETutorialStep::End); break;
    }

    UE_LOG(LogA1, Log, TEXT("Current Step: %hhd"), TutorialStep);
}

void AA1TutorialManager::DoPlayVideo(const FString& Params)
{
    // Blueprint에서 구현된 비디오 재생 함수 호출
    // Param : "VideoPath=/Game/Videos/Briefing.mp4;AutoProgress=true"
    UE_LOG(LogA1System, Log, TEXT("Playing video with params: %s"), *Params);

    UIWidget = CreateWidget<UA1ActivatableWidget>(PlayerController, UIWidgetClass);

    if (UIWidget)
    {
        OpenWidget();
    }
    NextStep();
}

void AA1TutorialManager::DoShowMessage(const FString& Params)
{
    // Param: "Title=Warning;Content=Emergency Detected;Duration=3.0"
    UE_LOG(LogA1System, Log, TEXT("Showing message: %s"), *Params);
    FText Title;
    FText Content;
    float Duration = 2.0f;
    // UI 위젯에 메시지 표시 (Blueprint 이벤트로 처리)
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
    //방식 변경 -> 액터 이름으로 하지 말고 
    TArray<FString> ActorNames = ParseStringArray(Params);

    
    TArray<AActor*> OutActor; //Item 정리 step 인 경우
    if (TutorialStep == ETutorialStep::CarryItems)
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1EquipmentBase::StaticClass(), OutActor);
        if (OutActor.Num() > 0)
        {
            //주워야 할 액터 하이라이팅
            for (AActor* EquipActor : OutActor)
            {
                if (AA1EquipmentBase* Equip = Cast<AA1EquipmentBase>(EquipActor))
                {
                    if (Equip->GetPickup()) continue;

                    const UA1ItemTemplate& Template = UA1ItemData::Get().FindItemTemplateByID(Equip->GetTemplateID());
                    if (const UA1ItemFragment_Equipable_Utility* ItemFragment = Cast<UA1ItemFragment_Equipable_Utility>(Template.FindFragmentByClass(UA1ItemFragment_Equipable_Utility::StaticClass())))
                    {
                        if (ItemFragment->UtilityType != EUtilityType::Repairkit)
                        {
                            Equip->OnItemPickupChanged.AddUniqueDynamic(this, &AA1TutorialManager::OnItemPickedUp);
                        }
                    }
                    else if (const UA1ItemFragment_Equipable_Weapon* WeaponFragment = Cast<UA1ItemFragment_Equipable_Weapon>(Template.FindFragmentByClass(UA1ItemFragment_Equipable_Weapon::StaticClass())))
                    {
                        Equip->OnItemPickupChanged.AddUniqueDynamic(this, &AA1TutorialManager::OnItemPickedUp);
                    }
                }
                // 액터 하이라이트 적용
                TArray<UPrimitiveComponent*> PrimitiveComponents;
                EquipActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

                for (UPrimitiveComponent* Component : PrimitiveComponents)
                {
                    Component->SetRenderCustomDepth(true);
                    Component->SetCustomDepthStencilValue(250);
                }
            }
        }
        OutActor.Empty();
    } //Repair step인 경우
    else if (TutorialStep == ETutorialStep::Repair)
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1RepairBase::StaticClass(), OutActor);
        if (OutActor.Num() > 0)
        {
            for (AActor* RepairActor : OutActor)
            {
	            if (AA1RepairBase* Repair = Cast<AA1RepairBase>(RepairActor))
	            {
                    //델리게이트 연결
                    Repair->OnRepairStateChanged.AddUniqueDynamic(this, &AA1TutorialManager::OnRepaired);
                    
                    //오버랩 검사 활성화
                    Repair->ActivateCheckOverlap();

                    Repair->SetCurrentState(RepairState::Break);
                    UE_LOG(LogA1, Log, TEXT("Set Repair State Complete"));
                    
                    // 액터 하이라이트 적용
                    TArray<UPrimitiveComponent*> PrimitiveComponents;
                    Repair->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
                    
                    for (UPrimitiveComponent* Component : PrimitiveComponents)
                    {
                        Component->SetRenderCustomDepth(true);
                        Component->SetCustomDepthStencilValue(250);
                    }
	            }
            }
        }
    }
    else if (TutorialStep == ETutorialStep::Collapse)
    {
        //Highlight CMD
        AActor* CachedActor = UGameplayStatics::GetActorOfClass(GetWorld(), AA1CMDBase::StaticClass());
        if (CachedActor!=nullptr)
        {
	        if (AA1CMDBase* Cmd = Cast<AA1CMDBase>(CachedActor))
	        {
                TArray<UPrimitiveComponent*> PrimitiveComponents;
                Cmd->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

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
    // 파라미터: "Effect=/Game/Effects/Explosion;Location=100,200,300"
    UE_LOG(LogA1System, Log, TEXT("Spawning effects: %s"), *Params);
}

void AA1TutorialManager::DoPlaySound(const FString& Params)
{
    // 파라미터: "Sound=/Game/Audio/Alarm;Volume=1.0;Loop=true"
    UE_LOG(LogA1System, Log, TEXT("Playing sound: %s"), *Params);
}

void AA1TutorialManager::DoCheckStorage(const FString& Params)
{
    TArray<AActor*> Results;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AA1StorageEntryBase::StaticClass(), OUT Results);
    if (Results.Num() > 0)
    {
	    for (auto Result : Results)
	    {
		    if (AA1StorageEntryBase* Entry = Cast<AA1StorageEntryBase>(Result))
		    {
                if (Entry->GetItem()!=nullptr)
                {
                    bool bResult = Entry->GetItem()->GetName().Contains(TEXT("FoamGun")) || Entry->GetItem()->GetName().Contains(TEXT("OneHandSword")) || Entry->GetItem()->GetName().Contains(TEXT("FlashLight"));
                    ItemStoredCount++;

                    ETutorialActionType ActionType = StepDataArray[CurrentStepIndex]->Actions[0];
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
                                Content = FText::FromString(FString::Printf(TEXT("%s %d/3"), *Value, ItemStoredCount));
                            }
                        }
                    }

                    SendUIMessage(Title, Content);
                }
                else
                {
                    Entry->OnItemEntryStateChanged.AddDynamic(this, &AA1TutorialManager::OnItemFill);
                }
		    }
	    }
    }

    if (ItemStoredCount == 3)
    {
        NextStep();
    }
}

void AA1TutorialManager::DoWaitForCondition(const FString& Params)
{
    // 파라미터에서 조건 태그 설정
    CurrentWaitingCondition = FGameplayTag::RequestGameplayTag(*Params);
}

void AA1TutorialManager::DoChangeLevel(const FString& Params)
{
    // 파라미터: "Level=MainGame;FadeTime=2.0"
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

void AA1TutorialManager::OnRepaired()
{
    RepairCount++;
    UE_LOG(LogA1, Log, TEXT("RepairCount: %d"), RepairCount);
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
                Content = FText::FromString(FString::Printf(TEXT("%s %d/10"), *Value, RepairCount));
            }
        }
    }

    SendUIMessage(Title, Content);

    if (RepairCount == 10)
    {
        NextStep();
    }

}

FVector AA1TutorialManager::ParseVector(const FString& VectorString)
{
    // "X,Y,Z" 형태 파싱
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
    // "Pitch,Yaw,Roll" 형태 파싱
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
        // 지연 후 트리거
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
    // 에디터에서 기본 튜토리얼 데이터 테이블 생성
    // 실제 구현은 에디터 전용 코드에서 처리
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

