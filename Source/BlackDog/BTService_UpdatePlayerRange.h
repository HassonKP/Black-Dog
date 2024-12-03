// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdatePlayerRange.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDOG_API UBTService_UpdatePlayerRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
    UBTService_UpdatePlayerRange();

protected:
    // Called every tick interval to update the Blackboard value
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
