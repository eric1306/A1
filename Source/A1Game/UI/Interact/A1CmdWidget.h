// Copyright (c) 2025 THIS-ACCENT. All Rights Reserved.

#pragma once

#include "UI/A1ActivatableWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "A1CmdWidget.generated.h"

class UTextBlock;
class UEditableText;
class UVerticalBox;
class UProgressBar;
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

	UFUNCTION(blueprintimplementableevent)
	void ShowRefairPercent(float Percent);

private:
	void ConstructUI(FGameplayTag Channel, const FASCInitializeMessage& Message);
	void DestructUI();

public:
	UPROPERTY(EditAnywhere, meta = (Categories = "Message"))
	FGameplayTag MessageChannelTag;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UEditableText> InputText;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SuperviseText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MenuBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> EscapeScreen;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EscapeGuideTxt;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EscapeKeyTxt;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayMessageListenerHandle MessageListenerHandle;

	bool EscapeMode;
};
