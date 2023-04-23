﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTTGame.generated.h"

class ATTTController;
class ATTTGameBoard;
class ATTTGameBoardField;
enum class ETTTGameStateType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameResetRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, ETTTGameStateType, GameStateType);

UENUM()
enum class ETTTGamePawnType : uint8
{
	Cross,
	Circle,
	Invalid UMETA(Hidden)
};

UENUM()
enum class ETTTGameStateType : uint8
{
	WaitForPlayers,
	//Pregame,
	Game,
	//PostGame,
	Pause,
	Invalid UMETA(Hidden)
};


UCLASS()
class TTTADVANCED_API ATTTGame : public AActor
{
	GENERATED_BODY()

public:
	ATTTGame();
	
	virtual void BeginPlay() override;
	
	bool RegisterControllerInGame(ATTTController* Controller);
	void UnRegisterControllerInGame(ATTTController* Controller);
	
	bool IsControllerRegisteredInGame(const ATTTController* Controller) const;
	bool TryPerformTurn(const ATTTController* Controller, ATTTGameBoardField* BoardField);
	TSubclassOf<AActor> GetGamePawnClass(ETTTGamePawnType GamePawnType);
	bool SetGameStateType(ETTTGameStateType NewGameStateType);
	ETTTGameStateType GetGameStateType() const { return GameStateType; }
	
private:
	void OnTurnPerformed(const ATTTController* Controller);
	void ResetGame();
	
public:
	UPROPERTY(EditDefaultsOnly, Category=Game)
	TSubclassOf<ATTTGameBoard> GameBoardClass;

	UPROPERTY(EditDefaultsOnly, Category=Game, meta=(AllowPrivateAccess = "true"))
	TMap< ETTTGamePawnType, TSubclassOf<AActor> > GamePawns;

	FOnGameResetRequested OnGameResetRequested;
	FOnGameStateChanged OnGameStateChanged;
	
private:
	TArray<ATTTController*> RegisteredControllers;

	UPROPERTY()
	ATTTGameBoard* GameBoard;

	ETTTGameStateType GameStateType = ETTTGameStateType::Invalid;
};