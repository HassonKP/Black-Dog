// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventureCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "WaveMode.h"
#include "Shop.h"
#include "BlackDogPlayerController.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AAdventureCharacter::AAdventureCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = true;

	// Initialize health and stamina properties
	MaxHealth = 100.0f;
	Health = MaxHealth;

	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;

	StaminaRegenRate = 10.0f;
	RegenDelay = 3.0f;

	// Initialize state variables
	IsAttacking = false;
	IsBlocking = false;
	bIsSprinting = false;
	IsUnArmed = true;

	// Initialize Currency
	Currency = 0.0f;

	// Initialize pointers
	EquipedWeapon = nullptr;
	WeaponClass = nullptr;

	// Initialize the delay timer
	TimeSinceLastAction = 0.0f;

	// Initialize inventory arrays
	Inventory = TArray<AItem*>();
	

}

float AAdventureCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	TimeSinceLastAction = 0.0f;
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);

	if (IsBlocking)
	{
		DamageToApply = DamageToApply / 2;
	}

	Health -= DamageToApply;
	//Play a grunt sound? Probably call a function to randomly decide to play the sound.
	PlayRandomReactionSound();
	if (IsDead())
	{
		AWaveMode* GameMode = Cast<AWaveMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->PawnKilled(this, EventInstigator);
		}
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	return Health;
}

// Called when the game starts or when spawned
void AAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize Health and Stamina values
	Health = MaxHealth;
	CurrentStamina = MaxStamina;

	// Equip the weapon from the WeaponClass
	EquipWeaponFromClass(WeaponClass);

	SetWeaponEquipped();

	// Add default health potions to the inventory
	AddDefaultHealthPotions();
}

// Called every frame
void AAdventureCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EquipedWeapon->WeaponType == "Fists")
	{
		IsUnArmed = true;
	}
	else
	{
		IsUnArmed = false;
		
	}
	if (!IsAttacking && CurrentStamina < MaxStamina)
	{
		TimeSinceLastAction += DeltaTime;

		if (TimeSinceLastAction >= RegenDelay)
		{
			CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate * DeltaTime, 0.0f, MaxStamina);
		}
	}

}

// Called to bind functionality to input
void AAdventureCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AAdventureCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AAdventureCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AAdventureCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AAdventureCharacter::LookRight);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AAdventureCharacter::StartSprint);
	PlayerInputComponent->BindAction(TEXT("StopSprint"), EInputEvent::IE_Released, this, &AAdventureCharacter::StopSprint);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AAdventureCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Block"), EInputEvent::IE_Pressed, this, &AAdventureCharacter::Block);
	PlayerInputComponent->BindAction(TEXT("Block"), EInputEvent::IE_Released, this, &AAdventureCharacter::StopBlock);
	PlayerInputComponent->BindAction(TEXT("UseHealthPotion"), IE_Pressed, this, & AAdventureCharacter::UseHealthPotion);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AAdventureCharacter::Interact);

}

void AAdventureCharacter::SetStopAnimation()
{
	StopAnimation = true;
	return;
}

void AAdventureCharacter::SetWeaponEquipped()
{
	if (!EquipedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("EquippedWeapon is null in SetWeaponEquipped!"));
		return;
	}

	FString WeaponType = EquipedWeapon->WeaponType;

	if (WeaponType == "2Hand")
	{
		b1HandWeaponEquipped = false;
		b2HandWeaponEquipped = true;
	}
	else if (WeaponType == "1Hand")  
	{
		b1HandWeaponEquipped = true;
		b2HandWeaponEquipped = false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon Equipped Type isnt working!"));
		b1HandWeaponEquipped = false; 
		b2HandWeaponEquipped = false;
	}

}

