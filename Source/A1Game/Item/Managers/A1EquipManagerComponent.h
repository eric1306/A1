﻿#pragma once

#include "A1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "A1EquipManagerComponent.generated.h"

class AA1PocketWorldAttachment;
class ALyraCharacter;
class ALyraPlayerController;
class AA1EquipmentBase;
class UA1ItemInstance;
class UA1EquipManagerComponent;
class UA1EquipmentManagerComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipStateChanged, EEquipState, EEquipState);

USTRUCT(BlueprintType)
struct FA1EquipEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
private:
	void Init(UA1ItemInstance* InItemInstance);
	
	void Equip();
	void Unequip();

public:
	UA1ItemInstance* GetItemInstance() const { return ItemInstance; }

	void SetEquipmentActor(AA1EquipmentBase* InEquipmentActor) { SpawnedEquipmentActor = InEquipmentActor; }
	AA1EquipmentBase* GetEquipmentActor() const { return SpawnedEquipmentActor; }
	
private:
	friend struct FA1EquipList;
	friend class UA1EquipManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UA1ItemInstance> ItemInstance;

private:
	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

private:
	UPROPERTY(NotReplicated)
	TObjectPtr<AA1EquipmentBase> SpawnedEquipmentActor;

	UPROPERTY(NotReplicated)
	TObjectPtr<AA1EquipmentBase> SpawnedPocketWorldActor;

private:
	UPROPERTY(NotReplicated)
	FLyraAbilitySet_GrantedHandles BaseAbilitySetHandles;

	UPROPERTY(NotReplicated)
	FActiveGameplayEffectHandle BaseStatHandle;

public:
	UPROPERTY(NotReplicated)
	TObjectPtr<UA1EquipManagerComponent> EquipManager;
};

USTRUCT(BlueprintType)
struct FA1EquipList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FA1EquipList() : EquipManager(nullptr) { }
	FA1EquipList(UA1EquipManagerComponent* InOwnerComponent) : EquipManager(InOwnerComponent) { }

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	void Equip(EEquipmentSlotType EquipmentSlotType, UA1ItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);
	
public:
	TArray<FA1EquipEntry>& GetAllEntries() { return Entries; }
	
private:
	friend class UA1EquipManagerComponent;

	UPROPERTY()
	TArray<FA1EquipEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UA1EquipManagerComponent> EquipManager;
};

template<>
struct TStructOpsTypeTraits<FA1EquipList> : public TStructOpsTypeTraitsBase2<FA1EquipList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS(BlueprintType)
class UA1EquipManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UA1EquipManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	
public:
	void Equip(EEquipmentSlotType EquipmentSlotType, UA1ItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);
	
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeEquipState(EEquipState NewEquipState);

	UFUNCTION(BlueprintCallable)
	bool CanChangeEquipState(EEquipState NewEquipState) const;

private:
	UFUNCTION()
	void OnRep_CurrentEquipState(EEquipState PrevEquipState);

	void BroadcastChangedMessage(EEquipState PrevEquipState, EEquipState NewEquipState);

public:
	ALyraCharacter* GetCharacter() const;
	ALyraPlayerController* GetPlayerController() const;
	
	TArray<FA1EquipEntry>& GetAllEntries();
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UA1EquipmentManagerComponent* GetEquipmentManager() const;

	static EEquipmentSlotType ConvertToEquipmentSlotType(EEquipState EquipState);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EItemHandType ItemHandType);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EItemSlotType ItemSlotType);

	static EEquipState ConvertToAnotherHand(EEquipmentSlotType EquipmentSlotType);
	static EEquipState ConvertToEquipState(EEquipmentSlotType EquipmentSlotType);
	static EItemHandType ConvertToItemHandType(EEquipmentSlotType EquipmentSlotType);
	
	UFUNCTION(BlueprintCallable)
	void ChangeShouldHiddenEquipments(bool bNewShouldHiddenEquipments);
	
	bool ShouldHiddenEquipments() const { return bShouldHiddenEquipments; }
	EEquipState GetCurrentEquipState() const { return CurrentEquipState; }
	
	AA1EquipmentBase* GetEquippedActor(EItemHandType WeaponHandType) const;

	UA1ItemInstance* GetEquippedItemInstance(EItemHandType WeaponHandType) const;
	UA1ItemInstance* GetEquippedItemInstance(EEquipmentSlotType EquipmentSlotType) const;

public:
	FOnEquipStateChanged OnEquipStateChanged;
	
private:
	UPROPERTY(Replicated)
	FA1EquipList EquipList;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquipState)
	EEquipState CurrentEquipState = EEquipState::Count;

	UPROPERTY(Replicated)
	bool bShouldHiddenEquipments = false;
};
