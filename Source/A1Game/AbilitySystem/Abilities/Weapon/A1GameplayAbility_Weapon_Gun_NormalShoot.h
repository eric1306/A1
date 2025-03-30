#pragma once

//#include "A1GameplayAbility_Weapon_Gun_Projectile.h"
#include "AbilitySystem/Abilities/Docker/A1GameplayAbility_Equipment.h"
#include "A1GameplayAbility_Weapon_Gun_NormalShoot.generated.h"

UCLASS()
class UA1GameplayAbility_Weapon_Gun_NormalShoot : public UA1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UA1GameplayAbility_Weapon_Gun_NormalShoot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION()
	void OnMontageFinished();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="A1|Bow NormalShoot")
	TObjectPtr<UAnimMontage> ReleaseMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="A1|Bow NormalShoot")
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="A1|Bow NormalShoot")
	TObjectPtr<UAnimMontage> ReleaseReloadMontage;
};
