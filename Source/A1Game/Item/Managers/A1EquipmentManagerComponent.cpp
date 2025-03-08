#include "A1EquipmentManagerComponent.h"

#include "A1EquipManagerComponent.h"
//#include "A1InventoryManagerComponent.h"
//#include "A1ItemManagerComponent.h"
#include "Character/LyraCharacter.h"
#include "Data/A1ItemData.h"
#include "Engine/ActorChannel.h"
#include "Item/A1ItemInstance.h"
#include "Item/Fragments/A1ItemFragment_Equipable_Armor.h"
#include "Item/Fragments/A1ItemFragment_Equipable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Player/LyraPlayerController.h"
#include "Player/LyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1EquipmentManagerComponent)

void FA1EquipmentEntry::Init(UA1ItemInstance* InItemInstance, int32 InItemCount)
{
	check(InItemInstance && InItemCount > 0);

	UA1EquipManagerComponent* EquipManager = EquipmentManager->GetEquipManager();
	if (EquipManager == nullptr)
		return;

	const UA1ItemFragment_Equipable* EquippableFragment = InItemInstance->FindFragmentByClass<UA1ItemFragment_Equipable>();
	if (EquippableFragment == nullptr)
		return;
	
	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}
	
	ItemInstance = InItemInstance;
	
	const UA1ItemTemplate& ItemTemplate = UA1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	ItemCount = FMath::Clamp(InItemCount, 1, ItemTemplate.MaxStackCount);
	
	EquipManager->Equip(EquipmentSlotType, ItemInstance);
}

UA1ItemInstance* FA1EquipmentEntry::Reset()
{
	UA1EquipManagerComponent* EquipManager = EquipmentManager->GetEquipManager();
	if (EquipManager == nullptr)
		return nullptr;

	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}
	
	UA1ItemInstance* RemovedItemInstance = ItemInstance;
	ItemInstance = nullptr;
	ItemCount = 0;
	
	if (EquipmentManager->IsAllEmpty(EquipManager->GetCurrentEquipState()))
	{
		EquipManager->ChangeEquipState(EEquipState::Unarmed);
	}
	
	return RemovedItemInstance;
}

bool FA1EquipmentList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FA1EquipmentEntry, FA1EquipmentList>(Entries, DeltaParams,*this);
}

void FA1EquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 AddedIndex : AddedIndices)
	{
		FA1EquipmentEntry& Entry = Entries[AddedIndex];
		Entry.EquipmentManager = EquipmentManager;
		Entry.EquipmentSlotType = (EEquipmentSlotType)AddedIndex;
		
		if (Entry.GetItemInstance())
		{
			BroadcastChangedMessage((EEquipmentSlotType)AddedIndex, Entry.GetItemInstance(), Entry.GetItemCount());
		}
	}
}

void FA1EquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 ChangedIndex : ChangedIndices)
	{
		const FA1EquipmentEntry& Entry = Entries[ChangedIndex];
		BroadcastChangedMessage((EEquipmentSlotType)ChangedIndex, Entry.GetItemInstance(), Entry.GetItemCount());
	}
}

void FA1EquipmentList::BroadcastChangedMessage(EEquipmentSlotType EquipmentSlotType, UA1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (EquipmentManager->OnEquipmentEntryChanged.IsBound())
	{
		EquipmentManager->OnEquipmentEntryChanged.Broadcast(EquipmentSlotType, ItemInstance, ItemCount);
	}
}

UA1EquipmentManagerComponent::UA1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	bWantsInitializeComponent = true;
    SetIsReplicatedByDefault(true);
}

void UA1EquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FA1EquipmentEntry>& Entries = EquipmentList.Entries;
		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FA1EquipmentEntry& Entry = Entries[i];
			Entry.EquipmentManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
			EquipmentList.MarkItemDirty(Entry);
		}
	}
}

void UA1EquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

