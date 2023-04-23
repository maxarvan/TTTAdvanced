// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TTTGameWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TTTADVANCED_API UTTTGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// virtual void NativeOnInitialized();
	// virtual void NativePreConstruct();
	// virtual void NativeConstruct();
	// virtual void NativeDestruct();
	// virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	
	UFUNCTION(BlueprintCallable)
	FString GetPlayerName() const;

protected:
	UFUNCTION()
	void HandleTurnChange(bool IsMyTurn);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, BindWidget))
	class UTextBlock* MyTurnText;
};
