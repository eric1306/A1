// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "Interaction/A1WorldInteractable.h"
#include "A1StorageBase.generated.h"

class UBoxComponent;
class UA1StorageComponent;
class UArrowComponent;
/**
 * 
 */
UCLASS()
class AA1StorageBase : public AA1WorldInteractable
{
	GENERATED_BODY()

public:
    AA1StorageBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


};
