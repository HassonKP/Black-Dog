// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetRandomPatrolLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Actor.h"

UBTTask_SetRandomPatrolLocation::UBTTask_SetRandomPatrolLocation()
{
    NodeName = "Set Random Patrol Location";  
    SearchRadius = 10000.0f;
}

EBTNodeResult::Type UBTTask_SetRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Get the AI controller and pawn
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        APawn* AIPawn = AIController->GetPawn();
        if (AIPawn)
        {
            // Get the navigation system to find a random point
            UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
            if (NavSystem)
            {
                // Get a random point within the defined radius around the AI
                FNavLocation RandomLocation;
                if (NavSystem->GetRandomPointInNavigableRadius(AIPawn->GetActorLocation(), SearchRadius, RandomLocation))
                {
                    // Set the random patrol location in the blackboard
                    OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("PatrolLocation"), RandomLocation.Location);

                    return EBTNodeResult::Succeeded;
                }
            }
        }
    }

    return EBTNodeResult::Failed;
}