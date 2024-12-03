// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "EnemyCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
   
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    AEnemyCharacter* Character = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (Character == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    Character->Attack();

    return EBTNodeResult::Succeeded;
}
