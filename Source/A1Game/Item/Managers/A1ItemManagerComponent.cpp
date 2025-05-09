﻿#include "A1ItemManagerComponent.h"
		  
#include "A1EquipManagerComponent.h"
#include "A1EquipmentManagerComponent.h"
#include "A1InventoryManagerComponent.h"
#include "Actors/A1EquipmentBase.h"
#include "Components/CapsuleComponent.h"
#include "Data/A1ItemData.h"
#include "Item/A1ItemInstance.h"
#include "Item/Fragments/A1ItemFragment_Equipable.h"
#include "Item/Fragments/A1ItemFragment_Equipable_Attachment.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1ItemManagerComponent)

UA1ItemManagerComponent::UA1ItemManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UA1ItemManagerComponent::Server_InventoryToEquipment_Implementation(UA1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UA1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToEquipmentManager) == false)
		return;

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		UA1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
	}
	else
	{
		FIntPoint ToItemSlotPos;
		if (ToEquipmentManager->CanSwapEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
		{
			const int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
			const int32 ToItemCount = ToEquipmentManager->GetItemCount(ToEquipmentSlotType);
			
			UA1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
			UA1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstanceTo, ToItemCount);
			ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
	}
}

void UA1ItemManagerComponent::Server_EquipmentToInventory_Implementation(UA1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UA1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	int32 MovableCount = ToInventoryManager->CanMoveOrMergeItem(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPos);
	if (MovableCount > 0)
	{
		UA1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
		ToInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableCount);
		FromEquipmentManager->GetEquipManager()->CanInteract();
	}
}

void UA1ItemManagerComponent::Server_InventoryToInventory_Implementation(UA1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UA1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	if (FromInventoryManager == ToInventoryManager && FromItemSlotPos == ToItemSlotPos)
		return;

	int32 MovableCount = ToInventoryManager->CanMoveOrMergeItem(FromInventoryManager, FromItemSlotPos, ToItemSlotPos);
	if (MovableCount > 0)
	{
		UA1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
		ToInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableCount);
	}
}

void UA1ItemManagerComponent::Server_EquipmentToEquipment_Implementation(UA1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UA1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false ||  IsAllowedComponent(ToEquipmentManager) == false)
		return;

	if (FromEquipmentManager == ToEquipmentManager && FromEquipmentSlotType == ToEquipmentSlotType)
		return;

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		UA1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
	}
	else if (ToEquipmentManager->CanSwapEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		const int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
		const int32 ToItemCount = ToEquipmentManager->GetItemCount(ToEquipmentSlotType);
		
		UA1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
		UA1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
		FromEquipmentManager->AddEquipment_Unsafe(FromEquipmentSlotType, RemovedItemInstanceTo, ToItemCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
	}
}

void UA1ItemManagerComponent::Server_QuickFromInventory_Implementation(UA1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false)
		return;
	
	UA1InventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	UA1EquipmentManagerComponent* MyEquipmentManager = GetMyEquipmentManager();
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(MyInventoryManager) == false || IsAllowedComponent(MyEquipmentManager) == false)
		return;

	UA1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;

	if (FromItemInstance->FindFragmentByClass<UA1ItemFragment_Equipable>())
	{
		// 1. [장비]
		// 1-1. [내 인벤토리] -> 내 장비 교체 -> 내 장비 장착 
		// 1-2. [다른 인벤토리] -> 내 장비 교체 -> 내 장비 장착 -> 내 인벤토리

		EEquipmentSlotType ToEquipmentSlotType;
		FIntPoint ToItemSlotPos;
		if (MyEquipmentManager->CanSwapEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
		{
			const int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
			const int32 ToItemCount = MyEquipmentManager->GetItemCount(ToEquipmentSlotType);
				
			UA1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
			UA1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstanceTo, ToItemCount);
			MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
		else
		{
			int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
			if (MovableCount > 0)
			{
				UA1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
				MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
			}
			else
			{
				if (MyInventoryManager != FromInventoryManager)
				{
					TArray<FIntPoint> OutToItemSlotPoses;
					TArray<int32> OutToItemCounts;
					MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, OutToItemSlotPoses, OutToItemCounts);
					if (MovableCount > 0)
					{
						UA1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
						for (int32 i = 0; i < OutToItemSlotPoses.Num(); i++)
						{
							MyInventoryManager->AddItem_Unsafe(OutToItemSlotPoses[i], RemovedItemInstance, OutToItemCounts[i]);
						}
						return;
					}
				}
			}
		}
	}
	else
	{
		// 2. [일반 아이템]
		// 2-1. [내 인벤토리] -> X
		// 2-2. [다른 인벤토리] -> 내 인벤토리

		if (MyInventoryManager != FromInventoryManager)
		{
			TArray<FIntPoint> ToItemSlotPoses;
			TArray<int32> ToItemCounts;
			
			int32 MovableItemCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, ToItemSlotPoses, ToItemCounts);
			if (MovableItemCount > 0)
			{
				UA1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
				for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
				{
					MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
				}
			}
		}
	}
}

