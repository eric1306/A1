// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Interaction/Abilities/A1GameplayAbility_Interact_Bed.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/A1CharacterAttributeSet.h"
#include "Actors/A1BedBase.h"
#include "Actors/A1DayNightManager.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_Interact_Bed)

UA1GameplayAbility_Interact_Bed::UA1GameplayAbility_Interact_Bed(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
}

void UA1GameplayAbility_Interact_Bed::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (TriggerEventData == nullptr || bInitialized == false)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    if (HasAuthority(&CurrentActivationInfo) == false)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    AA1BedBase* BedActor = Cast<AA1BedBase>(InteractableActor);
    if (BedActor == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetAvatarActorFromActorInfo());
    if (LyraCharacter == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    //�̹� ������ ���� �ƴ� ĳ���Ͱ� �����ϴ� ���
    if (BedActor->bIsOccupyingCharacterExist() && BedActor->GetOccupyingCharacter() != LyraCharacter)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    //ĳ������ �Ƿε��� 10 �����ε� �������� �õ��ϴ� ���
    ULyraAbilitySystemComponent* ASC = LyraCharacter->GetLyraAbilitySystemComponent();
    if (ASC)
    {
        const UA1CharacterAttributeSet* Attribute = ASC->GetSet<UA1CharacterAttributeSet>();

        if (Attribute->GetWeight() <= 10.f && BedActor->GetBedState() == EBedState::Empty)
        {
            CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
            return;
        }
    }

    AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AA1DayNightManager::StaticClass());
    if (Actor == nullptr)
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
        return;
    }

    switch (BedActor->GetBedState())
    {
    case EBedState::Empty:
    {
        LyraCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);
        BedActor->StorePlayerReturnTransform(LyraCharacter->GetActorTransform());

        PlayScreenFadeEffect(false);

        FTransform BedTransform = BedActor->GetLayDownTransform();
        LyraCharacter->SetActorTransform(BedTransform);

        //TODO eric1306: why rotation?

        //Temp Code eric1306: Character get BedActor's Transform -> Set Character Scale
        LyraCharacter->SetActorScale3D(FVector(1.f, 1.f, 1.f));

        BedActor->SetBedState(EBedState::Occupied);

        // �߰�: ħ�뿡 ĳ���� ���� ����
        BedActor->SetOccupyingCharacter(LyraCharacter);

        // �߰�: DayNightManager�� ħ�� ���
        if (AA1DayNightManager* DayNightManager = Cast<AA1DayNightManager>(Actor))
        {
            DayNightManager->SetPlayerSleeping(LyraCharacter, true);
        }
    }

    break;

    case EBedState::Occupied:
    {
        LyraCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

        PlayScreenFadeEffect(true);

        FTransform ReturnTransform = BedActor->GetPlayerReturnTransform();
        LyraCharacter->SetActorTransform(ReturnTransform);

        BedActor->SetBedState(EBedState::Empty);

        // �߰�: ħ�뿡 ĳ���� ���� ����
        BedActor->SetOccupyingCharacter(nullptr);

        // �߰�: DayNightManager���� ħ�� ��� ����
        if (AA1DayNightManager* DayNightManager = Cast<AA1DayNightManager>(Actor))
        {
            DayNightManager->SetPlayerSleeping(LyraCharacter, false);
        }
    }

    break;
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
