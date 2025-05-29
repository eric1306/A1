﻿#pragma once

#include "A1CmdData.generated.h"

USTRUCT()
struct FCmdTextEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName TextLabel;

	UPROPERTY(EditDefaultsOnly)
	FText Text;
};

USTRUCT()
struct FCmdTextSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FCmdTextEntry> TextEntries;
};

UCLASS(Const, CollapseCategories, meta=(DisplayName="A1 Cmd Data"))
class UA1CmdData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UA1CmdData& Get();
	
protected:
#if WITH_EDITOR
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	
public:
	const FCmdTextSet& GetTextSetByLabel(const FName& Label) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FCmdTextSet> TextGroupNameToSet;
};
