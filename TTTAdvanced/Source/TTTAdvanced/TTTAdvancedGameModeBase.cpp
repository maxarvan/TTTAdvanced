// Copyright Epic Games, Inc. All Rights Reserved.


#include "TTTAdvancedGameModeBase.h"

#include "EngineUtils.h"
#include "TTTGame.h"
#include "GameFramework/PlayerStart.h"

void ATTTAdvancedGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	SpawnGame();
}

AActor* ATTTAdvancedGameModeBase::GetPlayerStartLocal(AController* PlayerController)
{
	UWorld* World = GetWorld();
	const bool bControlledByServer = PlayerController->IsLocalPlayerController();
	
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if(PlayerStart->PlayerStartTag == "Server" && bControlledByServer)
		{
			return PlayerStart;
		}
		
		if(PlayerStart->PlayerStartTag == "Client" && !bControlledByServer)
		{
			return PlayerStart;
		}
	}
	return nullptr;
}

bool ATTTAdvancedGameModeBase::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	OutErrorMessage.Reset();
	
	if (AActor* const StartSpot = GetPlayerStartLocal(Player))
	{
		const FVector StartLocation = StartSpot->GetActorLocation();
		const FRotator StartRotation = StartSpot->GetActorRotation();
		Player->SetInitialLocationAndRotation(StartLocation, StartRotation);

		Player->StartSpot = StartSpot;

		return true;
	}

	OutErrorMessage = FString::Printf(TEXT("Could not find a starting spot"));
	return false;
}

void ATTTAdvancedGameModeBase::SpawnGame()
{
	UWorld* World = GetWorld();
	
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		const APlayerStart* StartSpot = *It;

		if(StartSpot->PlayerStartTag == "Board")
		{
			const FActorSpawnParameters SpawnParameters;
			const FVector StartLocation = StartSpot->GetActorLocation();
			const FRotator StartRotation = StartSpot->GetActorRotation();
			Game = Cast<ATTTGame>(World->SpawnActor(GameClass, &StartLocation, &StartRotation, SpawnParameters));
		}
	}
}