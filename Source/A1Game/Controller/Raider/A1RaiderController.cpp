// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "A1RaiderController.h"
#include "A1LogChannels.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AA1RaiderController::HomePosKey(TEXT("HomePos"));
const FName AA1RaiderController::PatrolPosKey(TEXT("PatrolPos"));
const FName AA1RaiderController::AggroTargetKey(TEXT("AggroTarget"));


AA1RaiderController::AA1RaiderController()
{
	// TODO
	// RaiderData를 만들어서 자동 연결되도록 변경
	
	//static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT(""));
	//if (BBObject.Succeeded())
	//{
	//	BBAsset = BBObject.Object;
	//}
	//
	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT(""));
	//if (BTObject.Succeeded())
	//{
	//	BTAsset = BTObject.Object;
	//}
}

void AA1RaiderController::BeginPlay()
{
	Super::BeginPlay();
}

void AA1RaiderController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBlackboardComponent* BlackboardComp = Blackboard;
	if (UseBlackboard(BBAsset, BlackboardComp))
	{
		BlackboardComp->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogA1Raider, Warning, TEXT("AIController couldn't run behavior tree!"));
		}
	}
}

void AA1RaiderController::OnUnPossess()
{
	Super::OnUnPossess();
}
