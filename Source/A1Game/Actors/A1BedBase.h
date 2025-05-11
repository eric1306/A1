// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "A1SpaceshipBase.h"
#include "Interaction/A1WorldInteractable.h"
#include "A1BedBase.generated.h"

class ALyraCharacter;
class UArrowComponent;

UENUM(BlueprintType)
enum class EBedState : uint8
{
    Empty,
    Occupied
};

/**
 * Basic Bed Class Interaction Object.
 */
UCLASS()
class AA1BedBase : public AA1WorldInteractable, public IA1SpaceshipComponent
{
    GENERATED_BODY()
public:
    AA1BedBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void BeginPlay() override;

public:
    virtual FA1InteractionInfo GetPreInteractionInfo(const FA1InteractionQuery& InteractionQuery) const override;
    virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;

    //IA1SpaceshipInterface
    virtual void RegisterWithSpaceship(class AA1SpaceshipBase* Spaceship) override;
    virtual ESpaceshipComponentType GetComponentType() const override { return ESpaceshipComponentType::Bed; }

public:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void SetBedState(EBedState NewBedState);

    UFUNCTION(BlueprintCallable)
    EBedState GetBedState() const { return BedState; }

    //return player lying transform
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FTransform GetLayDownTransform() const;

    FRotator GetArrowComponentVector() const;

    //  save player recover transform
    UFUNCTION(BlueprintCallable)
    void StorePlayerReturnTransform(const FTransform& PlayerTransform);

    // get player return transform
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FTransform GetPlayerReturnTransform() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsPlayerSleeping() const { return BedState == EBedState::Occupied; }

    // ĳ���͸� ����� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Bed")
    void WakeUpOccupyingCharacter();

    // ĳ���Ͱ� ħ�뿡 ���� �� ȣ��
    UFUNCTION(BlueprintCallable, Category = "Bed")
    void SetOccupyingCharacter(AActor* Character);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_Wakeup();

    FORCEINLINE bool bIsOccupyingCharacterExist() const { return OccupyingCharacter != nullptr; }
    FORCEINLINE AActor* GetOccupyingCharacter() const { return OccupyingCharacter; }

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnBedStateChanged(EBedState NewBedState);

    void ActivateDecreaseWeight();
    void DeactivateDecreaseWeight();

private:
    UFUNCTION()
    void OnRep_BedState();

    void SetupTags();

    void OnHealthChanged(AActor* InInstigator, float OldValue, float NewValue);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Bed|GameplayEffect")
    TSubclassOf<class UGameplayEffect> DecreaseWeightEffect;
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BedState)
    EBedState BedState = EBedState::Empty;

    UPROPERTY(EditDefaultsOnly, Category = "Bed|Info")
    FA1InteractionInfo EmptyInteractionInfo;

    UPROPERTY(EditDefaultsOnly, Category = "Bed|Info")
    FA1InteractionInfo OccupiedInteractionInfo;

    UPROPERTY(EditDefaultsOnly, Category = "Bed|Offset")
    FTransform LayDownOffset;

    UPROPERTY(Replicated)
    FTransform StoredPlayerTransform;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UArrowComponent> ArrowComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Bed|Recovery")
    float FatigueRecoveryRate = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Bed|Recovery")
    float HealthRecoveryRate = 2.0f;


    // ħ�뿡 �����ִ� ĳ���� ���� �߰�
    UPROPERTY(Replicated)
    TObjectPtr<AActor> OccupyingCharacter;
    // �÷��̾ �ִ�� �����ִٸ� ����� Ÿ�̸�
    FTimerHandle MaxLyingTimerHandle;
    //�ִ� �������� �� �ִ� �ð�
    float MaxLyingTimeRate;
    //�÷��̾ ħ�뿡 �������� �� Weight ���ҽ�Ű�� Timer
    FTimerHandle DecreaseWeightTimerHandle;

    float DecreaseWeightTimeRate;
};
