// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "A1SpaceshipBase.h"
#include "Interaction/A1WorldInteractable.h"
#include "A1StorageBase.generated.h"

class AA1EquipmentBase;
class UBoxComponent;
class UA1StorageComponent;
class UArrowComponent;
/**
 * Storage Interactable Object
 * - Can Store Item Instance
 */
UCLASS()
class AA1StorageBase : public AA1WorldInteractable, public IA1SpaceshipComponent
{
	GENERATED_BODY()

public:
	AA1StorageBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	//IA1SpaceshipComponent
	virtual void RegisterWithSpaceship(AA1SpaceshipBase* Spaceship) override;
	virtual ESpaceshipComponentType GetComponentType() const override { return ESpaceshipComponentType::Storage; }

protected:
	void SetupTags();

	UFUNCTION(BlueprintCallable)
	AA1SpaceshipBase* FindSpaceshipOwner() const;

	FORCEINLINE	AA1SpaceshipBase* GetOwningSpaceship() const { return OwningSpaceship.IsValid() ? OwningSpaceship.Get() : nullptr; }

	void TryDetectItem();

	void TryStoreItem(AA1EquipmentBase* Equip);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> StoreLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<USceneComponent>> StoreLocations;

	UPROPERTY(VisibleAnywhere, Transient)
	TWeakObjectPtr<AA1SpaceshipBase> OwningSpaceship;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AActor>> SavedEquips;

	FTimerHandle ItemDetectTimerHandle;

	float ItemDetectionRadius;

	int32 MaxStorageSize;

	int32 LatestIdx;
};
