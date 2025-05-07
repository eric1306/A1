// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Controller/Raider/BTService_meetsAttackCriteria.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/LyraCharacter.h"
#include "Controller/Raider/A1RaiderController.h"
#include "Item/Managers/A1EquipManagerComponent.h"

UBTService_meetsAttackCriteria::UBTService_meetsAttackCriteria()
{
	NodeName = TEXT("meetsAttackCriteria");
	Interval = 1.0f;
}

void UBTService_meetsAttackCriteria::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AActor* ControllingPawn = Cast<AActor>(OwnerComp.GetAIOwner()->GetPawn());
	if (ControllingPawn == nullptr)
		return;

	if (OwnerComp.GetBlackboardComponent()->GetValueAsObject(AA1RaiderController::AggroTargetKey) == nullptr)
		return;
	// AggroTarget�� ��������
	else
	{
		auto Target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AA1RaiderController::AggroTargetKey);
		ALyraCharacter* TargetActor = Cast<ALyraCharacter>(Target);
		if (TargetActor == nullptr)
			return;

		// ĳ���� ��� �� Aggro ����
		if (TargetActor->GetDeathState() != EA1DeathState::NotDead)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AA1RaiderController::AggroTargetKey, nullptr);
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(AA1RaiderController::CanAttackKey, false);
			return;
		}

		// �� ��ü �� �Ÿ� ���, cm ������� ��
		float dist = FVector::Dist(ControllingPawn->GetActorLocation(), TargetActor->GetActorLocation());

		//Ÿ���� 10m �̻� �־����� aggro ����
		if (dist >= 1000.f)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(AA1RaiderController::CanAttackKey, false);
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AA1RaiderController::AggroTargetKey, nullptr);	
		}

		// ���� ���°� �ƴҶ��� ���
		if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(AA1RaiderController::CanAttackKey) == false)
		{
			// ���� ���� �����ϸ� ���� ���� ���·� ��ȯ
			bool bStrikeFirst = false;
			// �ڵ��� �ִ°�
			{
				FVector MyForward = ControllingPawn->GetActorForwardVector();
				FVector SelfToTarget = (TargetActor->GetActorLocation() - ControllingPawn->GetActorLocation()).GetSafeNormal();

				FVector TargetForward = TargetActor->GetActorForwardVector();
				FVector TargetToSelf = (ControllingPawn->GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal();

				float Dot1 = FVector::DotProduct(MyForward, SelfToTarget);			// Ÿ���� �� ��?
				float Dot2 = FVector::DotProduct(TargetForward, TargetToSelf);		// Ÿ���� ������ �� ����?

				// 135�� �̻� -0.7f
				if (Dot1 > 0.0f && Dot2 < -0.7f)
					bStrikeFirst = true;
			}

			// ���� ���°�?
			{
				UA1EquipManagerComponent* EquipManager = TargetActor->FindComponentByClass<UA1EquipManagerComponent>();
				if (EquipManager == nullptr)
					return;

				if (EquipManager->GetCurrentEquipState() == EEquipState::Unarmed)
					bStrikeFirst = true;
			}

			OwnerComp.GetBlackboardComponent()->SetValueAsBool(AA1RaiderController::CanAttackKey, bStrikeFirst);
		}
	}
}
