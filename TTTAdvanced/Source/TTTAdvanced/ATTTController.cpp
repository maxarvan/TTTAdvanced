// Fill out your copyright notice in the Description page of Project Settings.

#include "ATTTController.h"

#include <SPIRV-Reflect/SPIRV-Reflect/include/spirv/unified1/spirv.h>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TTTGame.h"
#include "TTTGameBoardField.h"
#include "TTTHelper.h"
#include "TTTGamePawn.h"
#include "TTTPauseMenuWidget.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"

ATTTController::ATTTController()
{
	GamePawnType = ETTTGamePawnType::Invalid;
}

void ATTTController::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATTTController::OnPossess(APawn* PawnToPossess)
{
	Super::OnPossess(PawnToPossess);

	PawnToPossess->TeleportTo(StartSpot->GetActorLocation(), StartSpot->GetActorRotation());

	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->OnGameStateChanged.AddDynamic(this, &ATTTController::HandleGameStateChanged);
		Game->OnGameResetRequested.AddDynamic(this, &ATTTController::HandleResetGameRequested);
		Game->RegisterControllerInGame(this);
	}

	SpawnGamePawns();
}

void ATTTController::OnUnPossess()
{
	Super::OnUnPossess();

	DespawnGamePawns();
	
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->OnGameStateChanged.RemoveDynamic(this, &ATTTController::HandleGameStateChanged);
		Game->OnGameResetRequested.RemoveDynamic(this, &ATTTController::HandleResetGameRequested);
	}
}

void ATTTController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(OperateGamePawnInputAction, ETriggerEvent::Started, this, &ThisClass::OnOperateGamePawnInputPressed);
		EnhancedInputComponent->BindAction(PauseGameInputAction, ETriggerEvent::Started, this, &ThisClass::OnPauseGameInputPressed);
	}
}

void ATTTController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTTController, bIsMyTurn);
	DOREPLIFETIME(ATTTController, CurrentGameStateHandler);
}

void ATTTController::TryResumeGame()
{
	Server_RequestGameState(ETTTGameStateType::Game);
}

void ATTTController::HandleGameStateChanged(ETTTGameStateType NewGameStateType)
{
	Client_OnGameStateChanged(NewGameStateType);
}

void ATTTController::HandleResetGameRequested()
{
	DespawnGamePawns();
	SpawnGamePawns();
}

void ATTTController::SpawnGamePawns()
{
	if(!HasAuthority())
	{
		return;
	}

	ATTTGame* Game = UTTTHelper::GetGame(GetWorld());
	if(Game == nullptr)
	{
		return;
	}

	auto SpawnPawn = [&](const FVector& Location)->ATTTGamePawn*
	{
		const TSubclassOf<ATTTGamePawn> GamePawnClass = Game->GetGamePawnClass(GamePawnType);
		const FRotator Rotation = GetPawn()->GetActorRotation();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ATTTGamePawn* Pawn = Cast<ATTTGamePawn>(GetWorld()->SpawnActor(GamePawnClass, &Location, &Rotation, SpawnParameters));

		Pawn->SetState(EGamePawnState::OnSpawner);

		return Pawn;
	};

	constexpr int PawnsNumber = 6;
	constexpr float DistanceBetweenPawns = 60.f;

	const FVector InitialOffset = -FVector::RightVector * DistanceBetweenPawns * (PawnsNumber - 1) * 0.5f;
	
	for(int i = 0 ; i < PawnsNumber ; ++i)
	{
		FVector Location = GetPawn()->GetActorLocation() + FVector::RightVector * DistanceBetweenPawns * i + InitialOffset;
		GamePawns.Add(SpawnPawn(Location));
	}
}

void ATTTController::DespawnGamePawns()
{
	if(!HasAuthority())
	{
		return;
	}

	for(const auto& GamePawn : GamePawns)
	{
		if(IsValid(GamePawn))
		{
			GamePawn->Destroy();
		}
	}

	GamePawns.Empty();
}

