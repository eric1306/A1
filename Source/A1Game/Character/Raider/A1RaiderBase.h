// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/A1CreatureBase.h"
#include "A1RaiderBase.generated.h"

class UA1CharacterAttributeSet;
class AA1EquipmentBase;

UCLASS()
class A1GAME_API AA1RaiderBase : public AA1CreatureBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AA1RaiderBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	void BeAttacked(AActor* Instigator, float OldValue, float NewValue);
	void HandleOutOfHealth(float OldValue, float NewValue);
	
	UFUNCTION()
	void DestroyDueToDeath();

	//UFUNCTION(Server, Reliable)
	//void Server_PerformTrace(USkeletalMeshComponent* MeshComponent);

protected:
	UPROPERTY()
	TObjectPtr<UA1CharacterAttributeSet> HealthSet;

private:
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AA1EquipmentBase>> dropItems;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;
};
