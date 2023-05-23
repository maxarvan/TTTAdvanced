// Fill out your copyright notice in the Description page of Project Settings.

#include "TTTGameBoardField.h"
#include "TTTGamePawn.h"

ATTTGameBoardField::ATTTGameBoardField()
{
}

void ATTTGameBoardField::BeginPlay()
{
	Super::BeginPlay();
}

void ATTTGameBoardField::ResetGame()
{
	bIsOccupied = false;

	if(OccupationPawn)
	{
		OccupationPawn->SetLifeSpan(3.0f);
		OccupationPawn->SetActorHiddenInGame(true);
		OccupationPawn->SetActorEnableCollision(false);
		OccupationPawn = nullptr;
	}
}

// bool ATTTGameBoardField::TryOccupy(TSubclassOf<AActor> GamePawnClass)
// {
// 	if(!bIsOccupied)
// 	{
// 		const FActorSpawnParameters SpawnParameters;
// 		const FVector Location = GetActorLocation();
// 		const FRotator Rotation = GetActorRotation();
//
// 		OccupationPawn = GetWorld()->SpawnActor(GamePawnClass, &Location, &Rotation, SpawnParameters);
// 	
// 		return bIsOccupied = true;	
// 	}
// 	return false;
// }
//

bool ATTTGameBoardField::TryOccupyWithGamePawn(ATTTGamePawn* GamePawn)
{
	if(!bIsOccupied && GamePawn)
	{
		const FVector Location = GetActorLocation();
		const FRotator Rotation = GetActorRotation();

		GamePawn->SetState(EGamePawnState::Placed);
		
		GamePawn->TeleportTo(Location, Rotation);
	
		return bIsOccupied = true;	
	}
	return false;
}
