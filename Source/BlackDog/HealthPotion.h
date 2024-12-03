// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "HealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class BLACKDOG_API AHealthPotion : public AItem
{
	GENERATED_BODY()
public:
	AHealthPotion();
	void CopyFrom(AHealthPotion* SourcePotion);
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Properties")
	float HealingValue;

	virtual EItemType GetItemType() const override { return EItemType::IT_Consumable; }

	UFUNCTION(BlueprintCallable, Category = "Potion Actions")
	float UseHealthPotion() const { return HealingValue; }
};
