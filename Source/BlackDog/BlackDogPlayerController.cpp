// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackDogPlayerController.h"
#include "AdventureCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Shop.h"
#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"


void ABlackDogPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Initialize HUD if the class is set
    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }

    // Initialize the Combined Inventory and Shop Widget
    if (CombinedInventoryShopClass)
    {
        CombinedInventoryShopWidget = CreateWidget<UUserWidget>(this, CombinedInventoryShopClass);
        if (CombinedInventoryShopWidget)
        {
            CombinedInventoryShopWidget->SetVisibility(ESlateVisibility::Hidden);
            CombinedInventoryShopWidget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create CombinedInventoryShopWidget."));
        }
    }

    if (InteractPromptWidgetClass)
    {
        InteractPromptWidget = CreateWidget<UUserWidget>(this, InteractPromptWidgetClass);
        if (InteractPromptWidget)
        {
            InteractPromptWidget->AddToViewport();
            InteractPromptWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // Initialize inventory and shop panels
    InitializeInventory();
    InitializeShop();
}

void ABlackDogPlayerController::InitializeInventory()
{
    if (!CombinedInventoryShopWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("CombinedInventoryShopWidget is null!"));
        return;
    }

    // Get WBP_Inventory within CombinedInventoryShopWidget
    UUserWidget* InventoryWidget = Cast<UUserWidget>(CombinedInventoryShopWidget->GetWidgetFromName(TEXT("WBP_Inventory")));
    if (!InventoryWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("WBP_Inventory not found in CombinedInventoryShopWidget!"));
        return;
    }

    // Locate InventoryPanel within WBP_Inventory using WidgetTree->FindWidget
    UUniformGridPanel* InventoryPanel = InventoryWidget->WidgetTree->FindWidget<UUniformGridPanel>(TEXT("InventoryPanel"));
    if (!InventoryPanel)
    {
        UE_LOG(LogTemp, Error, TEXT("InventoryPanel not found in WBP_Inventory!"));
        return;
    }

    // Populate InventoryPanel as before
    AAdventureCharacter* PlayerCharacter = Cast<AAdventureCharacter>(GetCharacter());
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is null!"));
        return;
    }

    const TArray<AItem*>& PlayerInventory = PlayerCharacter->GetInventory();

    for (int32 Index = 0; Index < MaxInventorySize; ++Index)
    {
        UUInventorySlotWidget* InventorySlot = CreateWidget<UUInventorySlotWidget>(this, InventoryWidgetSlotClass);
        if (InventorySlot)
        {
            if (Index < PlayerInventory.Num() && PlayerInventory[Index] != nullptr)
            {
                AItem* CurrentItem = PlayerInventory[Index];
                InventorySlot->SetItemName(CurrentItem->ItemName);
                InventorySlot->SetItemImage(CurrentItem->ItemImage);
                InventorySlot->SetItem(CurrentItem);
                InventorySlot->SetItemSource(EItemSource::PlayerInventory);
            }
            else
            {
                InventorySlot->SetItemName(TEXT(""));
                InventorySlot->SetItemImage(nullptr);
                InventorySlot->SetItem(nullptr);
            }

            UUniformGridSlot* GridSlot = InventoryPanel->AddChildToUniformGrid(InventorySlot);
            GridSlot->SetColumn(Index % 5);
            GridSlot->SetRow(Index / 5);

            CachedInventorySlots.Add(InventorySlot);
        }
    }
    // Initialize equipped weapon display
    DisplayEquippedWeapon();
}

void ABlackDogPlayerController::InitializeShop()
{
    // Locate ShopInventoryPanel in CombinedInventoryShopWidget
    UUniformGridPanel* ShopInventoryPanel = Cast<UUniformGridPanel>(CombinedInventoryShopWidget->GetWidgetFromName(TEXT("WBP_ShopWidget")));
    if (!ShopInventoryPanel)
    {
        UE_LOG(LogTemp, Error, TEXT("ShopInventoryPanel not found in CombinedInventoryShopWidget!"));
    }
}


