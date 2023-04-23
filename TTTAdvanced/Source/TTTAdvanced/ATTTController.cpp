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
		Game->RegisterControllerInGame(this);
		Game->OnGameStateChanged.AddDynamic(this, &ATTTController::OnGameStateChanged);
	}
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
		EnhancedInputComponent->BindAction(MakeTurnInputAction, ETriggerEvent::Started, this, &ThisClass::OnMakeTurnInputPressed);
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
	// if(NewGameStateType == ETTTGameStateType::Pause && PauseMenuWidgetClass)
	// {
	// 	PauseMenuWidget = CreateWidget<UTTTPauseMenuWidget>(this, PauseMenuWidgetClass);
	// 	PauseMenuWidget->AddToViewport();
	// }
	// else
	// {
	// 	if(PauseMenuWidget)
	// 	{
	// 		PauseMenuWidget->RemoveFromParent();
	// 		PauseMenuWidget = nullptr;
	// 	}
	// }
}

void ATTTController::OnMakeTurnInputPressed()
{
	FHitResult Hit;

	if(GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
	{
		Server_PerformTurn(Hit);
	}
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
		OldHandler->OnEnd();
	}

	if(CurrentGameStateHandler)
	{
		CurrentGameStateHandler->OnBegin(this);
	}
}

void ATTTController::Server_PerformTurn_Implementation(const FHitResult& Hit)
{
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		if( ATTTGameBoardField* BoardField = Cast<ATTTGameBoardField>(Hit.GetActor()))
		{
			Game->TryPerformTurn(this, BoardField);
		}
	}
}

void ATTTController::Server_RequestGameState_Implementation(ETTTGameStateType NewState)
{
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->SetGameStateType(NewState);
	}
}