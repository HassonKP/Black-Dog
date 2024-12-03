// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "EnemyCharacter.generated.h"

USTRUCT(BlueprintType)
struct FLootTableEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    TSubclassOf<class AItem> ItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    float DropChance;
};

UCLASS()
class BLACKDOG_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AEnemyCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Audio")
    TArray<USoundBase*> HitReactions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Audio")
    USoundBase* HitSound;

public:
    void Attack();
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void DealDamage();

    UFUNCTION(BlueprintPure)
    bool GetIsAttacking() const;

    UFUNCTION(BlueprintPure)
    bool IsDead() const;

    AController* GetOwnerController();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    float GetValue();

    bool IsInRange(FVector PlayerLocation) const;

    // Health properties
    UPROPERTY(VisibleAnywhere)
    float Health;

    UPROPERTY(VisibleAnywhere)
    float MaxHealth = 100;

    UPROPERTY(EditAnywhere)
    float CurrencyValue = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "True"))
    bool bIsAttacking;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "True"))
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "True"))
    float AttackSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "True"))
    float AttackRange = 170;

    UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "True"))
    int32 AttackCounter;

    UFUNCTION(BlueprintCallable)
    void ResetEnemyAttackState();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    TArray<FLootTableEntry> LootTable;

    void ShowDamageNumber(float DamageAmount);

    void DropLoot();

    void HideDamageNumber();

    UFUNCTION(BlueprintCallable, Category = "SoundEffect")
    virtual void PlayRandomReactionSound();

private:
    TSet<AActor*> HitActors;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> DamageDisplay;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* DamageNumberWidgetComponent;

    float AccumulatedDamage;

    FTimerHandle DamageNumberHideTimerHandle;
};
