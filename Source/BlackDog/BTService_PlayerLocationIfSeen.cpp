// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIfSeen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"

UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen()
{
    NodeName = TEXT("Update Player Location If Seen");
}

void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn == nullptr)
    {
        return;
    }

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr)
    {
        return;
    }

    // Check if the AI has line of sight to the player
    if (AIController->LineOfSightTo(PlayerPawn))
    {
        FVector PlayerLocation = PlayerPawn->GetActorLocation();

        // Draw a line to visualize the Line of Sight check
        DrawDebugLine(GetWorld(), AIController->GetPawn()->GetActorLocation(), PlayerLocation, FColor::Green, false, 2.0f, 0, 5.0f);

        // Update the location last seen and set the PlayerDetected bool
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerDetected"), true);

        UE_LOG(LogTemp, Warning, TEXT("Player is in line of sight! Setting PlayerLocation: %s"), *PlayerLocation.ToString());
    }
    else
    {
        // Clear the Blackboard values if the player is no longer in sight
        OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerDetected"), false);
    }
}
