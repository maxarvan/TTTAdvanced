// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTTGame.generated.h"

class ATTTController;
class ATTTGameBoard;
class ATTTGameBoardField;
class ATTTGamePawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameResetRequested);

UENUM()
enum class ETTTGamePawnType : uint8
{
	Cross,
	Circle,
	Hidden UMETA(Hidden),
	Invalid UMETA(Hidden)
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETTTGamePawnType, ETTTGamePawnType::Cross, ETTTGamePawnType::Circle);

UENUM()
enum class ETTTGameStateType : uint8
{
	WaitForPlayers,
	Game,
	PostGame,
	Pause,
	Invalid UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, ETTTGameStateType, GameStateType);

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
	bool CanPerformTurn(const ATTTController* Controller, ATTTGamePawn* GamePawn, ATTTGameBoardField* BoardField) const;
	bool PerformTurn(const ATTTController* Controller, ATTTGamePawn* GamePawn, ATTTGameBoardField* BoardField);
	TSubclassOf<ATTTGamePawn> GetGamePawnClass(ETTTGamePawnType GamePawnType);
	bool SetGameStateType(ETTTGameStateType NewGameStateType);
	ETTTGameStateType GetGameStateType() const { return GameStateType; }
	void ResetGame();
	
private:
	void OnTurnPerformed(const ATTTController* Controller);
	
public:
	UPROPERTY(EditDefaultsOnly, Category=Game)
	TSubclassOf<ATTTGameBoard> GameBoardClass;

	UPROPERTY(EditDefaultsOnly, Category=Game, meta=(AllowPrivateAccess = "true"))
	TMap< ETTTGamePawnType, TSubclassOf<ATTTGamePawn> > GamePawns;

	FOnGameResetRequested OnGameResetRequested;
	FOnGameStateChanged OnGameStateChanged;
	
private:
	TArray<ATTTController*> RegisteredControllers;

	UPROPERTY()
	ATTTGameBoard* GameBoard;
	
	ETTTGameStateType GameStateType = ETTTGameStateType::Invalid;
};
