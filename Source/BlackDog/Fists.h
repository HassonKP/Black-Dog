// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Fists.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDOG_API AFists : public AWeapon
{
	GENERATED_BODY()
	
public:
	AFists();

protected:
	virtual void BeginPlay() override;

public:
	virtual void DealDamage() override;

	virtual void StartNewAttack() override;

private:
	// Left and Right fist collision components
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* LeftFistCollision;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* RightFistCollision;

	void ActivateLeftFist();

	void ActivateRightFist();

	// Handle damage detection for each hand
	void HandleFistHit(UBoxComponent* FistComponent);

};
