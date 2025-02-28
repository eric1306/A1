#include "A1ArmorBase.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1ArmorBase)

AA1ArmorBase::AA1ArmorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ArmorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ArmorMeshComponent");
	ArmorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ArmorMeshComponent);
}

void AA1ArmorBase::InitializeActor(TSoftObjectPtr<USkeletalMesh> InDefaultArmorMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial)
{
	DefaultArmorMesh = InDefaultArmorMesh;

	if (InSkinMaterialSlotName.IsNone() == false && InSkinMaterial.IsNull() == false)
	{
		SkinMaterialSlotName = InSkinMaterialSlotName;
		SkinMaterial = InSkinMaterial;
	}

	SetArmorMesh(DefaultArmorMesh);
}


void AA1ArmorBase::SetArmorMesh(TSoftObjectPtr<USkeletalMesh> InArmorMesh)
{
	USkeletalMesh* LoadedArmorMesh = nullptr;
	if (InArmorMesh.IsNull() == false)
	{
		LoadedArmorMesh = ULyraAssetManager::GetAsset<USkeletalMesh>(InArmorMesh);
	}

	ArmorMeshComponent->SetSkeletalMesh(LoadedArmorMesh);
	ArmorMeshComponent->EmptyOverrideMaterials();

	if (SkinMaterialSlotName.IsNone() == false && SkinMaterial.IsNull() == false)
	{
		UMaterialInterface* LoadedMaterial = ULyraAssetManager::GetAsset<UMaterialInterface>(SkinMaterial);
		ArmorMeshComponent->SetMaterialByName(SkinMaterialSlotName, LoadedMaterial);
	}
}