// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTGame.h"

#include "ATTTController.h"
#include "TTTGameBoard.h"
#include "TTTGameBoardField.h"

ATTTGame::ATTTGame()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = false;
}

void ATTTGame::BeginPlay()
{
	Super::BeginPlay();

	RegisteredControllers.Reserve(2);
	
	const FActorSpawnParameters SpawnParameters;
	const FVector Location = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	GameBoard = Cast<ATTTGameBoard>(GetWorld()->SpawnActor(GameBoardClass, &Location, &Rotation, SpawnParameters));

	SetGameStateType(ETTTGameStateType::WaitForPlayers);
}

void ATTTGame::ResetGame()
{
	SetGameStateType(ETTTGameStateType::Game);
	
	for(int i = 0 ; i < RegisteredControllers.Num() ; ++i)
	{
		RegisteredControllers[i]->SetMyTurn(i == 0);
	}

	OnGameResetRequested.Broadcast();
}

bool ATTTGame::SetGameStateType(ETTTGameStateType NewGameStateType)
{
	if(NewGameStateType == ETTTGameStateType::Invalid)
	{
		return false;
	}
	
	GameStateType = NewGameStateType;

	OnGameStateChanged.Broadcast(GameStateType);
	return true;
}

bool ATTTGame::RegisterControllerInGame(ATTTController* Controller)
{
	const int Num = RegisteredControllers.Num();
	
	if(Num >= 2)
	{
		return false;
	}

	if(Num == 0)
	{
		Controller->SetGamePawnType(ETTTGamePawnType::Circle);
		Controller->SetMyTurn(true);
	}
	else
	{
		Controller->SetGamePawnType(ETTTGamePawnType::Cross);
		Controller->SetMyTurn(false);
	}
	
	RegisteredControllers.Add(Controller);

	if(RegisteredControllers.Num() == 2)
	{
		SetGameStateType(ETTTGameStateType::Game);
	}
	
	return true;
}

void ATTTGame::UnRegisterControllerInGame(ATTTController* Controller)
{
	RegisteredControllers.Remove(Controller);
}

bool ATTTGame::IsControllerRegisteredInGame(const ATTTController* Controller) const
{
	return RegisteredControllers.Find(const_cast<ATTTController*>(Controller)) != INDEX_NONE;
}

TSubclassOf<AActor> ATTTGame::GetGamePawnClass(ETTTGamePawnType GamePawnType)
{
	if(const TSubclassOf<AActor>* Class = GamePawns.Find(GamePawnType))
	{
		return *Class;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("No Game Pawn found"));
	return nullptr;
}

bool ATTTGame::TryPerformTurn(const ATTTController* Controller, AActor* GamePawn, ATTTGameBoardField* BoardField)
{
	ensure(Controller);
	ensure(GamePawn);

	if(GetGameStateType() != ETTTGameStateType::Game)
	{
		return false;
	}
	
	if(!IsControllerRegisteredInGame(Controller))
	{
		return false;
	}
	
	if(!Controller->GetIsMyTurn())
	{
		return false;
	}
	
	if(GameBoard->TryOccupyBoardFieldWithGamePawn(Controller, GamePawn, BoardField))
	{
		for(auto* RegCntrl : RegisteredControllers)
		{
			if(RegCntrl == Controller)
			{
				RegCntrl->SetMyTurn(false);
			}
			else
			{
				RegCntrl->SetMyTurn(true);
			}
		}

		OnTurnPerformed(Controller);
		return true;
	}
	
	return false;
}

void ATTTGame::OnTurnPerformed(const ATTTController* Controller)
{
	if(GameBoard->IsBoardFilled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Game Ended"));
		SetGameStateType(ETTTGameStateType::PostGame);
	}
}


