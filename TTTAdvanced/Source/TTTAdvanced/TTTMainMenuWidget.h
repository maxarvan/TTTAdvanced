// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTTMainMenuWidget.generated.h"

class UButton;

UCLASS()
class TTTADVANCED_API UTTTMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnStartServerButtonPressed();
	UFUNCTION()
	void OnJoinServerButtonPressed();
	UFUNCTION()
	void OnQuitButtonPressed();
	
private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidgetOptional))
	UButton* StartServerButton;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidgetOptional))
	UButton* JoinServerButton;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidgetOptional))
	UButton* QuitButton;
};
