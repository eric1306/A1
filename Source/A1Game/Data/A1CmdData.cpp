#include "A1CmdData.h"

#include "UObject/ObjectSaveContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1CmdData)

const UA1CmdData& UA1CmdData::Get()
{
	return ULyraAssetManager::Get().GetCmdData();
}

#if WITH_EDITOR
void UA1CmdData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	TextGroupNameToSet.KeySort([](const FName& A, const FName& B)
	{
		return (A.Compare(B) < 0);
	});
	
}

EDataValidationResult UA1CmdData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (const auto& Pair : TextGroupNameToSet)
	{
		const FCmdTextSet& TextSet = Pair.Value;
		for (int32 i = 0; i < TextSet.TextEntries.Num(); i++)
		{
			const FCmdTextEntry& CmdTextEntry = TextSet.TextEntries[i];
			if (CmdTextEntry.TextLabel.IsNone())
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("CmdTextLabel is None : [Group Name : %s] - [Entry Index : %d]"), *Pair.Key.ToString(), i)));
				Result = EDataValidationResult::Invalid;
			}

			if (CmdTextEntry.Text.IsEmpty())
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("CmdText is Invalid : [Group Name : %s] - [Entry Index : %d]"), *Pair.Key.ToString(), i)));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif // WITH_EDITOR

const FCmdTextSet& UA1CmdData::GetTextSetByLabel(const FName& Label) const
{
	const FCmdTextSet* TextSet = TextGroupNameToSet.Find(Label);
	ensureAlwaysMsgf(TextSet, TEXT("Can't find CmdText Set from Label [%s]."), *Label.ToString());
	return *TextSet;
}
