// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ATTTController.h"
#include "GameFramework/Actor.h"
#include "TTTGameBoard.generated.h"

class ATTTGameBoardField;
class ATTTGamePawn;

UCLASS()
class TTTADVANCED_API ATTTGameBoard : public AActor
{
	GENERATED_BODY()

public:
	ATTTGameBoard();
	
	bool TryOccupyBoardFieldWithGamePawn(const ATTTController* Controller, ATTTGamePawn* GamePawn, ATTTGameBoardField* BoardField);
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	bool IsBoardFilled() const { return OccupiedFieldsNum == BoardFields.Num(); }

private:
	UFUNCTION()
	void HandleResetGameRequested();
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATTTGameBoardField> BoardFieldClass;

	UPROPERTY(EditDefaultsOnly)
	int FieldsNumber = 3;

	UPROPERTY(EditDefaultsOnly)
	float DistanceBetweenFields = 100.f;

private:
	UPROPERTY()
	TArray<ATTTGameBoardField*> BoardFields;

	int OccupiedFieldsNum;
};