bool UA1EquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (FA1EquipmentEntry& Entry : EquipmentList.Entries)
	{
		UA1ItemInstance* ItemInstance = Entry.ItemInstance;
		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}
	
	return bWroteSomething;
}

void UA1EquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FA1EquipmentEntry& Entry : EquipmentList.Entries)
		{
			UA1ItemInstance* ItemInstance = Entry.GetItemInstance();
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
	}
}

int32 UA1EquipmentManagerComponent::CanAddEquipment(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (ItemTemplateID <= 0 || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return 0;
	
	if (ToEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || ToEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || ToEquipmentSlotType == EEquipmentSlotType::Count)
		return 0;
	
	const UA1ItemTemplate& ItemTemplate = UA1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const UA1ItemFragment_Equipable* FromEquippableFragment = ItemTemplate.FindFragmentByClass<UA1ItemFragment_Equipable>();
	if (FromEquippableFragment == nullptr)
		return 0;
	
	const UA1ItemInstance* ToItemInstance = GetItemInstance(ToEquipmentSlotType);
	const int32 ToItemCount = GetItemCount(ToEquipmentSlotType);

	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (ItemTemplateID != ToTemplateID)
			return 0;

		if (ItemRarity != ToItemInstance->GetItemRarity())
			return 0;
		
		if (ItemTemplate.MaxStackCount < 2)
			return 0;

		return FMath::Min(ItemCount + ToItemCount, ItemTemplate.MaxStackCount) - ToItemCount;
	}
	else
	{
		if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			const UA1ItemFragment_Equipable_Weapon* FromWeaponFragment = Cast<UA1ItemFragment_Equipable_Weapon>(FromEquippableFragment);
			EItemHandType FromWeaponHandType = FromWeaponFragment->ItemHandType;
		
			if (IsWeaponSlot(ToEquipmentSlotType))
			{
				if (FromWeaponHandType == EItemHandType::LeftHand || FromWeaponHandType == EItemHandType::RightHand)
				{
					return (GetItemInstance(EEquipmentSlotType::TwoHand) == nullptr) ? ItemCount : 0;
				}
				else if (FromWeaponHandType == EItemHandType::TwoHand)
				{
					return (GetItemInstance(EEquipmentSlotType::LeftHand) == nullptr && GetItemInstance(EEquipmentSlotType::RightHand) == nullptr) ? ItemCount : 0;
				}
			}
		}
	}

	return 0;
}

void UA1EquipmentManagerComponent::AddUnarmedEquipments(TSubclassOf<UA1ItemTemplate> LeftHandClass, TSubclassOf<UA1ItemTemplate> RightHandClass)
{
	check(HasAuthority());

	SetEquipment(EEquipmentSlotType::Unarmed_LeftHand, LeftHandClass, EItemRarity::Poor, 1);
	SetEquipment(EEquipmentSlotType::Unarmed_RightHand, RightHandClass, EItemRarity::Poor, 1);
}

void UA1EquipmentManagerComponent::AddEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, UA1ItemInstance* ItemInstance, int32 ItemCount)
{
	check(HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemCount <= 0)
		return;
	
	FA1EquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	
	if (Entry.GetItemInstance())
	{
		Entry.ItemCount += ItemCount;
		EquipmentList.MarkItemDirty(Entry);
	}
	else
	{
		if (ItemInstance == nullptr)
			return;
		
		Entry.Init(ItemInstance, ItemCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
		{
			AddReplicatedSubObject(ItemInstance);
		}
		
		EquipmentList.MarkItemDirty(Entry);
	}
}

UA1ItemInstance* UA1EquipmentManagerComponent::RemoveEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, int32 ItemCount)
{
	check(HasAuthority());

	FA1EquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	UA1ItemInstance* ItemInstance = Entry.GetItemInstance();

	Entry.ItemCount -= ItemCount;
	if (Entry.GetItemCount() <= 0)
	{
		UA1ItemInstance* RemovedItemInstance = Entry.Reset();
		if (IsUsingRegisteredSubObjectList() && RemovedItemInstance)
		{
			RemoveReplicatedSubObject(RemovedItemInstance);
		}
	}

	EquipmentList.MarkItemDirty(Entry);
	return ItemInstance;
}

