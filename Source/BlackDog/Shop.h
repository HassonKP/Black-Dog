// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "AdventureCharacter.h"
#include "Shop.generated.h"

UCLASS()
class BLACKDOG_API AShop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TArray<TSubclassOf<AItem>> ShopInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shop")
	TArray<AItem*> ItemsForSale;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	USphereComponent* ShopTrigger;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	TArray<AItem*> GetItemsForSale() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void DisplayShopItems();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void CloseShopUI();

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
