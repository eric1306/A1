// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "A1FoamWrapChecker.generated.h"

/**
 * 
 */
UCLASS()
class A1GAME_API UA1FoamWrapChecker : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    // ���� �Լ� - ���Ͱ� ���� �������ִ��� Ȯ��
    UFUNCTION(BlueprintCallable, Category = "Foam System")
    static bool IsActorWrappedByFoam(AActor* TargetActor, float WrapThreshold = 0.6f);

    // Ŀ������ �ۼ�Ʈ Ȯ��
    UFUNCTION(BlueprintCallable, Category = "Foam System")
    static float GetActorFoamCoverage(AActor* TargetActor);
};
