// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1InventorySlotsWidget.h"

#include "A1InventorySlotWidget.h"
#include "A1InventoryEntryWidget.h"
#include "A1InventoryValidWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Item/A1ItemInstance.h"
#include "Item/Managers/A1InventoryManagerComponent.h"
#include "Data/A1UIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1InventorySlotsWidget)

UA1InventorySlotsWidget::UA1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TitleText = FText::FromString(TEXT("Title"));
}

void UA1InventorySlotsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text_Title->SetText(TitleText);
}

void UA1InventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UA1InventorySlotsWidget::NativeDestruct()
{
	DestructUI();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(MessageListenerHandle);

	Super::NativeDestruct();
}

bool UA1InventorySlotsWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

void UA1InventorySlotsWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

bool UA1InventorySlotsWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

void UA1InventorySlotsWidget::ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message)
{
	if (Message.InventoryManager == nullptr)
		return;

	InventoryManager = Message.InventoryManager;

	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	const int32 InventorySlotNum = InventorySlotCount.X * InventorySlotCount.Y;
	SlotWidgets.SetNum(InventorySlotNum);
	ValidWidgets.SetNum(InventorySlotNum);
	EntryWidgets.SetNum(InventorySlotNum);

	for (int32 y = 0; y < InventorySlotCount.Y; y++)
	{
		for (int32 x = 0; x < InventorySlotCount.X; x++)
		{
			TSubclassOf<UA1InventorySlotWidget> SlotWidgetClass = UA1UIData::Get().InventorySlotWidgetClass;
			UA1InventorySlotWidget* SlotWidget = CreateWidget<UA1InventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
			SlotWidgets[y * InventorySlotCount.X + x] = SlotWidget;
			GridPanel_Slots->AddChildToUniformGrid(SlotWidget, y, x);

			TSubclassOf<UA1InventoryValidWidget> ValidWidgetClass = UA1UIData::Get().InventoryValidWidgetClass;
			UA1InventoryValidWidget* ValidWidget = CreateWidget<UA1InventoryValidWidget>(GetOwningPlayer(), ValidWidgetClass);
			ValidWidgets[y * InventorySlotCount.X + x] = ValidWidget;
			GridPanel_ValidSlots->AddChildToUniformGrid(ValidWidget, y, x);
		}
	}

	const TArray<FA1InventoryEntry>& Entries = InventoryManager->GetAllEntries();
	for (int32 i = 0 ; i< Entries.Num();i++)
	{
		const FA1InventoryEntry& Entry = Entries[i];
		if (UA1ItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			FIntPoint ItemSlotPos = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
			OnInventoryEntryChanged(ItemSlotPos, ItemInstance, Entry.GetItemCount());
		}
	}

	EntryChangedDelegateHandle = InventoryManager->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

void UA1InventorySlotsWidget::DestructUI()
{
	InventoryManager->OnInventoryEntryChanged.Remove(EntryChangedDelegateHandle);
	EntryChangedDelegateHandle.Reset();

	CanvasPanel_Entries->ClearChildren();
	EntryWidgets.Reset();

	GridPanel_ValidSlots->ClearChildren();
	ValidWidgets.Reset();

	GridPanel_Slots->ClearChildren();
	SlotWidgets.Reset();
}

void UA1InventorySlotsWidget::OnInventoryEntryChanged(const FIntPoint& InItemSlotPos, UA1ItemInstance* InItemInstance, int32 InItemCount)
{
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	int32 SlotIndex = InItemSlotPos.Y * InventorySlotCount.X + InItemSlotPos.X;

	if (UA1InventoryEntryWidget* EntryWidget = EntryWidgets[SlotIndex])
	{
		UA1ItemInstance* ItemInstance = EntryWidget->GetItemInstance();
		if (ItemInstance && ItemInstance == ItemInstance)
		{
			EntryWidget->RefreshItemCount(InItemCount);
			return;
		}
		CanvasPanel_Entries->RemoveChild(EntryWidget);
		EntryWidgets[SlotIndex] = nullptr;
	}

	if (InItemInstance)
	{
		FIntPoint UnitInventorySlotSize = UA1UIData::Get().UnitInventorySlotSize;

		TSubclassOf<UA1InventoryEntryWidget> EntryWidgetClass = UA1UIData::Get().InventoryEntryWidgetClass;
		UA1InventoryEntryWidget* EntryWidget = CreateWidget<UA1InventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[SlotIndex] = EntryWidget;

		EntryWidget->Init(this, InItemInstance, InItemCount);

		UCanvasPanelSlot* CanvasPanelSlot = CanvasPanel_Entries->AddChildToCanvas(EntryWidget);
		CanvasPanelSlot->SetAutoSize(true);
		CanvasPanelSlot->SetPosition(FVector2D(InItemSlotPos.X * UnitInventorySlotSize.X, InItemSlotPos.Y * UnitInventorySlotSize.Y));
	}
}
