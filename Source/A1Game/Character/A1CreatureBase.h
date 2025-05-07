// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "A1CreatureBase.generated.h"

class UAbilitySystemComponent;

UCLASS()
class A1GAME_API AA1CreatureBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AA1CreatureBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

// Dead Section
protected:
	virtual void SetDead();

	float DeadEventDelayTime = 5.0f;

protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;
};
