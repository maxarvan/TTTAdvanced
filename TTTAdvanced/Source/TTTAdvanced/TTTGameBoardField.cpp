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
	if(OccupationPawn)
	{
		OccupationPawn->SetLifeSpan(3.0f);
		OccupationPawn->SetActorHiddenInGame(true);
		OccupationPawn->SetActorEnableCollision(false);
		OccupationPawn = nullptr;
	}
}

bool ATTTGameBoardField::IsOccupied() const
{
	return IsValid(OccupationPawn);
}

bool ATTTGameBoardField::CanOccupyWithGamePawn(const ATTTGamePawn* GamePawn) const
{
	//return !bIsOccupied && GamePawn;
	if(!GamePawn)
	{
		return false;
	}
	
	return !IsOccupied() || (OccupationPawn->GetPawnSizeType() < GamePawn->GetPawnSizeType());
}

bool ATTTGameBoardField::OccupyWithGamePawn(ATTTGamePawn* GamePawn)
{
	if(CanOccupyWithGamePawn(GamePawn))
	{
		const FVector Location = GetActorLocation();
		const FRotator Rotation = GetActorRotation();

		GamePawn->SetState(EGamePawnState::Placed);
		
		GamePawn->TeleportTo(Location, Rotation);

		if(OccupationPawn)
		{
			OccupationPawn->SetLifeSpan(3.0f);
			OccupationPawn->SetActorHiddenInGame(true);
			OccupationPawn->SetActorEnableCollision(false);
		}
		
		OccupationPawn = GamePawn;

		return true;
	}
	return false;
}

bool ATTTGameBoardField::AreOccupiedByGamePawnType(const ATTTGameBoardField* OtherField, ETTTGamePawnType GamePawnType) const
{
	if(OtherField == nullptr)
	{
		return false;
	}
	
	if(IsOccupied() && OtherField->IsOccupied() && OccupationPawn->GetPawnType() == OtherField->OccupationPawn->GetPawnType())
	{
		return true;
	}

	return false;
}