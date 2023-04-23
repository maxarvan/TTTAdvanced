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

	virtual void NativeOnBegin(ATTTController* InOwnerController);
	virtual void NativeOnEnd();

	//bool IsStateHandled() const { return bStateHandled; }
	virtual UWorld* GetWorld() const override;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnBegin();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnEnd();

	//UFUNCTION(BlueprintCallable)
	// void StateHandled()
	// {
	// 	bStateHandled = true;
	// }
	
protected:
	bool bStateHandled = false;

	UPROPERTY(BlueprintReadOnly)
	ATTTController* OwnerController;
};

UCLASS()
class TTTADVANCED_API UTTTPauseGameStateHandler : public UTTTGameStateHandler
{
	GENERATED_BODY()

public:
	UTTTPauseGameStateHandler(){}
	
	virtual void NativeOnBegin(ATTTController* OwnerController) override;
	virtual void NativeOnEnd() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Widgets, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UTTTPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UTTTPauseMenuWidget> PauseMenuWidget; 
};
