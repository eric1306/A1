#include "A1CosmeticManagerComponent.h"

#include "A1Define.h"
#include "Actors/A1ArmorBase.h"
#include "Character/LyraCharacter.h"
#include "Data/A1CharacterData.h"
//#include "Item/Fragments/A1ItemFragment_Equipable_Armor.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1CosmeticManagerComponent)

UA1CosmeticManagerComponent::UA1CosmeticManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UA1CosmeticManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeManager();
}

void UA1CosmeticManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HeadSlot)
	{
		HeadSlot->DestroyComponent();
	}

	for (UChildActorComponent* CosmeticSlot : CosmeticSlots)
	{
		if (CosmeticSlot)
		{
			CosmeticSlot->DestroyComponent();
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UA1CosmeticManagerComponent::RefreshArmorMesh(EArmorType ArmorType /*, const UD1ItemFragment_Equipable_Armor* ArmorFragment*/)
{
	if (ArmorType == EArmorType::Count)
		return;

	InitializeManager();

	/*if (ArmorFragment)
	{
		if (ArmorFragment == nullptr || ArmorFragment->ArmorType != ArmorType)
			return;

		SetPrimaryArmorMesh(ArmorType, ArmorFragment->ArmorMesh);

		if (ArmorType == EArmorType::Chest)
		{
			if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)EArmorType::Legs])
			{
				if (AA1ArmorBase* CosmeticActor = Cast<AA1ArmorBase>(CosmeticSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(ArmorFragment->bIsFullBody);
				}
			}
		}
		else if (ArmorType == EArmorType::Helmet)
		{
			if (HeadSlot)
			{
				if (AA1ArmorBase* CosmeticActor = Cast<AA1ArmorBase>(HeadSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(true);
				}
			}
		}
	}
	else*/
	{
		//if (ArmorType == EArmorType::Chest)
		//{
		//	if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)EArmorType::Legs])
		//	{
		//		if (AA1ArmorBase* CosmeticActor = Cast<AA1ArmorBase>(CosmeticSlot->GetChildActor()))
		//		{
		//			CosmeticActor->ShouldUseSecondaryMesh(false);
		//		}
		//	}
		//}
		//else if (ArmorType == EArmorType::Helmet)
		//{
		//	if (HeadSlot)
		//	{
		//		if (AA1ArmorBase* CosmeticActor = Cast<AA1ArmorBase>(HeadSlot->GetChildActor()))
		//		{
		//			CosmeticActor->ShouldUseSecondaryMesh(false);
		//		}
		//	}
		//}

		//SetPrimaryArmorMesh(ArmorType, nullptr);
	}
}

void UA1CosmeticManagerComponent::SetPrimaryArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr)
{
	if (ArmorType == EArmorType::Count)
		return;

	InitializeManager();

	//if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)ArmorType])
	//{
	//	if (AA1ArmorBase* CosmeticActor = Cast<AA1ArmorBase>(CosmeticSlot->GetChildActor()))
	//	{
	//		if (ArmorMeshPtr.IsNull())
	//		{
	//			CosmeticActor->SetPrimaryArmorMesh(nullptr);
	//		}
	//		else
	//		{
	//			USkeletalMesh* ArmorMesh = ULyraAssetManager::GetAsset<USkeletalMesh>(ArmorMeshPtr);
	//			CosmeticActor->SetPrimaryArmorMesh(ArmorMesh);
	//		}
	//	}
	//}
}

void UA1CosmeticManagerComponent::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	if (HeadSlot)
	{
		if (AA1ArmorBase* CosmeticActor = Cast<AA1ArmorBase>(HeadSlot->GetChildActor()))
		{
			OutMeshComponents.Add(CosmeticActor->GetMeshComponent());
		}
	}

	for (UChildActorComponent* CosmeticSlot : CosmeticSlots)
	{
		if (CosmeticSlot)
		{
			if (AA1ArmorBase* CosmeticActor = Cast<AA1ArmorBase>(CosmeticSlot->GetChildActor()))
			{
				OutMeshComponents.Add(CosmeticActor->GetMeshComponent());
			}
		}
	}
}

void UA1CosmeticManagerComponent::InitializeManager()
{
	if (bInitialized)
		return;

	bInitialized = true;

	const int32 ArmorTypeCount = (int32)EArmorType::Count;
	CosmeticSlots.SetNumZeroed(ArmorTypeCount);

	check(CosmeticSlotClass);
	check(CharacterSkinType != ECharacterSkinType::Count);

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (Character->IsNetMode(NM_DedicatedServer) == false)
		{
			const UA1CharacterData& CharacterData = ULyraAssetManager::Get().GetCharacterData();
			const FA1DefaultArmorMeshSet& DefaultArmorMeshSet = CharacterData.GetDefaultArmorMeshSet(CharacterSkinType);

			HeadSlot = SpawnCosmeticSlotActor(DefaultArmorMeshSet.DefaultMesh, NAME_None, nullptr);

			for (int32 i = 0; i < (int32)EArmorType::Count; i++)
			{
				EArmorType ArmorType = (EArmorType)i;
				FName SkinMaterialSlotName;
				TSoftObjectPtr<UMaterialInterface> SkinMaterial;

				if (ArmorType == EArmorType::Helmet || ArmorType == EArmorType::Chest || ArmorType == EArmorType::Hands)
				{
					SkinMaterialSlotName = FName("UpperBody");
					SkinMaterial = DefaultArmorMeshSet.BodySkinMaterial;
				}
				else if (ArmorType == EArmorType::Legs || ArmorType == EArmorType::Foot)
				{
					SkinMaterialSlotName = FName("LowerBody");
					SkinMaterial = DefaultArmorMeshSet.BodySkinMaterial;
				}

				CosmeticSlots[i] = SpawnCosmeticSlotActor(DefaultArmorMeshSet.DefaultMeshEntries[i], SkinMaterialSlotName, SkinMaterial);
			}
		}
	}
}

UChildActorComponent* UA1CosmeticManagerComponent::SpawnCosmeticSlotActor(TSoftObjectPtr<USkeletalMesh> InDefaultMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial)
{
	UChildActorComponent* CosmeticComponent = nullptr;

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		USceneComponent* ComponentToAttachTo = Character->GetMesh();
		CosmeticComponent = NewObject<UChildActorComponent>(Character);
		CosmeticComponent->SetupAttachment(ComponentToAttachTo);
		CosmeticComponent->SetChildActorClass(CosmeticSlotClass);
		CosmeticComponent->RegisterComponent();

		if (AA1ArmorBase* SpawnedActor = Cast<AA1ArmorBase>(CosmeticComponent->GetChildActor()))
		{
			if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
			{
				SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
			}

			SpawnedActor->InitializeActor(InDefaultMesh, InSkinMaterialSlotName, InSkinMaterial);
		}
	}

	return CosmeticComponent;
}
