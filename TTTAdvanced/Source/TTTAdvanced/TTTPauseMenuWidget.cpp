// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTPauseMenuWidget.h"

#include "ATTTController.h"
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

	if(RestartButton)
	{
		RestartButton->OnPressed.AddDynamic(this, &UTTTPauseMenuWidget::OnRestartButtonPressed);
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

	if(RestartButton)
	{
		RestartButton->OnPressed.RemoveDynamic(this, &UTTTPauseMenuWidget::OnRestartButtonPressed);
	}
}

void UTTTPauseMenuWidget::OnResumeButtonPressed()
{
	if(ATTTController* TTTController = Cast<ATTTController>(GetOwningPlayer()))
	{
		TTTController->TryResumeGame();
	}
}

void UTTTPauseMenuWidget::OnQuitButtonPressed()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UTTTPauseMenuWidget::OnRestartButtonPressed()
{
	if(ATTTController* TTTController = Cast<ATTTController>(GetOwningPlayer()))
	{
		TTTController->Server_RequestGameRestart();
	}
}