void AAdventureCharacter::EquipWeaponFromClass(TSubclassOf<AWeapon> NewWeaponClass)
{
	if (NewWeaponClass)
	{
		// Unequip the current weapon (if there is one)
		if (EquipedWeapon)
		{
			// Add the currently equipped weapon back to the inventory
			AddItemToInventory(EquipedWeapon);
			EquipedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); // Detach it from the character
		}

		// Spawn the new weapon
		AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeaponClass);
		if (NewWeapon)
		{
			// Attach the new weapon to the character's hand (or any socket you prefer)
			NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_R"));
			NewWeapon->SetOwner(this);

			// Set it as the equipped weapon
			EquipedWeapon = NewWeapon;

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn weapon from WeaponClass."));
		}
	}
}

void AAdventureCharacter::UpdateInventoryUI()
{
	ABlackDogPlayerController* BlackDogPC = Cast<ABlackDogPlayerController>(GetController());
	if (BlackDogPC)
	{
		BlackDogPC->RefreshInventory();
	}
}

void AAdventureCharacter::AddItemToInventory(AItem* Item)
{
	if (Item)
	{
		// Prevent adding an equipped weapon
		if (Item == GetEquippedWeapon())
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot add equipped weapon: %s to inventory."), *Item->ItemName);
			return;
		}

		if (Inventory.Num() < MaxInventorySize)
		{
			AItem* NewItem = nullptr;

			// Handle item duplication based on type
			switch (Item->GetItemType())
			{
			case EItemType::IT_Weapon:
			{
				AWeapon* WeaponToCopy = Cast<AWeapon>(Item);
				if (WeaponToCopy)
				{
					// Create a new weapon instance
					NewItem = GetWorld()->SpawnActor<AWeapon>(AWeapon::StaticClass());
					if (NewItem)
					{
						Cast<AWeapon>(NewItem)->CopyFrom(WeaponToCopy);
					}
				}
				break;
			}

			case EItemType::IT_Consumable:
			{
				AHealthPotion* PotionToCopy = Cast<AHealthPotion>(Item);
				if (PotionToCopy)
				{
					// Create a new health potion instance
					NewItem = GetWorld()->SpawnActor<AHealthPotion>(AHealthPotion::StaticClass());
					if (NewItem)
					{
						Cast<AHealthPotion>(NewItem)->CopyFrom(PotionToCopy);
					}
				}
				break;
			}

			default:
			{
				// Generic item duplication
				NewItem = GetWorld()->SpawnActor<AItem>(AItem::StaticClass());
				if (NewItem)
				{
					NewItem->CopyFrom(Item);
				}
				break;
			}
			}

			if (NewItem)
			{
				Inventory.Add(NewItem);  // Add the new item to the inventory

				// Hide the item in the world
				NewItem->SetActorHiddenInGame(true);
				NewItem->SetActorEnableCollision(false);
				PrintInventory();  // Print the current state of the inventory
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Inventory full! Cannot add item: %s"), *Item->ItemName);
		}
	}
	UpdateInventoryUI();
}

void AAdventureCharacter::RemoveItemFromInventory(AItem* Item)
{
	if (Item)
	{
		// Remove from the main inventory
		Inventory.Remove(Item);


		// Handle type-specific removal logic
		switch (Item->GetItemType())
		{
		case EItemType::IT_Weapon:
			UE_LOG(LogTemp, Warning, TEXT("Removed weapon: %s from inventory."), *Item->ItemName);
			break;

		case EItemType::IT_Consumable:
			UE_LOG(LogTemp, Warning, TEXT("Removed consumable: %s from inventory."), *Item->ItemName);
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Removed generic item: %s from inventory."), *Item->ItemName);
			break;
		}

		PrintInventory();
		UpdateInventoryUI();
	}
}

bool AAdventureCharacter::CanAfford(int32 Cost)
{
	return Currency >= Cost;
}

void AAdventureCharacter::SubtractCurrency(int32 Amount)
{
	Currency -= Amount;
}

bool AAdventureCharacter::PurchaseItem(AItem* ItemToBuy)
{
	if (!ItemToBuy) return false;

	int32 ItemCost = ItemToBuy->ItemValue;  // Assuming GetItemValue() returns the item's price

	if (CanAfford(ItemCost))
	{
		SubtractCurrency(ItemCost);
		AddItemToInventory(ItemToBuy);  // Add the item to the player's inventory
		return true;
	}
	else
	{
		return false;
	}
}

