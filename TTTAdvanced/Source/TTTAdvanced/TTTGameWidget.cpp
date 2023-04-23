// Fill out your copyright notice in the Description page of Project Settings.

#include "TTTGameWidget.h"
#include "ATTTController.h"
#include "Components/TextBlock.h"

void UTTTGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(ATTTController* Controller = Cast<ATTTController>(GetOwningPlayer()))
	{
		Controller->OnTurnChanged.AddDynamic(this, &UTTTGameWidget::HandleTurnChange);

		HandleTurnChange(Controller->GetIsMyTurn());
	}
}

void UTTTGameWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if(ATTTController* Controller = Cast<ATTTController>(GetOwningPlayer()))
	{
		Controller->OnTurnChanged.RemoveDynamic(this, &UTTTGameWidget::HandleTurnChange);
	}
}

FString UTTTGameWidget::GetPlayerName() const
{
	if(const APlayerController* Controller = GetOwningPlayer())
	{
		if(Controller->GetNetMode() == NM_Client)
		{
			return "Client";
		}
		else
		{
			return "Server";
		}
	}
	return "FUCK YOU";
}

void UTTTGameWidget::HandleTurnChange(bool IsMyTurn)
{
	if (MyTurnText)
	{
		const FString Text = IsMyTurn ? "My Turn" : "Not your turn";
		MyTurnText->SetText(FText::FromString(Text));
	}
}