void UA1EquipmentManagerComponent::SetEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<UA1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount)
{
	check(HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemTemplateClass == nullptr || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return;

	const int32 ItemTemplateID = UA1ItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
	const UA1ItemTemplate& ItemTemplate = UA1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	ItemCount = FMath::Clamp(ItemCount, 1, ItemTemplate.MaxStackCount);

	const UA1ItemFragment_Equipable* EquippableFragment = ItemTemplate.FindFragmentByClass<UA1ItemFragment_Equipable>();
	if (EquippableFragment == nullptr)
		return;

	FA1EquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	Entry.Reset();

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UA1ItemFragment_Equipable_Weapon* WeaponFragment = Cast<UA1ItemFragment_Equipable_Weapon>(EquippableFragment);
		EItemHandType ItemHandType = WeaponFragment->ItemHandType;

		if (IsWeaponSlot(EquipmentSlotType))
		{
			if (ItemHandType == EItemHandType::LeftHand || ItemHandType == EItemHandType::RightHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::TwoHand, 1);
			}
			else if (ItemHandType == EItemHandType::TwoHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::LeftHand, 1);
				RemoveEquipment_Unsafe(EEquipmentSlotType::RightHand, 1);
			}
		}
	}

	UA1ItemInstance* AddedItemInstance = NewObject<UA1ItemInstance>();
	AddedItemInstance->Init(ItemTemplateID, ItemRarity);
	Entry.Init(AddedItemInstance, ItemCount);

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && AddedItemInstance)
	{
		AddReplicatedSubObject(AddedItemInstance);
	}

	if (UA1EquipManagerComponent* EquipManager = GetEquipManager())
	{
		EEquipState EquipState = UA1EquipManagerComponent::ConvertToEquipState(EquipmentSlotType);
		if (EquipManager->GetCurrentEquipState() != EquipState)
		{
			EquipManager->ChangeEquipState(EquipState);
		}
	}

	EquipmentList.MarkItemDirty(Entry);
}

bool UA1EquipmentManagerComponent::IsWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Unarmed_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::TwoHand);
}

bool UA1EquipmentManagerComponent::IsSameEquipState(EEquipmentSlotType EquipmentSlotType, EEquipState WeaponEquipState)
{

	return (((EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand) && WeaponEquipState == EEquipState::Unarmed) ||
		((EquipmentSlotType == EEquipmentSlotType::LeftHand || EquipmentSlotType == EEquipmentSlotType::RightHand || EquipmentSlotType == EEquipmentSlotType::TwoHand) && WeaponEquipState != EEquipState::Unarmed));

}

