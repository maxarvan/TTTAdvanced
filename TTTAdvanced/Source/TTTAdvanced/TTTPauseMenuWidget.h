// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTTPauseMenuWidget.generated.h"

class UButton;

UCLASS()
class TTTADVANCED_API UTTTPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnResumeButtonPressed();
	UFUNCTION()
	void OnQuitButtonPressed();
	
private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidgetOptional))
	UButton* ResumeButton;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidgetOptional))
	UButton* QuitButton;
};
