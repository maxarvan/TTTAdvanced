// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ATTTController.generated.h"

enum class ETTTGameStateType : uint8;
enum class ETTTGamePawnType : uint8;

class UTTTPauseMenuWidget;
class UInputAction;

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

protected:
	virtual void SetupInputComponent() override;
	
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* PawnToPossess) override;
	virtual void OnUnPossess() override;
	
	void OnMakeTurnInputPressed();
	void OnPauseGameInputPressed();

private:
	UFUNCTION(Server, Reliable)
	void Server_PerformTurn(const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void Server_PauseGame();

	UFUNCTION()
	void OnRep_IsMyTurn();

	UFUNCTION()
	void OnGameStateChanged(ETTTGameStateType NewGameStateType);

public:
	UPROPERTY(BlueprintAssignable)
	FOnTurnChanged OnTurnChanged;
	
private:
	/** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MakeTurnInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* PauseGameInputAction;
	
	ETTTGamePawnType GamePawnType;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsMyTurn)
	mutable bool bIsMyTurn = true;

	UPROPERTY()
	TObjectPtr<UTTTPauseMenuWidget> PauseMenuWidget; 
};