const UA1ItemInstance* UA1EquipmentManagerComponent::FindPairItemInstance(const UA1ItemInstance* InBaseItemInstance, EEquipmentSlotType& OutEquipmentSlotType) const
{
	if (InBaseItemInstance == nullptr)
		return nullptr;

	const UA1ItemFragment_Equipable* BaseEquippableFragment = InBaseItemInstance->FindFragmentByClass<UA1ItemFragment_Equipable>();
	if (BaseEquippableFragment == nullptr)
		return nullptr;
	
	UA1ItemInstance* SelectedItemInstance = nullptr;

	if (BaseEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UA1ItemFragment_Equipable_Weapon* BaseWeaponFragment = Cast<UA1ItemFragment_Equipable_Weapon>(BaseEquippableFragment);
		
		const TArray<FA1EquipmentEntry>& Entries = EquipmentList.GetAllEntries();
		for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
		{
			if (IsWeaponSlot((EEquipmentSlotType)i) == false)
				continue;
		
			const FA1EquipmentEntry& Entry = Entries[i];
			if (UA1ItemInstance* ItemInstance = Entry.ItemInstance)
			{
				if (const UA1ItemFragment_Equipable_Weapon* EquippedWeaponFragment = ItemInstance->FindFragmentByClass<UA1ItemFragment_Equipable_Weapon>())
				{
					if ((BaseWeaponFragment->WeaponType == EquippedWeaponFragment->WeaponType) && (BaseWeaponFragment->ItemHandType == EquippedWeaponFragment->ItemHandType))
					{
						if (InBaseItemInstance == ItemInstance)
						{
							SelectedItemInstance = nullptr;
							break;
						}

						if (SelectedItemInstance == nullptr)
						{
							SelectedItemInstance = ItemInstance;
							OutEquipmentSlotType = (EEquipmentSlotType)i;
						}
					}
				}
			}
		}
	}
	else if (BaseEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		/*const UD1ItemFragment_Equipable_Armor* BaseArmorFragment = Cast<UD1ItemFragment_Equipable_Armor>(BaseEquippableFragment);
		OutEquipmentSlotType = UD1EquipManagerComponent::ConvertToEquipmentSlotType(BaseArmorFragment->ArmorType);
		SelectedItemInstance = GetItemInstance(OutEquipmentSlotType);*/
	}

	if (InBaseItemInstance == SelectedItemInstance)
	{
		SelectedItemInstance = nullptr;
	}

	return SelectedItemInstance;
}

bool UA1EquipmentManagerComponent::IsAllEmpty(EEquipState EquipState) const
{
	if (EquipState == EEquipState::Count)
		return true;

	if (EquipState == EEquipState::Unarmed)
		return false;

	bool bAllEmpty = true;
	for (EEquipmentSlotType SlotType : UA1EquipManagerComponent::GetEquipmentSlotsByEquipState(EquipState))
	{
		const FA1EquipmentEntry& Entry = EquipmentList.Entries[(int32)SlotType];
		if (Entry.ItemInstance)
		{
			bAllEmpty = false;
			break;
		}
	}
	return bAllEmpty;
}

ALyraCharacter* UA1EquipmentManagerComponent::GetCharacter() const
{
	return Cast<ALyraCharacter>(GetOwner());
}

ALyraPlayerController* UA1EquipmentManagerComponent::GetPlayerController() const
{
	if (ALyraCharacter* LyraCharacter = GetCharacter())
	{
		return LyraCharacter->GetLyraPlayerController();
	}
	return nullptr;
}

UA1EquipManagerComponent* UA1EquipmentManagerComponent::GetEquipManager() const
{
	UA1EquipManagerComponent* EquipManager = nullptr;
	if (ALyraCharacter* Character = GetCharacter())
	{
		EquipManager = Character->FindComponentByClass<UA1EquipManagerComponent>();
	}
	return EquipManager;
}

UA1ItemInstance* UA1EquipmentManagerComponent::GetItemInstance(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return nullptr;
	
	const TArray<FA1EquipmentEntry>& Entries = EquipmentList.GetAllEntries();
	const FA1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	
	return Entry.GetItemInstance();
}

int32 UA1EquipmentManagerComponent::GetItemCount(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	const TArray<FA1EquipmentEntry>& Entries = EquipmentList.GetAllEntries();
	const FA1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	
	return Entry.GetItemCount();
}

const TArray<FA1EquipmentEntry>& UA1EquipmentManagerComponent::GetAllEntries() const
{
	return EquipmentList.GetAllEntries();
}

void UA1EquipmentManagerComponent::GetAllWeaponItemInstances(TArray<UA1ItemInstance*>& OutItemInstances) const
{
	OutItemInstances.Reset();
	
	const TArray<FA1EquipmentEntry>& Entries = EquipmentList.GetAllEntries();

	for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
	{
		if (IsWeaponSlot((EEquipmentSlotType)i) == false)
			continue;
		
		const FA1EquipmentEntry& Entry = Entries[i];
		if (Entry.ItemInstance)
		{
			OutItemInstances.Add(Entry.ItemInstance);
		}
	}
}
