// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "A1ItemEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UA1ItemInstance;
class UA1ItemHoverWidget;

UCLASS()
class A1GAME_API UA1ItemEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UA1ItemEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void RefreshWidgetOpacity(bool bClearlyVisible);
	void RefreshUI(UA1ItemInstance* NewItemInstance, int32 NewItemCount);
	void RefreshItemCount(int32 NewItemCount);
	UA1ItemInstance* GetItemInstance() const { return ItemInstance; }

protected:
	UPROPERTY()
	TObjectPtr<UA1ItemInstance> ItemInstance;

	UPROPERTY()
	int32 ItemCount = 0;

	//UPROPERTY()
	//TObjectPtr<UA1ItemHoverWidget> HoverWidget;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_RarityCover;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Hover;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Count;
};
