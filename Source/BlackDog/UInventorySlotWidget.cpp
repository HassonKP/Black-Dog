// Fill out your copyright notice in the Description page of Project Settings.


#include "UInventorySlotWidget.h"
#include "Blueprint/UserWidget.h"
#include <Blueprint/WidgetBlueprintLibrary.h>
#include "Blueprint/DragDropOperation.h"
#include "AdventureCharacter.h"
#include "GameFramework/PlayerController.h"
#include "BlackDogPlayerController.h"


void UUInventorySlotWidget::SetItemName(const FString& NewItemName)
{
	ItemName = NewItemName;

	// Update the text block if it exists
	if (ItemNameText)
	{
		ItemNameText->SetText(FText::FromString(ItemName));
	}
}

void UUInventorySlotWidget::SetItemImage(UTexture2D* NewImage)
{
    if (ItemImage)  // Make sure the image widget exists
    {
        if (NewImage)
        {
            // Set the image to the provided texture
            ItemImage->SetBrushFromTexture(NewImage);
        }
        else
        {
            // Set the image to a default/empty image when no item exists
            UTexture2D* DefaultEmptyImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/DefaultEmptyImage"));
            if (DefaultEmptyImage)
            {
                ItemImage->SetBrushFromTexture(DefaultEmptyImage);
            }
        }
    }
}

FText UUInventorySlotWidget::GetItemName() const
{
	return FText::FromString(ItemName);
}

void UUInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemNameText)
	{
		ItemNameText->SetText(FText::FromString(ItemName));
	}

	if (ItemImage && CurrentItemImage)
	{
		ItemImage->SetBrushFromTexture(CurrentItemImage);
	}

	if (ItemInfoText)
	{
        ItemInfoText->SetMinDesiredWidth(150.0f); 
        ItemInfoText->SetAutoWrapText(true);
		ItemInfoText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUInventorySlotWidget::SetItem(AItem* NewItem)
{
	ItemInSlot = NewItem;
}

void UUInventorySlotWidget::ResetItemImage()
{
    if (ItemImage)
    {
        FSlateBrush EmptyBrush;
        ItemImage->SetBrush(EmptyBrush); 
    }
}

void UUInventorySlotWidget::SetItemSource(EItemSource NewSource)
{
    ItemSource = NewSource;
}

void UUInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    // Use the GetItem function to get the item for this slot
    AItem* Item = GetItem();

    if (Item && ItemInfoText)
    {
        FString InfoText;

        // Check the item type using EItemType
        switch (Item->GetItemType())
        {
        case EItemType::IT_Weapon:
        {
            // Cast the item to AWeapon since we know it's a weapon
            AWeapon* Weapon = Cast<AWeapon>(Item);
            if (Weapon)
            {
                // Display weapon-specific info like damage and attack speed
                InfoText = FString::Printf(TEXT("Value: %d\nDamage: %.2f\nAtk Speed: %.2f"),
                    Item->ItemValue, Weapon->Damage, Weapon->AttackSpeed);
                UE_LOG(LogTemp, Warning, TEXT("Hovered over weapon: %s, Value: %d, Damage: %.2f, Attack Speed: %.2f"),
                    *Weapon->ItemName, Weapon->ItemValue, Weapon->Damage, Weapon->AttackSpeed);
            }
            break;
        }
        case EItemType::IT_Consumable:
        {
            AHealthPotion* HealthPotion = Cast<AHealthPotion>(Item);
            if (HealthPotion)
            {
                // Display consumable-specific info (if any additional data, you can expand here)
                InfoText = FString::Printf(TEXT("Value: %d (Consumable)\nHealing: %.2f\n"), Item->ItemValue,HealthPotion->HealingValue);
                UE_LOG(LogTemp, Warning, TEXT("Hovered over consumable: %s, Value: %d"), *Item->ItemName, Item->ItemValue);
            }
            break;
        }
        case EItemType::IT_Generic:
        default:
        {
            // Display generic item info
            InfoText = FString::Printf(TEXT("Value: %d"), Item->ItemValue);
            UE_LOG(LogTemp, Warning, TEXT("Hovered over item: %s, Value: %d"), *Item->ItemName, Item->ItemValue);
            break;
        }
        }

        // Show the item info when hovering over the slot
        ItemInfoText->SetVisibility(ESlateVisibility::Visible);
        ItemInfoText->SetText(FText::FromString(InfoText));
    }
    else if (ItemInfoText)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hovered over empty slot"));

        // Hide or reset the text if no item is found
        ItemInfoText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UUInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// Hide the item info text when not hovering
	if (ItemInfoText)
	{
		ItemInfoText->SetVisibility(ESlateVisibility::Hidden);
	}
}

FReply UUInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && ItemInSlot)
    {
        AAdventureCharacter* PlayerCharacter = Cast<AAdventureCharacter>(GetOwningPlayerPawn());
        if (!PlayerCharacter)
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is null!"));
            return FReply::Unhandled();
        }

        ABlackDogPlayerController* PlayerController = Cast<ABlackDogPlayerController>(PlayerCharacter->GetController());
        if (!PlayerController)
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerController is null!"));
            return FReply::Unhandled();
        }

        // Handle item interactions based on the source of the item
        if (ItemSource == EItemSource::Shop)
        {
            // Attempt to purchase the item from the shop
            bool bPurchaseSuccessful = PlayerCharacter->PurchaseItem(ItemInSlot);
            if (bPurchaseSuccessful)
            {
                UE_LOG(LogTemp, Log, TEXT("Purchased item: %s"), *ItemInSlot->ItemName);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Not enough currency to purchase: %s"), *ItemInSlot->ItemName);
            }
            return FReply::Handled();
        }
        else if (ItemSource == EItemSource::PlayerInventory)
        {
            // Check if the shop UI is open to handle selling items
            if (PlayerController->IsShopUIOpen())
            {
                // Attempt to sell the item
                bool bSellSuccessful = PlayerCharacter->SellItem(ItemInSlot);
                if (bSellSuccessful)
                {
                    if (ItemInSlot)
                    {
                        UE_LOG(LogTemp, Log, TEXT("Sold item: %s for %d currency"), *ItemInSlot->ItemName, ItemInSlot->ItemValue);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Item was sold, but the reference is now null."));
                    }
                }
                else
                {
                    if (ItemInSlot)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Failed to sell item: %s"), *ItemInSlot->ItemName);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Failed to sell item: ItemInSlot is null or invalid."));
                    }
                }
                return FReply::Handled();
            }
            else if (ItemInSlot->GetItemType() == EItemType::IT_Weapon)
            {
                // Equip the weapon if the item is from the player's inventory
                AWeapon* WeaponToEquip = Cast<AWeapon>(ItemInSlot);
                if (WeaponToEquip)
                {
                    PlayerCharacter->EquipWeaponFromInventory(WeaponToEquip);
                    return FReply::Handled();
                }
            }
        }
    }

    // Call the base class implementation for other mouse button actions
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

AItem* UUInventorySlotWidget::GetItem()
{
	return ItemInSlot;
}
