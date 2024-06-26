﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTTGameBoardField.generated.h"

class ATTTGamePawn;
enum class ETTTGamePawnType : uint8;

UCLASS()
class TTTADVANCED_API ATTTGameBoardField : public AActor
{
	GENERATED_BODY()

public:
	ATTTGameBoardField();

	bool IsOccupied() const;
	bool CanOccupyWithGamePawn(const ATTTGamePawn* GamePawn) const;
	bool OccupyWithGamePawn(ATTTGamePawn* GamePawn);
	bool AreOccupiedByGamePawnType(const ATTTGameBoardField* OtherField, ETTTGamePawnType GamePawnType) const;
	void ResetGame();

	UFUNCTION(BlueprintImplementableEvent)
	void MarkAsValidForTurn();

	UFUNCTION(BlueprintImplementableEvent)
	void MarkAsInvalidForTurn();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ResetMarking();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ATTTGamePawn* OccupationPawn = nullptr;
};
