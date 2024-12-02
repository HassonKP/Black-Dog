// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdatePlayerRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"

UBTService_UpdatePlayerRange::UBTService_UpdatePlayerRange()
{
    NodeName = TEXT("Update Player In Range");
}

void UBTService_UpdatePlayerRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    if (AIController == nullptr)
    {
        return;
    }

    AEnemyCharacter* ControlledEnemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (ControlledEnemy == nullptr)
    {
        return;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn == nullptr)
    {
        return;
    }

    float Distance = FVector::Dist(ControlledEnemy->GetActorLocation(), PlayerPawn->GetActorLocation());
    bool bInRange = Distance <= ControlledEnemy->AttackRange;

    OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerInRange"), bInRange);

}