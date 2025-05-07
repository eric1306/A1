// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Character/A1CreatureBase.h"
#include "AbilitySystemComponent.h"
#include "Controller/Player/A1PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AA1CreatureBase::AA1CreatureBase(const FObjectInitializer& ObjectInitializer)
{
	ASC = nullptr;
}

UAbilitySystemComponent* AA1CreatureBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void AA1CreatureBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetMesh()->SetAnimInstanceClass(nullptr);
	if (DeadMontage)
	{
		GetMesh()->PlayAnimation(DeadMontage, false);
	}
	SetActorEnableCollision(false);
}

