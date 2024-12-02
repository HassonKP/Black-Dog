// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdventureCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDOG_API AEnemyAICharacter : public AAdventureCharacter
{
	GENERATED_BODY()

public:
	AEnemyAICharacter();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* PerceptionComponent;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	
};
