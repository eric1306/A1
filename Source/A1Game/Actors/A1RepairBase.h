// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Interaction/A1WorldInteractable.h"
#include "A1RepairBase.generated.h"

class UArrowComponent;

UENUM()
enum class RepairState
{
	NotBroken,
	Break,
	Complete
};

UCLASS()
class AA1RepairBase : public AA1WorldInteractable
{
	GENERATED_BODY()

public:
	AA1RepairBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual FA1InteractionInfo GetPreInteractionInfo(const FA1InteractionQuery& InteractionQuery) const override;
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FA1InteractionInfo InteractionInfo;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

public:
	RepairState CurrentState = RepairState::Break;
};