bool AAdventureCharacter::SellItem(AItem* ItemToSell)
{
	if (!ItemToSell)
	{
		UE_LOG(LogTemp, Error, TEXT("SellItem failed: ItemToSell is null."));
		return false;
	}

	if (!Inventory.Contains(ItemToSell))
	{
		UE_LOG(LogTemp, Warning, TEXT("SellItem failed: Item %s not found in inventory."), *ItemToSell->ItemName);
		return false;
	}

	int32 ItemValue = ItemToSell->ItemValue;

	GainCurrency(ItemValue);

	RemoveItemFromInventory(ItemToSell);

	return true; // Return true to indicate the item was successfully sold
}

AItem* AAdventureCharacter::GetItemByName(FString ItemName)
{
	for (AItem* Item : Inventory)
	{
		if (Item && Item->ItemName == ItemName)
		{
			return Item;
		}
	}
	return nullptr;
}

int32 AAdventureCharacter::CountHealthPotions()
{
	int32 count = 0;
	for(AItem* Item : Inventory)
	{
		if (Item)
		{
			switch (Item->GetItemType())
			{
			case EItemType::IT_Consumable:
				if (Item->ItemName == "Health Potion")
				{
					++count;
				}
			default:
				break;
			}
		}
	}
	return count;
}

void AAdventureCharacter::PrintInventory() const
{

	for (AItem* Item : Inventory)
	{
		if (Item)
		{
			// Get the item type and print accordingly
			switch (Item->GetItemType())
			{
			case EItemType::IT_Weapon:
				UE_LOG(LogTemp, Warning, TEXT("- %s (Weapon) - Attack Speed: %.2f"), *Item->ItemName, Cast<AWeapon>(Item)->AttackSpeed);
				break;

			case EItemType::IT_Consumable:
				UE_LOG(LogTemp, Warning, TEXT("- %s (Consumable Item)"), *Item->ItemName);
				break;

			default:
				UE_LOG(LogTemp, Warning, TEXT("- %s (Generic Item)"), *Item->ItemName);
				break;
			}
		}
	}
}

void AAdventureCharacter::UseHealthPotion()
{
	AItem* FoundItem = GetItemByName("Health Potion");
	if (FoundItem)
	{
		AHealthPotion* HealthPotion = Cast<AHealthPotion>(FoundItem);
		if (HealthPotion)
		{
			// Restore health and remove the potion from inventory
			Health = FMath::Clamp(Health + HealthPotion->HealingValue, 0.0f, MaxHealth);
			RemoveItemFromInventory(HealthPotion);
			HealthPotion->Destroy();

			// Update the inventory UI after the potion is consumed
			ABlackDogPlayerController* PlayerController = Cast<ABlackDogPlayerController>(GetController());
			if (PlayerController)
			{
				PlayerController->RefreshInventory();
			}
		}
	}
}

const TArray<AItem*>& AAdventureCharacter::GetInventory() const
{
	return Inventory;
}

void AAdventureCharacter::SetInventoryItem(int32 Index, AItem* Item)
{
	if (Inventory.IsValidIndex(Index))
	{
		Inventory[Index] = Item;
	}
}

void AAdventureCharacter::PlayRandomReactionSound()
{
	if (HitReactions.Num() > 0)
	{
		// Generate a random index within the array bounds
		int32 RandomIndex = FMath::RandRange(0, HitReactions.Num() - 1);

		// Get the sound at the random index
		USoundBase* SelectedSound = HitReactions[RandomIndex];

		int32 PlaySoundInt = FMath::RandRange(0, 2);

		if (SelectedSound)
		{
			if (PlaySoundInt == 1)
			{
				// Play the sound at the hit location
				UGameplayStatics::PlaySoundAtLocation(this, SelectedSound, GetActorLocation());
				UE_LOG(LogTemp, Log, TEXT("Playing Hit Sound: %s"), *SelectedSound->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SelectedSound is null in PlayRandomHitSound()"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HitSounds array is empty in PlayRandomHitSound()"));
	}
}

void AAdventureCharacter::MoveForward(float AxisValue)
{
	if (bIsSprinting)
	{
		AddMovementInput(GetActorForwardVector() * AxisValue * SprintMultiplier);
	}
	else
	{
		AddMovementInput(GetActorForwardVector() * AxisValue);
	}
}
void AAdventureCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}
void AAdventureCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}
void AAdventureCharacter::LookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void AAdventureCharacter::StartSprint()
{
	bIsSprinting = true;
}

