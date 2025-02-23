// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Character/A1CreatureBase.h"
#include "AbilitySystemComponent.h"
#include "Controller/Player/A1PlayerState.h"

// Sets default values
AA1CreatureBase::AA1CreatureBase()
{
	ASC = nullptr;
}

UAbilitySystemComponent* AA1CreatureBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void AA1CreatureBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AA1PlayerState* GASPS = GetPlayerState<AA1PlayerState>();
	if (GASPS)
	{
		//Setting Ability System Component
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);

		//Print GAS Debug in Editor viewport
		APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
		PlayerController->ConsoleCommand("showdebug abilitysystem");
	}
}

