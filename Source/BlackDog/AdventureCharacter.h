// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Item.h"
#include "Weapon.h"
#include "Fists.h"
#include "HealthPotion.h"
#include "AdventureCharacter.generated.h"


class AShop;

UCLASS()
class BLACKDOG_API AAdventureCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdventureCharacter();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Audio")
	TArray<USoundBase*> HitReactions;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GainCurrency(float CurrencyToGain);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AWeapon* GetEquippedWeapon() const;

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrency() const { return Currency; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetCurrentStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxStamina() const { return MaxStamina; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetStopAnimation();

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	bool Get1HandEquipped() const { return b1HandWeaponEquipped; }

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	bool Get2HandEquipped() const { return b2HandWeaponEquipped; }

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void SetWeaponEquipped();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItemToInventory(AItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItemFromInventory(AItem* Item);

	bool CanAfford(int32 Cost);

	void SubtractCurrency(int32 Amount);

	bool PurchaseItem(AItem* ItemToBuy);

	bool SellItem(AItem* ItemToSell);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AItem* GetItemByName(FString ItemName);

	UFUNCTION(BlueprintCallable)
	int32 CountHealthPotions();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PrintInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseHealthPotion();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<AItem*>& GetInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventoryItem(int32 Index, AItem* Item);

	int32 GetAttackCounter() { return AttackCounter; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 MaxInventorySize = 25;

	void EquipWeaponFromClass(TSubclassOf<AWeapon> NewWeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void EquipWeaponFromInventory(AWeapon* WeaponToEquip);

	void UpdateInventoryUI();

	UPROPERTY(BlueprintReadWrite, Category = "Shop")
	bool bIsInteractingWithShop;

	void SetCurrentShop(AShop* Shop) { CurrentShop = Shop; }

	AShop* GetCurrentShop() { return CurrentShop; }

	UFUNCTION(BlueprintCallable, Category = "SoundEffect")
	virtual void PlayRandomReactionSound();

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUp(float AxisValue);
	void LookRight(float AxisValue);
	void StartSprint();
	void StopSprint();
	void Block();
	void StopBlock();
	void Interact();
	
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "True"))
	void Attack();

	UPROPERTY(EditAnywhere)
	float SprintMultiplier = 2.0f;

	UPROPERTY(EditAnywhere)
	float WalkSpeed = 0.5f;

	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "True"))
	bool IsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "True"))
	bool IsUnArmed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "True"))
	bool IsBlocking = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "True"))
	int32 AttackCounter = 0;

	UFUNCTION(BlueprintCallable)
	void ResetAttackState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (AllowPrivateAccess = "True"))
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "True"))
	AWeapon* EquipedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "True"))
	TSubclassOf<AFists> FistWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AHealthPotion> HealthPotionClass;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddDefaultHealthPotions();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "True"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "True"))
	float Health = MaxHealth;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "True"))
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "True"))
	float CurrentStamina = MaxStamina;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "True"))
	float StaminaRegenRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "True"))
	float RegenDelay = 3.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency", meta = (AllowPrivateAccess = "True"))
	float Currency = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency", meta = (AllowPrivateAccess = "True"))
	bool StopAnimation = false;

	
	float TimeSinceLastAction = 0.0f;

	bool b1HandWeaponEquipped;

	bool b2HandWeaponEquipped;

	AShop* CurrentShop;


};
