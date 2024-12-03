// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item.h"
#include "Components/EditableTextBox.h"
#include "UInventorySlotWidget.generated.h"


UENUM(BlueprintType)
enum class EItemSource : uint8
{
	PlayerInventory UMETA(DisplayName = "Player Inventory"),
	Shop UMETA(DisplayName = "Shop"),
	Loot UMETA(DisplayName = "Loot") // Add additional sources as needed
};

UCLASS()
class BLACKDOG_API UUInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Sets the item name to be displayed */
	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void SetItemName(const FString& NewItemName);

	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void SetItemImage(UTexture2D* NewImage);

	/** Returns the item name for binding */
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
	FText GetItemName() const;

	AItem* GetItem();

	void SetItem(AItem* NewItem);

	/** Sets the index of the slot */
	UPROPERTY(BlueprintReadWrite, Category = "Inventory Slot")
	int32 SlotIndex;

	void ResetItemImage();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetItemSource(EItemSource NewSource);

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	// Override the NativeConstruct function to bind data during creation.
	virtual void NativeConstruct() override;

	/** Reference to the TextBlock for displaying the item name */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemInfoText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemSource ItemSource;

private:
	/** Internal variable to store the item name */
	AItem* ItemInSlot;
	FString ItemName;
	UTexture2D* CurrentItemImage;

};
