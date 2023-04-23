﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTTGameBoardField.generated.h"

UCLASS()
class TTTADVANCED_API ATTTGameBoardField : public AActor
{
	GENERATED_BODY()

public:
	ATTTGameBoardField();

	bool IsOccupied() const { return bIsOccupied; }
	bool TryOccupy(TSubclassOf<AActor> GamePawnClass);

	void ResetGame();

protected:
	virtual void BeginPlay() override;

private:
	bool bIsOccupied = false;

	UPROPERTY()
	AActor* OccupationPawn = nullptr;
};