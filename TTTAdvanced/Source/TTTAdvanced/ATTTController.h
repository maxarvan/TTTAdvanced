// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTTGamePawn.h"
#include "TTTGameStateHandler.h"
#include "GameFramework/PlayerController.h"
#include "ATTTController.generated.h"

class UInputAction;
class ATTTGamePawn;

enum class ETTTGameStateType : uint8;
enum class ETTTGamePawnType : uint8;
enum class EGamePawnState : uint8;

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

	void HandleGamePawnStateChanged(ATTTGamePawn* GamePawn, EGamePawnState GamePawnState);
	
protected:
	virtual void SetupInputComponent() override;
	
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* PawnToPossess) override;
	virtual void OnUnPossess() override;
	
	void OnOperateGamePawnInputPressed();
	void OnPauseGameInputPressed();

private:
	bool CanPerformTurn(ATTTGamePawn* GamePawn, const FHitResult& Hit);
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
	void HandleGameStateChanged(ETTTGameStateType NewGameStateType);

	UFUNCTION()
	void HandleResetGameRequested();
	
	UFUNCTION(Client, Reliable)
	void Client_OnGameStateChanged(ETTTGameStateType NewGameStateType);

	void SpawnGamePawns();
	void DespawnGamePawns();
		
public:
	UPROPERTY(BlueprintAssignable)
	FOnTurnChanged OnTurnChanged;
	
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;

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
	
	UPROPERTY()
	TArray<TObjectPtr<ATTTGamePawn> > GamePawns;

	UPROPERTY()
	TObjectPtr<ATTTGamePawn> PawnInAir = nullptr;
};
