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
	 PrimaryActorTick.bCanEverTick = true;
	// PrimaryActorTick.bTickEvenWhenPaused = false;
	// PrimaryActorTick.TickInterval = 0.5f;
	
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
	DOREPLIFETIME(ATTTController, PawnInAir);
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

void ATTTController::HandleGamePawnStateChanged(ATTTGamePawn* GamePawn, EGamePawnState GamePawnState)
{
	if(GamePawn)
	{
		if(PawnInAir && PawnInAir != GamePawn)
		{
			PawnInAir->SetState(EGamePawnState::OnSpawner);
		}

		PawnInAir = nullptr;

		if(GamePawnState == EGamePawnState::InAir)
		{
			PawnInAir = GamePawn;
		}
	}
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

	auto SpawnPawn = [&](const FVector& Location, ETTTGamePawnSizeType PawnSizeType)->ATTTGamePawn*
	{
		const TSubclassOf<ATTTGamePawn> GamePawnClass = Game->GetGamePawnClass(GamePawnType);
		const FRotator Rotation = GetPawn()->GetActorRotation();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Owner = this;

		ATTTGamePawn* Pawn = Cast<ATTTGamePawn>(GetWorld()->SpawnActor(GamePawnClass, &Location, &Rotation, SpawnParameters));

		Pawn->SetState(EGamePawnState::OnSpawner);
		Pawn->SetPawnType(GamePawnType);
		Pawn->SetPawnSizeType(PawnSizeType);

		float Scale = 1.0f;
		switch (PawnSizeType)
		{
		case ETTTGamePawnSizeType::Small:
			Scale = .6f;
			break;
		case ETTTGamePawnSizeType::Medium:
			Scale = .8f;
			break;
		case ETTTGamePawnSizeType::Large:
			Scale = 1.1f;
			break;
		}

		Pawn->SetActorScale3D(FVector(Scale));
		
		return Pawn;
	};

	if(Game->GamePawnSizeOrder.IsEmpty())
	{
		return;
	}
	
	constexpr float DistanceBetweenPawns = 60.f;
	const FVector InitialOffset = -FVector::RightVector * DistanceBetweenPawns * (Game->GamePawnSizeOrder.Num() - 1) * 0.5f;
	
	for(int i = 0 ; i < Game->GamePawnSizeOrder.Num() ; ++i)
	{
		FVector Location = GetPawn()->GetActorLocation() + FVector::RightVector * DistanceBetweenPawns * i + InitialOffset;
		GamePawns.Add(SpawnPawn(Location, Game->GamePawnSizeOrder[i]));
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

void ATTTController::Server_OperateGamePawn_Implementation(const FHitResult& Hit)
{
	if(!GetIsMyTurn())
	{
		return;
	}

	TObjectPtr<ATTTGamePawn> HitPawn = Cast<ATTTGamePawn>(Hit.GetActor());
	TObjectPtr<ATTTGameBoardField> HitField = Cast<ATTTGameBoardField>(Hit.GetActor());

	if(PawnInAir && IsValid(HitPawn) && HitPawn->GetState() == EGamePawnState::Placed)
	{
		HitField = HitPawn->GetOccupiedBoardField();
		HitPawn = nullptr;
	}
	
	if(HitPawn)
	{
		switch(HitPawn->GetState())
		{
		case EGamePawnState::OnSpawner:
			HitPawn->SetState(EGamePawnState::InAir);
			break;
		
		case EGamePawnState::InAir:
			HitPawn->SetState(EGamePawnState::OnSpawner);
			break;
			
		default:
			break;
		}
	}
	else if(HitField && PawnInAir)
	{
		if(CanPerformTurn(PawnInAir, HitField))
		{
			PerformTurn(PawnInAir, HitField);
		}
	}
	else if(PawnInAir)
	{
		PawnInAir->SetState(EGamePawnState::OnSpawner);
	}
}

void ATTTController::Server_RequestGameRestart_Implementation()
{
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		Game->ResetGame();
	}
}

bool ATTTController::CanPerformTurn(const ATTTGamePawn* GamePawn, const ATTTGameBoardField* BoardField) const
{
	ensure(GamePawn);
	
	if(const ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		return Game->CanPerformTurn(this, GamePawn, BoardField);
	}

	return false;
}

bool ATTTController::PerformTurn(ATTTGamePawn* GamePawn, ATTTGameBoardField* BoardField)
{
	ensure(GamePawn);
	
	if(ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
	{
		return Game->PerformTurn(this, GamePawn, BoardField);
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

void ATTTController::Tick(float DeltaSeconds)
{
	if(IsLocalPlayerController() && GetIsMyTurn())
	{
		if(PawnInAir)
		{
			FHitResult Hit;
	
			if(GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
			{
				TObjectPtr<ATTTGamePawn> HitPawn = Cast<ATTTGamePawn>(Hit.GetActor());
				TObjectPtr<ATTTGameBoardField> HitField = Cast<ATTTGameBoardField>(Hit.GetActor());

				if(!HitField && HitPawn)
				{
					HitField = HitPawn->GetOccupiedBoardField();
				}

				if(HitField)
				{
					if(const ATTTGame* Game = UTTTHelper::GetGame(GetWorld()))
					{
						if(Game->CanPerformTurn(this, PawnInAir, HitField))
						{
							HitField->MarkAsValidForTurn();
						}
						else
						{
							HitField->MarkAsInvalidForTurn();
						}

						if(IsValid(LastMarkedField) && HitField != LastMarkedField)
						{
							LastMarkedField->ResetMarking();
							LastMarkedField = nullptr;
						}
						
						LastMarkedField = HitField;
						return;
					}
				}
			}
		}
	}

	if(IsValid(LastMarkedField))
	{
		LastMarkedField->ResetMarking();
		LastMarkedField = nullptr;
	}
}