void UA1ItemManagerComponent::Server_QuickFromEquipment_Implementation(UA1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false)
		return;

	// 1. [내 장비창] -> 내 인벤토리
	// 2. [다른 장비창] -> 내 장비 교체 -> 내 장비 장착 -> 내 인벤토리 

	UA1InventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	UA1EquipmentManagerComponent* MyEquipmentManager = GetMyEquipmentManager();
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	if (IsAllowedComponent(MyInventoryManager) == false || IsAllowedComponent(MyEquipmentManager) == false)
		return;

	if (MyEquipmentManager == FromEquipmentManager)
	{
		TArray<FIntPoint> ToItemSlotPoses;
		TArray<int32> ToItemCounts;
		
		int32 MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPoses, ToItemCounts);
		if (MovableCount > 0)
		{
			UA1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
			for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
			{
				MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
			}
		}
	}
	else
	{
		EEquipmentSlotType ToEquipmentSlotType;
		if (MyEquipmentManager->CanSwapEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
		{
			const int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
			const int32 ToItemCount = MyEquipmentManager->GetItemCount(ToEquipmentSlotType);
					
			UA1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
			UA1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromEquipmentManager->AddEquipment_Unsafe(FromEquipmentSlotType, RemovedItemInstanceTo, ToItemCount);
			MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
		else
		{
			int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
			if (MovableCount > 0)
			{
				UA1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
				MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
			}
			else
			{
				TArray<FIntPoint> ToItemSlotPoses;
				TArray<int32> ToItemCounts;

				MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPoses, ToItemCounts);
				if (MovableCount > 0)
				{
					UA1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
					for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
					{
						MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
					}
				}
			}
		}
	}
}

void UA1ItemManagerComponent::Server_DropItemFromInventory_Implementation(UA1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	if (HasAuthority() == false)
		return;

	if (FromInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false)
		return;

	UA1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;

	int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
	if (FromItemCount <= 0)
		return;
	
	//if (TryDropItem(FromItemInstance, FromItemCount))
	{
		FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
	}
}

void UA1ItemManagerComponent::Server_DropItemFromEquipment_Implementation(UA1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	if (HasAuthority() == false)
		return;
	
	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	if (IsAllowedComponent(FromEquipmentManager) == false)
		return;

	UA1ItemInstance* FromItemInstance = FromEquipmentManager->GetItemInstance(FromEquipmentSlotType);
	if (FromItemInstance == nullptr)
		return;

	int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
	if (FromItemCount <= 0)
		return;
	
	//if (TryDropItem(FromItemInstance, FromItemCount))
	{
		FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
	}
}

bool UA1ItemManagerComponent::TryPickItem(AA1EquipmentBase* PickupableItemActor)
{
	if (HasAuthority() == false)
		return false;

	if (IsValid(PickupableItemActor) == false)
		return false;

	UA1InventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	UA1EquipmentManagerComponent* MyEquipmentManager = GetMyEquipmentManager();
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return false;

	if (IsAllowedComponent(MyInventoryManager) == false || IsAllowedComponent(MyEquipmentManager) == false)
		return false;

	const UA1ItemTemplate& ItemTemplate = UA1ItemData::Get().FindItemTemplateByID(PickupableItemActor->GetTemplateID());

	const UA1ItemFragment_Equipable_Attachment* EquippableFragment = ItemTemplate.FindFragmentByClass<UA1ItemFragment_Equipable_Attachment>();
	if (EquippableFragment == nullptr)
		return false;
	
	int32 ItemTemplateID = PickupableItemActor->GetTemplateID();
	EItemRarity ItemRarity = PickupableItemActor->GetItemRarity();
	int32 ItemCount = 1;
	
	EEquipmentSlotType ToEquipmentSlotType = EquippableFragment->ItemHandType;

	int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(ItemTemplateID, ItemRarity, ItemCount, ToEquipmentSlotType);
	if (MovableCount == ItemCount)
	{
		UA1ItemInstance* ItemInstance = NewObject<UA1ItemInstance>();
		ItemInstance->Init(ItemTemplateID, ItemRarity);

		MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, ItemInstance, MovableCount);

		PickupableItemActor->Destroy();

		MyEquipmentManager->GetEquipManager()->CanInteract();
		return true;
	}
	//else
	//{
	//	TArray<FIntPoint> ToItemSlotPoses;
	//	TArray<int32> ToItemCounts;
	//
	//	MovableCount = MyInventoryManager->CanAddItem(ItemTemplateID, ItemRarity, ItemCount, ToItemSlotPoses, ToItemCounts);
	//	if (MovableCount == ItemCount)
	//	{
	//		if (ItemInstance == nullptr)
	//		{
	//			ItemInstance = NewObject<UA1ItemInstance>();
	//			ItemInstance->Init(ItemTemplateID, ItemRarity);
	//		}
	//
	//		for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
	//		{
	//			MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], ItemInstance, ToItemCounts[i]);
	//		}
	//
	//		PickupableItemActor->Destroy();
	//		return true;
	//	}
	//}

	

	return false;
}

