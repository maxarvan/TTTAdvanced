// Fill out your copyright notice in the Description page of Project Settings.

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

	bool IsOccupied() const { return bIsOccupied; }
	bool CanOccupyWithGamePawn(const ATTTGamePawn* GamePawn) const;
	bool OccupyWithGamePawn(ATTTGamePawn* GamePawn);
	bool AreOccupiedByGamePawnType(const ATTTGameBoardField* OtherField, ETTTGamePawnType GamePawnType) const;
	void ResetGame();

protected:
	virtual void BeginPlay() override;

private:
	bool bIsOccupied = false;

	UPROPERTY()
	ATTTGamePawn* OccupationPawn = nullptr;
};