void AAdventureCharacter::StopSprint()
{
	bIsSprinting = false;
}

void AAdventureCharacter::Block()
{
	if (EquipedWeapon->WeaponType == "1Hand")
	{
		IsBlocking = true;
		IsAttacking = false;
	}
}

void AAdventureCharacter::StopBlock()
{
	IsBlocking = false;
	StopAnimation = false;
}

void AAdventureCharacter::Interact()
{
	// Check if the player is near a shop (assuming SetCurrentShop sets this variable)
	if (CurrentShop)
	{
		// Check if the shop UI is already open
		if (bIsInteractingWithShop)
		{
			// If the shop is open, close it
			CurrentShop->CloseShopUI();  // Assuming CloseShopUI() is implemented in AShop
			bIsInteractingWithShop = false;
		}
		else
		{
			// If the shop is not open, open it
			CurrentShop->DisplayShopItems();  // Assuming DisplayShopItems() is implemented in AShop
			bIsInteractingWithShop = true;
		}
	}
	
}


void AAdventureCharacter::GainCurrency(float CurrencyToGain)
{
	Currency += CurrencyToGain;
}

void AAdventureCharacter::Attack()
{
	if (IsAttacking || CurrentStamina < 10) return; 

	CurrentStamina -= 10;
	TimeSinceLastAction = 0.0f;

	if (EquipedWeapon)
	{
		EquipedWeapon->StartNewAttack();
		EquipedWeapon->DealDamage();
	}
	IsAttacking = true;
	AttackCounter = (AttackCounter + 1) % 3;

}

void AAdventureCharacter::ResetAttackState()
{
	IsAttacking = false;
}

void AAdventureCharacter::AddDefaultHealthPotions()
{
	// Check if the HealthPotionClass is valid before spawning
	if (HealthPotionClass)
	{
		for (int32 i = 0; i < 2; i++)  // Spawn 2 health potions by default
		{
			AHealthPotion* NewPotion = GetWorld()->SpawnActor<AHealthPotion>(HealthPotionClass);
			if (NewPotion)
			{
				AddItemToInventory(NewPotion);  // Add the potion to the inventory
			}
		}
	}
	
}

void AAdventureCharacter::EquipWeaponFromInventory(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		// Unequip current weapon (if there is one)
		if (EquipedWeapon)
		{
			// Detach the current weapon from the character's mesh
			EquipedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			// Clear the equipped weapon before adding it back to the inventory to avoid self-reference issues
			AWeapon* PreviousWeapon = EquipedWeapon;
			EquipedWeapon = nullptr;  // Clear the reference

			// Add the previously equipped weapon back to the inventory
			if (Inventory.Num() < MaxInventorySize)
			{
				AddItemToInventory(PreviousWeapon);

				// Destroy the world actor representation to prevent it from floating in the air
				PreviousWeapon->Destroy();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Inventory is full, could not add weapon: %s"), *PreviousWeapon->ItemName);
			}
		}

		// Equip the new weapon
		EquipedWeapon = WeaponToEquip;

		// Remove it from the main inventory
		RemoveItemFromInventory(WeaponToEquip);

		// Attach the new weapon to the character's hand socket
		EquipedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_R"));
		EquipedWeapon->SetActorHiddenInGame(false);

		EquipedWeapon->SetOwner(this);

		// Set weapon equipped status
		SetWeaponEquipped();
	}
}

AWeapon* AAdventureCharacter::GetEquippedWeapon() const
{
	return EquipedWeapon;
}

bool AAdventureCharacter::IsDead() const
{
	return Health <= 0;
}
