// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAICharacter.h"
#include "EnemyAIController.h"

AEnemyAICharacter::AEnemyAICharacter()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1000.0f;
    SightConfig->LoseSightRadius = 1200.0f;
    SightConfig->PeripheralVisionAngleDegrees = 120.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAICharacter::OnTargetPerceptionUpdated);

}

void AEnemyAICharacter::BeginPlay()
{
    Super::BeginPlay();
}


void AEnemyAICharacter::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    AEnemyAIController* AIController = Cast< AEnemyAIController>(GetController());
    if (AIController && Actor && Actor->IsA(AAdventureCharacter::StaticClass()))  // Replace with your player class
    {
        if (Stimulus.WasSuccessfullySensed())  // Player is detected
        {
            AIController->SetPlayerDetected(Actor, true);
            //UE_LOG(LogTemp, Warning, TEXT("Player detected: %s"), *Actor->GetName());
        }
        else  // Player is no longer detected
        {
            AIController->SetPlayerDetected(Actor, false);
            //UE_LOG(LogTemp, Warning, TEXT("Player lost: %s"), *Actor->GetName());
        }
    }
}
