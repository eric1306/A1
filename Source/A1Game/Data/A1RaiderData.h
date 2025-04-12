// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "A1Define.h"
#include "Engine/DataAsset.h"
#include "A1RaiderData.generated.h"

USTRUCT(BlueprintType)
struct FA1RaiderMeshSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UMaterialInterface>> BodySkinMaterial;

public:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> DefaultMesh;
};

UCLASS(BlueprintType)
class A1GAME_API UA1RaiderData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UA1RaiderData& Get();

public:
#if WITH_EDITOR
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif // WITH_EDITOR

public:
	const FA1RaiderMeshSet& GetRaiderDataSet(ERaiderType RaiderType) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<ERaiderType, FA1RaiderMeshSet> RaiderDataMap;

//public:
//	UPROPERTY(EditDefaultsOnly)
//	TArray<FA1DefaultItemEntry> DefaultItemEntries;
};
