// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "EnemyAICharacter.h"

AEnemyAIController::AEnemyAIController()
{
	
	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerceptionComponent"));


	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 50.0f;
	SightConfig->PeripheralVisionAngleDegrees = 180.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;


	EnemyPerceptionComponent->ConfigureSense(*SightConfig);
	EnemyPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());


}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AEnemyAIController::IsDead() const
{
	AAdventureCharacter* ControlledCharacter = Cast<AAdventureCharacter>(GetPawn());
	if (ControlledCharacter != nullptr)
	{
		return ControlledCharacter->IsDead();
	}
	return false;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyPerceptionComponent)
	{
		EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy Perception Component is missing in BeginPlay!"));
	}

	// Initialize Blackboard and Behavior Tree
	if (AIBehavior)
	{
		if (UseBlackboard(AIBehavior->BlackboardAsset, BlackboardComponent))
		{
			APawn* ControlledPawn = GetPawn();
			if (ControlledPawn)
			{
				FVector SpawnLocation = ControlledPawn->GetActorLocation();
				//BlackboardComponent->SetValueAsVector(TEXT("SpawnLocation"), SpawnLocation);
			}

			RunBehaviorTree(AIBehavior);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to initialize Blackboard in BeginPlay!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIBehavior is not set in BeginPlay!"));
	}
}

void AEnemyAIController::SetPlayerDetected(AActor* Actor, bool bDetected)
{
	if (Actor && BlackboardComponent)
	{
		if (bDetected)
		{
			// Set the Player Actor and Location in the Blackboard
			BlackboardComponent->SetValueAsObject(TEXT("Player"), Actor);
			FVector PlayerLocation = Actor->GetActorLocation();
			BlackboardComponent->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
			BlackboardComponent->SetValueAsBool(TEXT("PlayerDetected"), true);

		}
		else
		{
			// Clear the Player Actor and Location in the Blackboard
			BlackboardComponent->ClearValue(TEXT("Player"));
			BlackboardComponent->ClearValue(TEXT("PlayerLocation"));
			BlackboardComponent->SetValueAsBool(TEXT("PlayerDetected"), false);

		}
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor && Actor->IsA(AAdventureCharacter::StaticClass()))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			SetPlayerDetected(Actor, true);
		}
		else
		{
			SetPlayerDetected(Actor, false);
		}
	}
}