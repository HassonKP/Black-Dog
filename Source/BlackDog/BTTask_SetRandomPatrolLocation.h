// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetRandomPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDOG_API UBTTask_SetRandomPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SetRandomPatrolLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	float SearchRadius;
	
};
