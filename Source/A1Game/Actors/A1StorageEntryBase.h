// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Interaction/A1WorldInteractable.h"
#include "A1StorageEntryBase.generated.h"

enum class EEquipmentSlotType : uint8;
enum class EItemRarity : uint8;
class UArrowComponent;
class AA1EquipmentBase;
class UBoxComponent;

UENUM()
enum class EItemEntryState
{
	None,
	Exist
};
/**
 * 
 */
UCLASS()
class AA1StorageEntryBase : public AA1WorldInteractable
{
	GENERATED_BODY()
public:
	AA1StorageEntryBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	virtual FA1InteractionInfo GetPreInteractionInfo(const FA1InteractionQuery& InteractionQuery) const override;

	void SetItemTransform(int32 ItemTemplateID, EItemRarity ItemRarity, EEquipmentSlotType SlotToCheck);

	UFUNCTION()
	void SetItemInput();

	UFUNCTION()
	void SetItemOutput();

	FORCEINLINE EItemEntryState GetItemEntryState() const { return ItemState; }
	FORCEINLINE void SetItemEntryState(EItemEntryState InItemState) { ItemState = InItemState; }

protected:
	//�ʿ��� ���
	/*
	 * 1. �������� ������ ��ġ�� �������� ���
	 * 2. �������� TryInteract�� ������ ���� �����ϴ� ��� -> overlap���� ����
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere, Category = "Entry", Replicated)
	TObjectPtr<UBoxComponent> ItemTrigger;

	UPROPERTY(EditDefaultsOnly, Category = "Entry|Info")
	FA1InteractionInfo ItemInputInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Entry|Info")
	FA1InteractionInfo ItemOutputInfo;

	UPROPERTY(VisibleAnywhere, Category = "Entry", Replicated)
	TObjectPtr<AA1EquipmentBase> CachedItem;

	UPROPERTY(VisibleAnywhere, Category = "Entry")
	EItemEntryState ItemState = EItemEntryState::None;
};