void ATTTController::OnOperateGamePawnInputPressed()
{
	FHitResult Hit;
	
	if(!GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
	{
		return;
	}
	
	Server_OperateGamePawn(Hit);
}

void ATTTController::OnPauseGameInputPressed()
{
	Server_RequestGameState(ETTTGameStateType::Pause);
}

void ATTTController::SetMyTurn(bool NewValue)
{
	bIsMyTurn = NewValue;

	if(HasAuthority())
	{
		OnRep_IsMyTurn();
	}
}

void ATTTController::OnRep_IsMyTurn()
{
	OnTurnChanged.Broadcast(bIsMyTurn);
}

void ATTTController::OnRep_CurrentGameStateHandler(UTTTGameStateHandler* OldHandler)
{
	if(OldHandler)
	{
		OldHandler->NativeOnEnd();
	}

	if(CurrentGameStateHandler)
	{
		CurrentGameStateHandler->NativeOnBegin(this);
	}
}

void ATTTController::Client_OnGameStateChanged_Implementation(ETTTGameStateType NewGameStateType)
{
	UTTTGameStateHandler* OldHandler = CurrentGameStateHandler;
	
	if(TSubclassOf<UTTTGameStateHandler>* NewHandlerType = GameStateHandlers.Find(NewGameStateType))
	{
		const UClass* ClassType = NewHandlerType->GetDefaultObject()->GetClass();
		CurrentGameStateHandler = NewObject<UTTTGameStateHandler>(this, ClassType);
	}
	else
	{
		CurrentGameStateHandler = nullptr;
	}

	OnRep_CurrentGameStateHandler(OldHandler);
}

void ATTTController::SetPawnState(ATTTGamePawn* GamePawn, EGamePawnState State)
{
	if(GamePawn)
	{
		GamePawn->SetState(State);

		if(PawnInAir && PawnInAir != GamePawn)
		{
			PawnInAir->SetState(EGamePawnState::OnSpawner);
		}

		PawnInAir = nullptr;

		if(State == EGamePawnState::InAir)
		{
			PawnInAir = GamePawn;
		}
	}
}

void ATTTController::Server_OperateGamePawn_Implementation(const FHitResult& Hit)
{
	if(!GetIsMyTurn())
	{
		return;
	}

	const TObjectPtr<ATTTGamePawn> HitPawn = Cast<ATTTGamePawn>(Hit.GetActor());
	const TObjectPtr<ATTTGameBoardField> HitField = Cast<ATTTGameBoardField>(Hit.GetActor());

	if(HitPawn)
	{
		switch(HitPawn->GetState())
		{
		case EGamePawnState::OnSpawner:
			SetPawnState(HitPawn, EGamePawnState::InAir);
			break;
		
		case EGamePawnState::InAir:
			SetPawnState(HitPawn, EGamePawnState::OnSpawner);
			break;
			
		default:
			break;
		}
	}
	else if(HitField && PawnInAir)
	{
		if(CanPerformTurn(PawnInAir, Hit))
		{
			ATTTGamePawn* ShitPawn = PawnInAir;
			SetPawnState(PawnInAir, EGamePawnState::Placed);

			// SHITTTTT ShitPawn
			
			PerformTurn(ShitPawn, Hit);
		}
	}
	else if(PawnInAir)
	{
		SetPawnState(PawnInAir, EGamePawnState::OnSpawner);
	}
}

void ATTTController::Server_RequestGameRestart_Implementation()
{
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->ResetGame();
	}
}

bool ATTTController::CanPerformTurn(ATTTGamePawn* GamePawn, const FHitResult& Hit)
{
	ensure(GamePawn);
	
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		if(ATTTGameBoardField* BoardField = Cast<ATTTGameBoardField>(Hit.GetActor()))
		{
			return Game->CanPerformTurn(this, GamePawn, BoardField);
		}
	}

	return false;
}

bool ATTTController::PerformTurn(ATTTGamePawn* GamePawn, const FHitResult& Hit)
{
	ensure(GamePawn);
	
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		if(ATTTGameBoardField* BoardField = Cast<ATTTGameBoardField>(Hit.GetActor()))
		{
			return Game->PerformTurn(this, GamePawn, BoardField);
		}
	}

	return false;
}

void ATTTController::Server_RequestGameState_Implementation(ETTTGameStateType NewState)
{
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->SetGameStateType(NewState);
	}
}