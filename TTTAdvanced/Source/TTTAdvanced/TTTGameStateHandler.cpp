#include "TTTGameStateHandler.h"
#include "TTTPauseMenuWidget.h"
#include "ATTTController.h"

UWorld* UTTTGameStateHandler::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		return nullptr;
	}
	else if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

void UTTTGameStateHandler::NativeOnBegin(ATTTController* InOwnerController)
{
	OwnerController = InOwnerController;

	ensure(OwnerController);
	ensure(OwnerController->IsLocalPlayerController());
	
	BP_OnBegin();
}

void UTTTGameStateHandler::NativeOnEnd()
{
	BP_OnEnd();
}

void UTTTPauseGameStateHandler::NativeOnBegin(ATTTController* InOwnerController)
{
	Super::NativeOnBegin(InOwnerController);
	
	if( PauseMenuWidgetClass )
	{
		PauseMenuWidget = CreateWidget<UTTTPauseMenuWidget>(Cast<APlayerController>(OwnerController), PauseMenuWidgetClass);
		PauseMenuWidget->AddToViewport();
	}
}

void UTTTPauseGameStateHandler::NativeOnEnd()
{
	Super::NativeOnEnd();
	
	if(PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}
}