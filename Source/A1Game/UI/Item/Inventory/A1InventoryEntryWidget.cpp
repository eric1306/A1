// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1InventoryEntryWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SizeBox.h"
#include "Data/A1UIData.h"
#include "Data/A1ItemData.h"
#include "Item/A1ItemInstance.h"
#include "Item/A1ItemTemplate.h"

UA1InventoryEntryWidget::UA1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UA1InventoryEntryWidget::Init(UA1InventorySlotsWidget* InSlotsWidget, UA1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (InSlotsWidget == nullptr || InItemInstance == nullptr)
		return;

	SlotsWidget = InSlotsWidget;

	FIntPoint UnitInventorySlotSize = UA1UIData::Get().UnitInventorySlotSize;
	const UA1ItemTemplate& ItemTemplate = UA1ItemData::Get().FindItemTemplateByID(InItemInstance->GetItemTemplateID());

	FVector2D WidgetSize = FVector2D(ItemTemplate.SlotCount.X * UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * UnitInventorySlotSize.Y);
	SizeBox_Root->SetWidthOverride(WidgetSize.X);
	SizeBox_Root->SetHeightOverride(WidgetSize.Y);

	RefreshUI(InItemInstance, InItemCount);
}
