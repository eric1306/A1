// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1EquipmentSlotWidget.h"

#include "A1EquipmentSlotsWidget.h"
#include "A1EquipmentEntryWidget.h"
#include "Item/Managers/A1EquipManagerComponent.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Data/A1ItemData.h"
#include "Data/A1UIData.h"
#include "Item/A1ItemInstance.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1EquipmentSlotWidget)

UA1EquipmentSlotWidget::UA1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UA1EquipmentSlotWidget::Init(EItemSlotType InItemSlotType, UA1EquipmentManagerComponent* InEquipmentManager)
{
	check(InItemSlotType != EItemSlotType::Count && InEquipmentManager != nullptr);

	ItemSlotType = InItemSlotType;
	EquipmentManager = InEquipmentManager;
}

void UA1EquipmentSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Image_BaseIcon)
	{
		Image_BaseIcon->SetBrushFromTexture(BaseIconTexture, true);
	}
}

bool UA1EquipmentSlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

bool UA1EquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

void UA1EquipmentSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	OnDragEnded();
}

void UA1EquipmentSlotWidget::OnDragEnded()
{
	bAlreadyHovered = false;

	Image_Red->SetVisibility(ESlateVisibility::Hidden);
	Image_Green->SetVisibility(ESlateVisibility::Hidden);
}

void UA1EquipmentSlotWidget::OnEquipmentEntryChange(EItemHandType InItemHandType, UA1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (EntryWidget)
	{
		Overlay_Entry->RemoveChild(EntryWidget);
		EntryWidget = nullptr;
	}

	if (InItemInstance)
	{
		TSubclassOf<UA1EquipmentEntryWidget> EntryWidgetClass = UA1UIData::Get().EquipmentEntryWidgetClass;
		EntryWidget = CreateWidget<UA1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);

		UOverlaySlot* OverlaySlot = Overlay_Entry->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);

		EntryWidget->Init(InItemInstance, InItemCount, UA1EquipManagerComponent::ConvertToEquipmentSlotType(InItemHandType), EquipmentManager);

		Image_BaseIcon->SetRenderOpacity(0.f);
	}
	else
	{
		Image_BaseIcon->SetRenderOpacity(1.f);
	}
}

void UA1EquipmentSlotWidget::Test(UA1ItemInstance* InItemInstance)
{
	if (InItemInstance == nullptr)
		return;
	else
	{
		OnEquipmentEntryChange(EItemHandType::RightHand, InItemInstance, 1);
	}
}
