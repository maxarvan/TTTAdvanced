// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTHelper.h"
#include "TTTAdvancedGameModeBase.h"

ATTTGame* UTTTHelper::GetGame(const UWorld* World)
{
	if(World)
	{
		if(const ATTTAdvancedGameModeBase* GameMode = Cast<ATTTAdvancedGameModeBase>(World->GetAuthGameMode()))
		{
			return GameMode->GetGame();
		}
	}
	return nullptr;
}
