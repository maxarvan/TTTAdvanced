#include "TTTGameStateHandler.h"
#include "TTTPauseMenuWidget.h"
#include "ATTTController.h"

void UTTTPauseGameStateHandler::OnBegin(ATTTController* InOwnerController)
{
	Super::OnBegin(InOwnerController);
	
	if( PauseMenuWidgetClass )
	{
		PauseMenuWidget = CreateWidget<UTTTPauseMenuWidget>(Cast<APlayerController>(OwnerController), PauseMenuWidgetClass);
		PauseMenuWidget->AddToViewport();
	}
}

void UTTTPauseGameStateHandler::OnEnd()
{
	Super::OnEnd();
	
	if( PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}
}