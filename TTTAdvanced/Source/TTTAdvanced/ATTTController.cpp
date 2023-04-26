// Fill out your copyright notice in the Description page of Project Settings.

#include "ATTTController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TTTGame.h"
#include "TTTGameBoardField.h"
#include "TTTHelper.h"
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
		Game->OnGameStateChanged.AddDynamic(this, &ATTTController::OnGameStateChanged);
		Game->RegisterControllerInGame(this);
	}

	RespawnGamePawn();
	SetGamePawnState(EGamePawnState::OnSpawner);
}

void ATTTController::SetGamePawnState(EGamePawnState NewState)
{
	GamePawnState = NewState;
	OnRep_GamePawnState();
}


void ATTTController::OnUnPossess()
{
	Super::OnUnPossess();

	// if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	// {
	// 	Game->UnRegisterControllerInGame(this);
	// }
}

void ATTTController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
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

void ATTTController::OnGameStateChanged(ETTTGameStateType NewGameStateType)
{
	Client_OnGameStateChanged(NewGameStateType);
}

void ATTTController::RespawnGamePawn()
{
	if(!HasAuthority())
	{
		return;
	}
	
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		const TSubclassOf<AActor> GamePawnClass = Game->GetGamePawnClass(GamePawnType);
		const FVector Location = GetPawn()->GetActorLocation();
		const FRotator Rotation = GetPawn()->GetActorRotation();
		const FActorSpawnParameters SpawnParameters;
		
		CurrentGamePawn = GetWorld()->SpawnActor(GamePawnClass, &Location, &Rotation, SpawnParameters);
	}
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

void ATTTController::OnRep_GamePawnState()
{
	
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

void ATTTController::Server_OperateGamePawn_Implementation(const FHitResult& Hit)
{
	if(CurrentGamePawn == nullptr)
	{
		return;
	}
		
	switch(GamePawnState)
	{
	case EGamePawnState::OnSpawner:
		{
			if(Hit.GetActor() == CurrentGamePawn)
			{
				SetGamePawnState(EGamePawnState::InAir);
			}
		}
		break;
		
	case EGamePawnState::InAir:
		if(PerformTurn(CurrentGamePawn, Hit))
		{
			RespawnGamePawn();
			SetGamePawnState(EGamePawnState::OnSpawner);
		}
		break;

	default:
		break;
	}
}

void ATTTController::Server_RequestGameRestart_Implementation()
{
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->ResetGame();
	}
}

bool ATTTController::PerformTurn(AActor* GamePawn, const FHitResult& Hit)
{
	ensure(GamePawn);
	
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		if(ATTTGameBoardField* BoardField = Cast<ATTTGameBoardField>(Hit.GetActor()))
		{
			return Game->TryPerformTurn(this, CurrentGamePawn, BoardField);
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