void ABlackDogPlayerController::UpdateInventorySlots(bool bDisplayEquippedWeapon)
{
    AAdventureCharacter* PlayerCharacter = Cast<AAdventureCharacter>(GetCharacter());
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is null!"));
        return;
    }

    const TArray<AItem*>& PlayerInventory = PlayerCharacter->GetInventory();

    // Loop through CachedInventorySlots and update each one
    for (int32 Index = 0; Index < CachedInventorySlots.Num(); ++Index)
    {
        UUInventorySlotWidget* InventorySlot = CachedInventorySlots[Index];

        if (Index < PlayerInventory.Num() && PlayerInventory[Index] != nullptr)
        {
            AItem* CurrentItem = PlayerInventory[Index];
            InventorySlot->SetItemName(CurrentItem->ItemName);
            InventorySlot->SetItemImage(CurrentItem->ItemImage);
            InventorySlot->SetItem(CurrentItem);  // Store the item reference in the widget
        }
        else
        {
            // Clear the slot if no item
            InventorySlot->SetItemName(TEXT(""));
            InventorySlot->SetItemImage(nullptr);
            InventorySlot->SetItem(nullptr);
            InventorySlot->ResetItemImage();
        }
    }

    // Update the equipped weapon if the flag is set
    if (bDisplayEquippedWeapon)
    {
        DisplayEquippedWeapon();
    }
}

void ABlackDogPlayerController::PopulateShopWidget(AShop* Shop)
{
    if (!CombinedInventoryShopWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("CombinedInventoryShopWidget is null!"));
        return;
    }

    // Get WBP_ShopWidget within CombinedInventoryShopWidget
    UUserWidget* ShopWidget = Cast<UUserWidget>(CombinedInventoryShopWidget->GetWidgetFromName(TEXT("WBP_ShopWidget")));
    if (!ShopWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("WBP_ShopWidget not found in CombinedInventoryShopWidget!"));
        return;
    }

    // Locate ShopInventoryPanel within WBP_ShopWidget
    UUniformGridPanel* ShopInventoryPanel = Cast<UUniformGridPanel>(ShopWidget->GetWidgetFromName(TEXT("ShopInventoryPanel")));
    if (!ShopInventoryPanel)
    {
        UE_LOG(LogTemp, Error, TEXT("ShopInventoryPanel not found in WBP_ShopWidget!"));
        return;
    }

    ShopInventoryPanel->ClearChildren();

    const TArray<AItem*>& ItemsForSale = Shop->GetItemsForSale();

    for (int32 Index = 0; Index < ItemsForSale.Num(); ++Index)
    {
        AItem* ShopItem = ItemsForSale[Index];
        if (ShopItem)
        {
            UUInventorySlotWidget* ShopSlotWidget = CreateWidget<UUInventorySlotWidget>(this, InventoryWidgetSlotClass);
            if (ShopSlotWidget)
            {
                ShopSlotWidget->SetItemName(ShopItem->ItemName);
                ShopSlotWidget->SetItemImage(ShopItem->ItemImage);
                ShopSlotWidget->SetItem(ShopItem);
                ShopSlotWidget->SetItemSource(EItemSource::Shop);

                UUniformGridSlot* GridSlot = ShopInventoryPanel->AddChildToUniformGrid(ShopSlotWidget);
                GridSlot->SetColumn(Index % 5);
                GridSlot->SetRow(Index / 5);
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("ShopWidget successfully populated with items."));
}

void ABlackDogPlayerController::ShowInteractPrompt()
{
    if (InteractPromptWidget)
    {
        InteractPromptWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void ABlackDogPlayerController::HideInteractPrompt()
{
    if (InteractPromptWidget)
    {
        InteractPromptWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void ABlackDogPlayerController::ShowHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlackDogPlayerController::ToggleInventoryUI()
{
    if (!CombinedInventoryShopWidget) return;

    // Make sure the CombinedInventoryShopWidget is visible
    CombinedInventoryShopWidget->SetVisibility(ESlateVisibility::Visible);

    // Get WBP_Inventory within CombinedInventoryShopWidget
    UUserWidget* InventoryWidget = Cast<UUserWidget>(CombinedInventoryShopWidget->GetWidgetFromName(TEXT("WBP_Inventory")));
    if (!InventoryWidget) return;

    // Get WBP_ShopWidget within CombinedInventoryShopWidget and ensure it's hidden
    UUserWidget* ShopWidget = Cast<UUserWidget>(CombinedInventoryShopWidget->GetWidgetFromName(TEXT("WBP_ShopWidget")));
    if (ShopWidget)
    {
        ShopWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    // Toggle visibility of the InventoryWidget within CombinedInventoryShopWidget
    ESlateVisibility CurrentVisibility = InventoryWidget->GetVisibility();
    if (CurrentVisibility == ESlateVisibility::Hidden)
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Visible);
        SetInputMode(FInputModeGameAndUI());
        bShowMouseCursor = true;
        DisplayEquippedWeapon();
    }
    else
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Hidden);

        // Hide the whole CombinedInventoryShopWidget if Inventory is hidden
        CombinedInventoryShopWidget->SetVisibility(ESlateVisibility::Hidden);

        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
    }
}

void ABlackDogPlayerController::ToggleCombinedInventoryAndShop()
{
    ToggleInventoryAndShopUI(true);
}

void ABlackDogPlayerController::ToggleInventoryAndShopUI(bool bShow)
{
    if (!CombinedInventoryShopWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("CombinedInventoryShopWidget is not initialized!"));
        return;
    }

    // Toggle visibility for the entire CombinedInventoryShopWidget
    CombinedInventoryShopWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

    // Get both WBP_Inventory and WBP_ShopWidget within CombinedInventoryShopWidget
    UUserWidget* InventoryWidget = Cast<UUserWidget>(CombinedInventoryShopWidget->GetWidgetFromName(TEXT("WBP_Inventory")));
    UUserWidget* ShopWidget = Cast<UUserWidget>(CombinedInventoryShopWidget->GetWidgetFromName(TEXT("WBP_ShopWidget")));

    if (InventoryWidget && ShopWidget)
    {
        InventoryWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        ShopWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }

    // Set input mode and cursor visibility
    if (bShow)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(nullptr);
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);
        bShowMouseCursor = true;
        HideInteractPrompt();
    }
    else
    {
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
        AAdventureCharacter* PlayerCharacter = Cast<AAdventureCharacter>(GetCharacter());
        if (PlayerCharacter && PlayerCharacter->GetCurrentShop() != NULL)
        {
            ShowInteractPrompt();
        }
    }
}

