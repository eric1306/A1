// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Interaction/Abilities/A1GameplayAbility_Interact_Object.h"
#include "A1GameplayAbility_Interact_Storage.generated.h"

/**
 * 
 */
UCLASS()
class UA1GameplayAbility_Interact_Storage : public UA1GameplayAbility_Interact_Object
{
	GENERATED_BODY()
public:
    UA1GameplayAbility_Interact_Storage(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // 라인 트레이스로 아이템 찾기
    UFUNCTION()
    void PerformPickupLineTrace();

    // 아이템 획득 시도
    UFUNCTION()
    void TryPickupItem(AActor* ItemActor);
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    float PickupRange = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    bool bShowDebugTrace = false;

};
