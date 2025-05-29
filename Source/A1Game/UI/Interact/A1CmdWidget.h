// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "UI/A1ActivatableWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "A1CmdWidget.generated.h"

class UEditableText;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FASCInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> ASC;
};

UCLASS()
class A1GAME_API UA1CmdWidget : public UA1ActivatableWidget
{
	GENERATED_BODY()

public:
	UA1CmdWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void InputEnded(FText InText);

private:
	void ConstructUI(FGameplayTag Channel, const FASCInitializeMessage& Message);

public:
	UPROPERTY(EditAnywhere, meta = (Categories = "Message"))
	FGameplayTag MessageChannelTag;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> InputText;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayMessageListenerHandle MessageListenerHandle;
};
