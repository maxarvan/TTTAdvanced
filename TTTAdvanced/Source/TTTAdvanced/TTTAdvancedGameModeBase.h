// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TTTAdvancedGameModeBase.generated.h"

class ATTTGame;
/**
 * 
 */
UCLASS()
class TTTADVANCED_API ATTTAdvancedGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATTTGame* GetGame() const { return Game; }
	
protected:
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
private:
	AActor* GetPlayerStartLocal(AController* Player);
	void SpawnGame();

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATTTGame> GameClass;
	
private:
	UPROPERTY()
	ATTTGame* Game;
};
