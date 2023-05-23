// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTTGameStateHandler.h"
#include "GameFramework/PlayerController.h"
#include "ATTTController.generated.h"

class UInputAction;
class ATTTGamePawn;

enum class ETTTGameStateType : uint8;
enum class ETTTGamePawnType : uint8;

// UENUM(BlueprintType)
// enum class ETurnState : uint8
// {
// 	NotMyTurn,
// 	MyTurn,
//
// 	Invalid UMETA(Hidden)
// };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, bool, bIsMyTurn);

UCLASS()
class TTTADVANCED_API ATTTController : public APlayerController
{
	GENERATED_BODY()

public:
	ATTTController();

	void SetMyTurn(bool NewValue);
	bool GetIsMyTurn() const { return bIsMyTurn; }
	void SetGamePawnType(ETTTGamePawnType InGamePawnType) { GamePawnType = InGamePawnType; }
	ETTTGamePawnType GetGamePawnType() const { return GamePawnType; }
	void TryResumeGame();

	UFUNCTION(Server, Reliable)
	void Server_RequestGameRestart();
	
protected:
	virtual void SetupInputComponent() override;
	
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* PawnToPossess) override;
	virtual void OnUnPossess() override;
	
	void OnOperateGamePawnInputPressed();
	void OnPauseGameInputPressed();

private:
	bool PerformTurn(ATTTGamePawn* GamePawn, const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void Server_RequestGameState(ETTTGameStateType NewState);

	UFUNCTION(Server, Reliable)
	void Server_OperateGamePawn(const FHitResult& Hit);
	
	UFUNCTION()
	void OnRep_IsMyTurn();

	UFUNCTION()
	void OnRep_CurrentGameStateHandler(UTTTGameStateHandler* OldHandler);
	
	UFUNCTION()
	void OnGameStateChanged(ETTTGameStateType NewGameStateType);

	UFUNCTION(Client, Reliable)
	void Client_OnGameStateChanged(ETTTGameStateType NewGameStateType);

	void RespawnGamePawn();
		
public:
	UPROPERTY(BlueprintAssignable)
	FOnTurnChanged OnTurnChanged;
	
private:
	/** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* OperateGamePawnInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* PauseGameInputAction;

	UPROPERTY(EditAnywhere, Category=GameStates, meta = (AllowPrivateAccess = "true", ForceInlineRow))
	TMap<ETTTGameStateType, TSubclassOf<UTTTGameStateHandler>> GameStateHandlers;
	
	ETTTGamePawnType GamePawnType;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsMyTurn)
	mutable bool bIsMyTurn = true;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentGameStateHandler)
	UTTTGameStateHandler* CurrentGameStateHandler;

	// public for debug only
public:
	UPROPERTY()
	TObjectPtr<ATTTGamePawn> CurrentGamePawn = nullptr;
};
