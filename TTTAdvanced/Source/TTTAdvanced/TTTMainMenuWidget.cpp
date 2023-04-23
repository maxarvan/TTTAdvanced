// Fill out your copyright notice in the Description page of Project Settings.

#include "TTTMainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UTTTMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(StartServerButton)
	{
		StartServerButton->OnPressed.AddDynamic(this, &UTTTMainMenuWidget::OnStartServerButtonPressed);
	}

	if(JoinServerButton)
	{
		JoinServerButton->OnPressed.AddDynamic(this, &UTTTMainMenuWidget::OnJoinServerButtonPressed);
	}

	if(QuitButton)
	{
		QuitButton->OnPressed.AddDynamic(this, &UTTTMainMenuWidget::OnQuitButtonPressed);
	}
}

void UTTTMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if(StartServerButton)
	{
		StartServerButton->OnPressed.RemoveDynamic(this, &UTTTMainMenuWidget::OnStartServerButtonPressed);
	}

	if(JoinServerButton)
	{
		JoinServerButton->OnPressed.RemoveDynamic(this, &UTTTMainMenuWidget::OnJoinServerButtonPressed);
	}

	if(QuitButton)
	{
		QuitButton->OnPressed.RemoveDynamic(this, &UTTTMainMenuWidget::OnQuitButtonPressed);
	}
}

void UTTTMainMenuWidget::OnStartServerButtonPressed()
{
	//UGameplayStatics::OpenLevel(this, FName(TEXT("TestMap")));
}

void UTTTMainMenuWidget::OnJoinServerButtonPressed()
{
	
}

void UTTTMainMenuWidget::OnQuitButtonPressed()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}