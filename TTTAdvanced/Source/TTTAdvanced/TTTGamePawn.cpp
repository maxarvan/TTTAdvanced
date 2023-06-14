// Fill out your copyright notice in the Description page of Project Settings.

#include "TTTGamePawn.h"

#include "ATTTController.h"
#include "Net/UnrealNetwork.h"

ATTTGamePawn::ATTTGamePawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATTTGamePawn::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SetReplicatingMovement(true);
		SetReplicates(true);
	}
}

void ATTTGamePawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTTGamePawn, GamePawnState);
}

void ATTTGamePawn::SetState(EGamePawnState NewState)
{
	GamePawnState = NewState;
	OnRep_GamePawnState();

	if(ATTTController* Controller = Cast<ATTTController>(GetOwner()))
	{
		Controller->HandleGamePawnStateChanged(this, GamePawnState);
	}
}

EGamePawnState ATTTGamePawn::GetState() const
{
	return GamePawnState;
}

void ATTTGamePawn::OnRep_GamePawnState()
{
	OnGamePawnStateChanged.Broadcast(GamePawnState);
}

void ATTTGamePawn::Tick(float Delta)
{
	//if( HasAuthority() )
	{
		const FString StateString = UEnum::GetDisplayValueAsText(GamePawnState).ToString();
		
		DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 50.f, StateString, nullptr, FColor::White, 0);
	}
}
