// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTGameBoard.h"

#include "TTTGame.h"
#include "TTTGameBoardField.h"
#include "TTTHelper.h"


ATTTGameBoard::ATTTGameBoard()
{
}

void ATTTGameBoard::BeginPlay()
{
	Super::BeginPlay();
	
	// BoardFields
	BoardFields.Reserve(FieldsNumber * FieldsNumber);
	BoardFields.Init(nullptr, FieldsNumber * FieldsNumber);
	
	for(int y = 0 ; y < FieldsNumber ; ++y)
	{
		const float OffsetDistance = (1 - FieldsNumber) * DistanceBetweenFields * 0.5f;
		FVector ZeroLocation = GetActorLocation() + FVector(OffsetDistance, OffsetDistance, 0.f);
		const FRotator Rotation = GetActorRotation();
		
		for(int x = 0 ; x < FieldsNumber ; ++x)
		{
			const FActorSpawnParameters SpawnParameters;
			const FVector Location = ZeroLocation + x * FVector::XAxisVector * DistanceBetweenFields + y * FVector::YAxisVector * DistanceBetweenFields;

			ATTTGameBoardField* Field = Cast<ATTTGameBoardField>(GetWorld()->SpawnActor(BoardFieldClass, &Location, &Rotation, SpawnParameters));
			BoardFields[x + y * FieldsNumber] = Field;
		}
	}

	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->OnGameResetRequested.AddDynamic(this, &ATTTGameBoard::HandleResetGameRequested);
	}
}

void ATTTGameBoard::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->OnGameResetRequested.RemoveDynamic(this, &ATTTGameBoard::HandleResetGameRequested);
	}
}

void ATTTGameBoard::HandleResetGameRequested()
{
	OccupiedFieldsNum = 0;

	for(const auto& Field : BoardFields)
	{
		Field->ResetGame();
	}
}


bool ATTTGameBoard::TryOccupyBoardField(const ATTTController* Controller, ATTTGameBoardField* BoardField)
{
	if(Controller == nullptr || BoardField == nullptr)
	{
		return false;
	}

	if(BoardFields.Find(BoardField) == INDEX_NONE)
	{
		return false;
	}
	
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		const ETTTGamePawnType PawnType = Controller->GetGamePawnType();
		const bool OccupationResult = BoardField->TryOccupy(Game->GetGamePawnClass(PawnType));

		if(OccupationResult)
		{
			OccupiedFieldsNum++;
		}
		
		return OccupationResult;
	}

	return false;
}