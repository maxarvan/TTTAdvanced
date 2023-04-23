// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TTTHelper.generated.h"

class ATTTGame;
/**
 * 
 */
UCLASS()
class TTTADVANCED_API UTTTHelper : public UObject
{
	GENERATED_BODY()

public:
	static ATTTGame* GetGame(const UWorld* World);
};
