// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_PauseAnimation.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDOG_API UAnimNotifyState_PauseAnimation : public UAnimNotifyState
{
	GENERATED_BODY()
public:
    // Called when the notify begins
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

    // Called when the notify ends
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
