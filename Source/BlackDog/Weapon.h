// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"


UCLASS()
class BLACKDOG_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();
	void CopyFrom(AWeapon* SourceWeapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void DealDamage();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetAttackSpeed() const { return AttackSpeed; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	FString WeaponType; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float AttackSpeed;

	AController* GetOwnerController() const;

	virtual void StartNewAttack();

	virtual EItemType GetItemType() const override { return EItemType::IT_Weapon; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void PlayRandomHitSound();



protected:
	TSet<AActor*> HitActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Audio")
	TArray<USoundBase*> HitSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effect")
	UParticleSystem* HitEffect;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;

	
};
