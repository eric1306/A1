#include "Interaction/Abilities/Tasks/A1AbilityTask_GrantNearbyInteraction.h"

#include "Interaction/A1Interactable.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "Interaction/A1Interactable.h"
#include "Interaction/A1InteractionQuery.h"
#include "Physics/LyraCollisionChannels.h"

UA1AbilityTask_GrantNearbyInteraction* UA1AbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractables(
	UGameplayAbility* OwningAbility, float InteractionAbilityScanRange, float InteractionAbilityScanRate)
{
	UA1AbilityTask_GrantNearbyInteraction* Task = NewAbilityTask<UA1AbilityTask_GrantNearbyInteraction>(OwningAbility);

	Task->InteractionAbilityScanRange = InteractionAbilityScanRange;
	Task->InteractionAbilityScanRate = InteractionAbilityScanRate;

	return Task;
}

void UA1AbilityTask_GrantNearbyInteraction::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionAbilityScanRate, true);
	}
}

void UA1AbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UA1AbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	AActor* AvatarActor = GetAvatarActor();

	if (World && AvatarActor)
	{
		TSet<FObjectKey> RemoveKeys;
		GrantedInteractionAbilities.GetKeys(RemoveKeys);

		FCollisionQueryParams Params(SCENE_QUERY_STAT(UD1AbilityTask_GrantNearbyInteraction), false);

		TArray<FOverlapResult> OverlapResults;
		//World->OverlapMultiByChannel(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, A1_TraceChannel_Interaction, FCollisionShape::MakeSphere(InteractionAbilityScanRange), Params);

		if (OverlapResults.Num() > 0)
		{
			TArray<TScriptInterface<IA1Interactable>> Interactables;
			for (const FOverlapResult& OverlapResult : OverlapResults)
			{
				/*TScriptInterface<IA1Interactable> InteractableActor(OverlapResult.GetActor());
				if (InteractableActor)
				{
					Interactables.AddUnique(InteractableActor);
				}

				TScriptInterface<IA1Interactable> InteractableComponent(OverlapResult.GetComponent());
				if (InteractableComponent)
				{
					Interactables.AddUnique(InteractableComponent);
				}*/
			}

			FA1InteractionQuery InteractionQuery;
			InteractionQuery.RequestingAvatar = AvatarActor;
			InteractionQuery.RequestingController = Cast<AController>(AvatarActor->GetOwner());

			TArray<FA1InteractionInfo> InteractionInfos;
			for (TScriptInterface<IA1Interactable>& Interactable : Interactables)
			{
				FA1InteractionInfoBuilder InteractionInfoBuilder(Interactable, InteractionInfos);
				Interactable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);
			}

			for (FA1InteractionInfo& InteractionInfo : InteractionInfos)
			{
				if (InteractionInfo.AbilityToGrant)
				{
					FObjectKey ObjectKey(InteractionInfo.AbilityToGrant);
					if (GrantedInteractionAbilities.Find(ObjectKey))
					{
						RemoveKeys.Remove(ObjectKey);
					}
					else
					{
						FGameplayAbilitySpec Spec(InteractionInfo.AbilityToGrant, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(Spec);
						GrantedInteractionAbilities.Add(ObjectKey, SpecHandle);
					}
				}
			}
		}

		for (const FObjectKey& RemoveKey : RemoveKeys)
		{
			AbilitySystemComponent->ClearAbility(GrantedInteractionAbilities[RemoveKey]);
			GrantedInteractionAbilities.Remove(RemoveKey);
		}
	}
}
