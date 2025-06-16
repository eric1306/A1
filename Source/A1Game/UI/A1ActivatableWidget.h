// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "A1ActivatableWidget.generated.h"


UCLASS()
class A1GAME_API UA1ActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UA1ActivatableWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable)
	void Deactivate();

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
};