/*
bool UA1ItemManagerComponent::TryDropItem(UA1ItemInstance* FromItemInstance, int32 FromItemCount)
{
	if (HasAuthority() == false)
		return false;

	if (FromItemInstance == nullptr || FromItemCount <= 0)
		return false;

	AController* Controller = Cast<AController>(GetOwner());
	ACharacter* Character = Controller ? Cast<ACharacter>(Controller->GetPawn()) : Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
		return false;
	
	const float MaxDistance = 100.f;
	const int32 MaxTryCount = 5.f;
	float HalfRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius() / 2.f;
	float QuarterHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2.f;
	TArray<AActor*> ActorsToIgnore = { Character };

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	for (int32 i = 0; i < MaxTryCount; i++)
	{
		FHitResult HitResult;
		FVector2D RandPoint = FMath::RandPointInCircle(MaxDistance);
		FVector TraceStartLocation = Character->GetCapsuleComponent()->GetComponentLocation();
		FVector TraceEndLocation = TraceStartLocation + FVector(RandPoint.X, RandPoint.Y, 0.f);
		
		if (UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), TraceStartLocation, TraceEndLocation, HalfRadius, QuarterHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
			continue;
		
		TSubclassOf<AA1PickupableItemBase> PickupableItemBaseClass = ULyraAssetManager::Get().GetSubclassByName<AA1PickupableItemBase>("PickupableItemBaseClass");
		AA1PickupableItemBase* PickupableItemActor = GetWorld()->SpawnActor<AA1PickupableItemBase>(PickupableItemBaseClass, TraceEndLocation, FRotator::ZeroRotator, SpawnParameters);
		if (PickupableItemActor == nullptr)
			continue;
		
		FA1PickupInfo PickupInfo;
		PickupInfo.PickupInstance.ItemInstance = FromItemInstance;
		PickupInfo.PickupInstance.ItemCount = FromItemCount;
		PickupableItemActor->SetPickupInfo(PickupInfo);
		return true;
	}
	
	return false;
}
*/

bool UA1ItemManagerComponent::TryDropItem(UA1ItemInstance* FromItemInstance, int32 FromItemCount)
{
	if (HasAuthority() == false)
		return false;

	if (FromItemInstance == nullptr || FromItemCount <= 0)
		return false;

	AController* Controller = Cast<AController>(GetOwner());
	ACharacter* Character = Controller ? Cast<ACharacter>(Controller->GetPawn()) : Cast<ACharacter>(GetOwner());
	if (Character == nullptr)
		return false;


	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	FVector SpawnLocation = Character->GetActorLocation();
	SpawnLocation.Z = 0.0f;
	
	//TSubclassOf<AA1EquipmentBase> PickupableItemBaseClass = ULyraAssetManager::Get().GetSubclassByName<AA1EquipmentBase>("PickupableItemBaseClass");
	AA1EquipmentBase* PickupableItemActor = GetWorld()->SpawnActor<AA1EquipmentBase>(AA1EquipmentBase::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	if (PickupableItemActor == nullptr)
		return false;
	
	const UA1ItemFragment_Equipable_Attachment* EquippableFragment = FromItemInstance->FindFragmentByClass<UA1ItemFragment_Equipable_Attachment>();
	if (EquippableFragment == nullptr)
		return false;

	PickupableItemActor->Init(FromItemInstance->GetItemTemplateID(), EquippableFragment->ItemHandType, FromItemInstance->GetItemRarity());
	
	return true;
}

void UA1ItemManagerComponent::AddAllowedComponent(UActorComponent* ActorComponent)
{
	AllowedComponents.Add(ActorComponent);
}

void UA1ItemManagerComponent::RemoveAllowedComponent(UActorComponent* ActorComponent)
{
	AllowedComponents.Remove(ActorComponent);
}

bool UA1ItemManagerComponent::IsAllowedComponent(UActorComponent* ActorComponent) const
{
	return AllowedComponents.Contains(ActorComponent);
}

UA1InventoryManagerComponent* UA1ItemManagerComponent::GetMyInventoryManager() const
{
	UA1InventoryManagerComponent* MyInventoryManager = nullptr;
	
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			MyInventoryManager = Pawn->GetComponentByClass<UA1InventoryManagerComponent>();
		}
	}

	return MyInventoryManager;
}

UA1EquipmentManagerComponent* UA1ItemManagerComponent::GetMyEquipmentManager() const
{
	UA1EquipmentManagerComponent* MyEquipmentManager = nullptr;
	
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			MyEquipmentManager = Pawn->GetComponentByClass<UA1EquipmentManagerComponent>();
		}
	}

	return MyEquipmentManager;
}
