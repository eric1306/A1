#include "A1GameplayAbility_Utility_FlashLight.h"

#include "Actors/A1UtilityBase.h"
#include "Item/Managers/A1EquipmentManagerComponent.h"
#include "Item/Managers/A1InventoryManagerComponent.h"
#include "Item/Managers/A1ItemManagerComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(A1GameplayAbility_Utility_FlashLight)

UA1GameplayAbility_Utility_FlashLight::UA1GameplayAbility_Utility_FlashLight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UA1GameplayAbility_Utility_FlashLight::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AA1UtilityBase* flashlight = Cast<AA1UtilityBase>(GetFirstEquipmentActor());
	flashlight->ChangeState();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UA1GameplayAbility_Utility_FlashLight::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
