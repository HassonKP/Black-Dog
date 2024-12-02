// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UInventorySlotWidget.h"
#include "BlackDogPlayerController.generated.h"

UCLASS()
class BLACKDOG_API ABlackDogPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void ShowHUD();

	void ToggleInventoryUI();

	void ToggleCombinedInventoryAndShop();

	void ToggleInventoryAndShopUI(bool bShow);

	void DisplayEquippedWeapon();

	virtual void SetupInputComponent() override;

	void RefreshInventory();

	void UpdateInventorySlots(bool bDisplayEquippedWeapon);

	void PopulateShopWidget(class AShop* Shop);

	bool IsShopUIOpen() const { return bIsShopUIOpen; }
	void SetShopUIOpen(bool bOpen) { bIsShopUIOpen = bOpen; }

	UFUNCTION()
	void ShowInteractPrompt();

	UFUNCTION()
	void HideInteractPrompt();

protected:
	virtual void BeginPlay() override;

	void InitializeInventory();

	void InitializeShop();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> CombinedInventoryShopClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> InventoryWidgetSlotClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InteractPromptWidgetClass;

	UPROPERTY()
	UUserWidget* InteractPromptWidget;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	UUserWidget* HUDWidget;

	UPROPERTY()
	UUserWidget* CombinedInventoryShopWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 MaxInventorySize = 25;

	TArray<UUInventorySlotWidget*> CachedInventorySlots;

	bool bIsShopUIOpen = false;
};
