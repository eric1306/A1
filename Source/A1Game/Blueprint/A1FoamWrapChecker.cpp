// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.


#include "Blueprint/A1FoamWrapChecker.h"

#include "A1LogChannels.h"
#include "Engine/OverlapResult.h"

bool UA1FoamWrapChecker::IsActorWrappedByFoam(AActor* TargetActor, float WrapThreshold)
{
    if (!IsValid(TargetActor))
        return false;

    float Coverage = GetActorFoamCoverage(TargetActor);
    UE_LOG(LogA1, Log, TEXT("Coverage: %f"), Coverage);
    return Coverage >= WrapThreshold;
}

float UA1FoamWrapChecker::GetActorFoamCoverage(AActor* TargetActor)
{
    if (!IsValid(TargetActor))
        return 0.0f;

    FVector Center = TargetActor->GetActorLocation();
    FVector Extent = FVector(25.f, 25.f, 25.f); // ������ ũ��� ����

    TArray<FOverlapResult> OverlapResults;
    FCollisionShape CollisionShape = FCollisionShape::MakeBox(Extent);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(TargetActor); // �ڱ� �ڽ��� ����

    // Overlap �˻�� ���� �� ���͵� ã��
    bool bFoundOverlaps = TargetActor->GetWorld()->OverlapMultiByChannel(
        OverlapResults, 
        Center,
        FQuat::Identity,
        ECC_WorldDynamic,
        CollisionShape,
        QueryParams
    );

    DrawDebugBox(TargetActor->GetWorld(), Center, Extent, FColor::Red, true);

    // Foam ���͵鸸 ���͸�
    TArray<AActor*> FoamActors;
    if (bFoundOverlaps)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* OverlapActor = Result.GetActor();
            UE_LOG(LogA1, Log, TEXT("%s"), *OverlapActor->GetName())
            if (IsValid(OverlapActor) &&
                OverlapActor->GetClass()->GetName().Contains(TEXT("Foam")))
            {
                FoamActors.AddUnique(OverlapActor);
            }
        }
    }

    if (FoamActors.Num() == 0)
        return 0.0f;

    // üũ ����Ʈ ���� (������ ����)
    TArray<FVector> CheckPoints;

    // 8�� �𼭸�
    CheckPoints.Add(Center + FVector(Extent.X, Extent.Y, Extent.Z));
    CheckPoints.Add(Center + FVector(Extent.X, Extent.Y, -Extent.Z));
    CheckPoints.Add(Center + FVector(Extent.X, -Extent.Y, Extent.Z));
    CheckPoints.Add(Center + FVector(Extent.X, -Extent.Y, -Extent.Z));
    CheckPoints.Add(Center + FVector(-Extent.X, Extent.Y, Extent.Z));
    CheckPoints.Add(Center + FVector(-Extent.X, Extent.Y, -Extent.Z));
    CheckPoints.Add(Center + FVector(-Extent.X, -Extent.Y, Extent.Z));
    CheckPoints.Add(Center + FVector(-Extent.X, -Extent.Y, -Extent.Z));

    // 6�� �� �߽�
    CheckPoints.Add(Center + FVector(Extent.X, 0, 0));
    CheckPoints.Add(Center + FVector(-Extent.X, 0, 0));
    CheckPoints.Add(Center + FVector(0, Extent.Y, 0));
    CheckPoints.Add(Center + FVector(0, -Extent.Y, 0));
    CheckPoints.Add(Center + FVector(0, 0, Extent.Z));
    CheckPoints.Add(Center + FVector(0, 0, -Extent.Z));

    // �߽���
    CheckPoints.Add(Center);

    // �� üũ ����Ʈ�� ���� �����ִ��� Ȯ��
    int32 CoveredPoints = 0;

    for (const FVector& Point : CheckPoints)
    {
        bool bPointCovered = false;

        // ���͸��� �� ���͵���� üũ (�ξ� ���� ��)
        for (AActor* FoamActor : FoamActors)
        {
            if (!IsValid(FoamActor))
                continue;

            // �� ������ ��� Primitive ������Ʈ üũ
            TArray<UPrimitiveComponent*> PrimitiveComponents;
            FoamActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

            for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
            {
                if (!IsValid(PrimComp) || !PrimComp->GetCollisionEnabled())
                    continue;

                // ����Ʈ�� ������Ʈ �ٿ�� �ȿ� �ִ��� üũ
                FBox ComponentBounds = PrimComp->Bounds.GetBox();
                if (ComponentBounds.IsInsideOrOn(Point))
                {
                    bPointCovered = true;
                    break;
                }
            }

            if (bPointCovered)
                break;
        }

        if (bPointCovered)
            CoveredPoints++;
    }

    // Ŀ������ ���� ��ȯ
    return CheckPoints.Num() > 0 ? (float)CoveredPoints / (float)CheckPoints.Num() : 0.0f;
}
