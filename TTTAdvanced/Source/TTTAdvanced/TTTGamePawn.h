// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTTGame.h"
#include "GameFramework/Actor.h"
#include "TTTGamePawn.generated.h"

UENUM(BlueprintType)
enum class EGamePawnState : uint8
{
	NotSpawned,
	OnSpawner,
	InAir,
	Placed,
	Invalid UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePawnStateChanged, EGamePawnState, NewState);

UCLASS()
class TTTADVANCED_API ATTTGamePawn : public AActor
{
	GENERATED_BODY()

public:
	ATTTGamePawn();

	void SetState(EGamePawnState NewState);
	EGamePawnState GetState() const;

	void SetPawnType(ETTTGamePawnType NewPawnType)
	{
		PawnType = NewPawnType;
	}
	
	ETTTGamePawnType GetPawnType() const
	{
		return PawnType;
	}

	virtual void Tick(float Delta) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnRep_GamePawnState();
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_GamePawnState)
	EGamePawnState GamePawnState = EGamePawnState::Invalid;

	UPROPERTY(BlueprintAssignable)
	FOnGamePawnStateChanged OnGamePawnStateChanged;

private:
	ETTTGamePawnType PawnType = ETTTGamePawnType::Invalid;
};
