// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPotion.h"

AHealthPotion::AHealthPotion() : AItem()
{
    // Set default values for the healing potion
    ItemName = "Health Potion";
    ItemDescription = "A potion that restores health.";
    HealingValue = 10.0f;  // Default healing value
    ItemType = EItemType::IT_Consumable;  // Set item type to Consumable
}

void AHealthPotion::CopyFrom(AHealthPotion* SourcePotion)
{
    if (SourcePotion)
    {
        // Call parent class CopyFrom to copy common item properties
        AItem::CopyFrom(SourcePotion);

        // Copy potion-specific properties
        this->HealingValue = SourcePotion->HealingValue;
        // Copy other potion-specific properties...
    }
}

