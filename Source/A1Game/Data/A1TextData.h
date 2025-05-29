#pragma once

#include "A1TextData.generated.h"


USTRUCT()
struct FTextSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FText> TextEntries;
};

UCLASS(Const, CollapseCategories, meta=(DisplayName="A1 Text Data"))
class UA1TextData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UA1TextData& GetNoticeData();
	static const UA1TextData& GetGuideData();
	
protected:
#if WITH_EDITOR
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	
public:
	const FTextSet& GetTextSetByLabel(const FName& Label) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FTextSet> TextGroupNameToSet;
};
