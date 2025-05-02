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
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void AA1CreatureBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);

	if (DeadMontage != nullptr)
		AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