void ABlackDogPlayerController::DisplayEquippedWeapon()
{
    AAdventureCharacter* PlayerCharacter = Cast<AAdventureCharacter>(GetCharacter());
    if (!PlayerCharacter || !CombinedInventoryShopWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCharacter or CombinedInventoryShopWidget is null!"));
        return;
    }

    // Get WBP_Inventory within CombinedInventoryShopWidget
    UUserWidget* InventoryWidget = Cast<UUserWidget>(CombinedInventoryShopWidget->GetWidgetFromName(TEXT("WBP_Inventory")));
    if (!InventoryWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("WBP_Inventory not found in CombinedInventoryShopWidget!"));
        return;
    }

    // Locate EquippedWeaponPanel within WBP_Inventory
    UUniformGridPanel* EquippedWeaponPanel = InventoryWidget->WidgetTree->FindWidget<UUniformGridPanel>(TEXT("EquippedWeaponPanel"));
    if (!EquippedWeaponPanel)
    {
        UE_LOG(LogTemp, Error, TEXT("EquippedWeaponPanel not found in WBP_Inventory!"));
        return;
    }

    EquippedWeaponPanel->ClearChildren();

    AWeapon* EquippedWeapon = PlayerCharacter->GetEquippedWeapon();
    if (EquippedWeapon)
    {
        UUInventorySlotWidget* WeaponSlot = CreateWidget<UUInventorySlotWidget>(this, InventoryWidgetSlotClass);
        if (WeaponSlot)
        {
            WeaponSlot->SetItemName(EquippedWeapon->ItemName);
            WeaponSlot->SetItemImage(EquippedWeapon->ItemImage);
            WeaponSlot->SetItem(EquippedWeapon);
            UUniformGridSlot* GridSlot = EquippedWeaponPanel->AddChildToUniformGrid(WeaponSlot);
            GridSlot->SetColumn(0);
            GridSlot->SetRow(0);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No weapon is currently equipped."));
    }
}

void ABlackDogPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind the "I" key to toggle the inventory display
	InputComponent->BindAction("ToggleInventory", IE_Pressed, this, &ABlackDogPlayerController::ToggleInventoryUI);
}

void ABlackDogPlayerController::RefreshInventory()
{
    UpdateInventorySlots(true);
}
