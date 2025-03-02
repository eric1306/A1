#pragma once

#include "A1Define.h"
#include "Engine/DataAsset.h"
#include "A1CharacterData.generated.h"

USTRUCT(BlueprintType)
struct FA1DefaultArmorMeshSet
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
class A1GAME_API UA1CharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UA1CharacterData& Get();

public:
#if WITH_EDITOR
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif // WITH_EDITOR

public:
	const FA1DefaultArmorMeshSet& GetDefaultArmorMeshSet(ECharacterSkinType CharacterSkinType) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<ECharacterSkinType, FA1DefaultArmorMeshSet> DefaultArmorMeshMap;
};