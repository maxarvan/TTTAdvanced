

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TTTGameStateHandler.generated.h"

class UTTTPauseMenuWidget;
class ATTTController;

UCLASS(Abstract, Blueprintable)
class TTTADVANCED_API UTTTGameStateHandler : public UObject
{
	GENERATED_BODY()

public:
	UTTTGameStateHandler(){}
	
	virtual void OnBegin(ATTTController* InOwnerController)
	{
		OwnerController = InOwnerController;
	}
	
	virtual void OnEnd() {}
	
	//UFUNCTION(BlueprintCallable)
	void StateHandled()
	{
		bStateHandled = true;
	}

	bool IsStateHandled() const { return bStateHandled; }
	
protected:
	bool bStateHandled = false;

	UPROPERTY()
	ATTTController* OwnerController;
};

UCLASS()
class TTTADVANCED_API UTTTPauseGameStateHandler : public UTTTGameStateHandler
{
	GENERATED_BODY()

public:
	UTTTPauseGameStateHandler(){}
	
	virtual void OnBegin(ATTTController* OwnerController) override;
	virtual void OnEnd() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Widgets, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UTTTPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UTTTPauseMenuWidget> PauseMenuWidget; 
};
