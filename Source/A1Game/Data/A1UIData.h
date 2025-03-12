// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "A1Define.h"
#include "GameplayTagContainer.h"
#include "A1UIData.generated.h"

class UImage;
class UA1ItemDragWidget;
class UA1ItemHoverWidget;
class UA1EquipmentEntryWidget;
class UA1InventorySlotWidget;
class UA1InventoryEntryWidget;
class UA1InventoryValidWidget;

USTRUCT(BlueprintType)
struct FA1UIInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly)
	FText Title;

	UPROPERTY(EditDefaultsOnly)
	FText Content;
};

UCLASS(BlueprintType, Const, meta=(DisplayName="A1 UI Data"))
class A1GAME_API UA1UIData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UA1UIData& Get();

public: 
	const FA1UIInfo& GetTagUIInfo(FGameplayTag Tag) const;

public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint UnitInventorySlotSize = FIntPoint::ZeroValue;

public:
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UA1ItemDragWidget> DragWidgetClass;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UA1ItemHoverWidget> ItemHoverWidgetClass;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UA1EquipmentEntryWidget> EquipmentEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UA1InventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UA1InventoryEntryWidget> InventoryEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UA1InventoryValidWidget> InventoryValidWidgetClass;

private:
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Tag UI Infos"))
	TMap<FGameplayTag, FA1UIInfo> TagUIInfos;
};
