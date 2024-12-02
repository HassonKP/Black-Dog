// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BlackDogGameMode.generated.h"

UCLASS(minimalapi)
class ABlackDogGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABlackDogGameMode();
	void PawnKilled(APawn* PawnKilled);
};



