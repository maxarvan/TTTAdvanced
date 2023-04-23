// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTPauseMenuWidget.h"
#include "TTTGame.h"
#include "TTTHelper.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UTTTPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(ResumeButton)
	{
		ResumeButton->OnPressed.AddDynamic(this, &UTTTPauseMenuWidget::OnResumeButtonPressed);
	}

	if(QuitButton)
	{
		QuitButton->OnPressed.AddDynamic(this, &UTTTPauseMenuWidget::OnQuitButtonPressed);
	}
}

void UTTTPauseMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if(ResumeButton)
	{
		ResumeButton->OnPressed.RemoveDynamic(this, &UTTTPauseMenuWidget::OnResumeButtonPressed);
	}

	if(QuitButton)
	{
		QuitButton->OnPressed.RemoveDynamic(this, &UTTTPauseMenuWidget::OnQuitButtonPressed);
	}
}

void UTTTPauseMenuWidget::OnResumeButtonPressed()
{
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->SetGameStateType(ETTTGameStateType::Game);
	}
}

void UTTTPauseMenuWidget::OnQuitButtonPressed()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}