// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "A1RaiderController.h"
#include "A1LogChannels.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/A1RaiderData.h"
#include "System/LyraAssetManager.h"

const FName AA1RaiderController::HomePosKey(TEXT("HomePos"));
const FName AA1RaiderController::PatrolPosKey(TEXT("PatrolPos"));
const FName AA1RaiderController::AggroTargetKey(TEXT("AggroTarget"));
const FName AA1RaiderController::CanAttackKey(TEXT("CanAttack"));


AA1RaiderController::AA1RaiderController()
{
}

void AA1RaiderController::BeginPlay()
{
	Super::BeginPlay();

}

void AA1RaiderController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// TODO
	// RaiderData를 만들어서 자동 연결되도록 변경
	const UA1RaiderData& RaiderData = ULyraAssetManager::Get().GetRaiderData();
	const FA1RaiderBaseSet& RaiderBase = RaiderData.GetRaiderDataSet(ERaiderType::Despoiler);

	BBAsset = RaiderBase.BBAsset;
	BTAsset = RaiderBase.BTAsset;

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
