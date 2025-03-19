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
#include "Item/Fragments/A1ItemFragment_Equipable_Weapon.h"
#include "Item/Fragments/A1ItemFragment_Equipable_Utility.h"
#include "Item/Managers/A1EquipManagerComponent.h"
#include "Item/Managers/A1EquipmentManagerComponent.h"
#include "Item/Managers/A1InventoryManagerComponent.h"
#include "Item/Managers/A1ItemManagerComponent.h"
#include "System/LyraAssetManager.h"
#include "UI/Item/A1ItemDragDrop.h"

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
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (bAlreadyHovered)
		return true;

	bAlreadyHovered = true;

	UA1ItemDragDrop* ItemDragDrop = Cast<UA1ItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;

	UA1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;

	bool bIsValid = false;
	EEquipmentSlotType ToEquipmentSlotType = UA1EquipManagerComponent::ConvertToEquipmentSlotType(ItemSlotType);
	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	if (UA1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
	{
		if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
		{
			FIntPoint OutToItemSlotPos;
			bIsValid = EquipmentManager->CanSwapEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType, OutToItemSlotPos);
		}
		else
		{
			bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType) > 0;
		}
	}
	else if (UA1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
	{
		if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
		{
			bIsValid = EquipmentManager->CanSwapEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
		}
		else
		{
			bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType) > 0;
		}
	}

	if (bIsValid)
	{
		Image_Red->SetVisibility(ESlateVisibility::Hidden);
		Image_Green->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_Red->SetVisibility(ESlateVisibility::Visible);
		Image_Green->SetVisibility(ESlateVisibility::Hidden);
	}

	return true;
}

bool UA1EquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	OnDragEnded();

	UA1ItemDragDrop* ItemDragDrop = Cast<UA1ItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;

	if (UA1ItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget)
	{
		FromEntryWidget->RefreshWidgetOpacity(true);
	}

	UA1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;

	UA1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UA1ItemManagerComponent>();
	if (ItemManager == nullptr)
		return false;

	EEquipmentSlotType ToEquipmentSlotType = UA1EquipManagerComponent::ConvertToEquipmentSlotType(ItemSlotType);
	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	const UA1ItemFragment* FromItemFragment = nullptr;
	switch (ItemSlotType)
	{
	case EItemSlotType::Left:	FromItemFragment = FromItemInstance->FindFragmentByClass<UA1ItemFragment_Equipable_Utility>();	break;
	case EItemSlotType::Right:	FromItemFragment = FromItemInstance->FindFragmentByClass<UA1ItemFragment_Equipable_Weapon>();	break;
	}
	if (FromItemFragment == nullptr)
		return false;

	if (UA1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
	{
		ItemManager->Server_InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
	}
	else if (UA1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
	{
		ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
	}

	return true;
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
		Image_BaseIcon->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		Image_BaseIcon->SetRenderOpacity(1.f);
		Image_BaseIcon->SetVisibility(ESlateVisibility::Visible);
	}
}
