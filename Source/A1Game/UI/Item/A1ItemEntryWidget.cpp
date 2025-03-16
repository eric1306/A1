// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#include "A1ItemEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/A1ItemData.h"
#include "Data/A1UIData.h"
#include "Item/A1ItemInstance.h"
#include "Item/A1ItemTemplate.h"

UA1ItemEntryWidget::UA1ItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UA1ItemEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UA1ItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UA1ItemEntryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UA1ItemEntryWidget::RefreshWidgetOpacity(bool bClearlyVisible)
{
	SetRenderOpacity(bClearlyVisible ? 1.f : 0.5f);
}

void UA1ItemEntryWidget::RefreshUI(UA1ItemInstance* NewItemInstance, int32 NewItemCount)
{
	if (NewItemInstance == nullptr || NewItemCount < 1)
		return;

	ItemInstance = NewItemInstance;
	ItemCount = NewItemCount;

	const UA1ItemTemplate& ItemTemplate = UA1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	Image_Icon->SetBrushFromTexture(ItemTemplate.IconTexture, true);
	Text_Count->SetText(ItemCount <= 1 ? FText::GetEmpty() : FText::AsNumber(ItemCount));

	UTexture2D* RarityTexture = UA1UIData::Get().GetEntryRarityTexture(ItemInstance->GetItemRarity());
	Image_RarityCover->SetBrushFromTexture(RarityTexture, true);
}

void UA1ItemEntryWidget::RefreshItemCount(int32 NewItemCount)
{
	if (NewItemCount < 1)
		return;

	ItemCount = NewItemCount;
	Text_Count->SetText(ItemCount <= 1 ? FText::GetEmpty() : FText::AsNumber(ItemCount));
}
