// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "A1EquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1EquipmentEntryWidget)

UA1EquipmentEntryWidget::UA1EquipmentEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UA1EquipmentEntryWidget::Init(UA1ItemInstance* InItemInstance, int32 InItemCount, EEquipmentSlotType InEquipmentSlotType, UA1EquipmentManagerComponent* InEquipmentManager)
{
	if (InEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || InEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || InEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	RefreshUI(InItemInstance, InItemCount);

	EquipmentSlotType = InEquipmentSlotType;
	EquipmentManager = InEquipmentManager;
}