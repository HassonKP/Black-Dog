// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Generic     UMETA(DisplayName = "Generic"),
	IT_Weapon      UMETA(DisplayName = "Weapon"),
	IT_Consumable  UMETA(DisplayName = "Consumable")
};

UCLASS()
class BLACKDOG_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	void CopyFrom(AItem* SourceItem);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UTexture2D* ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 ItemValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	bool bIsEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	bool bIsPickedUp;


	// Virtual function to get the item type, useful for identifying subclasses
	virtual EItemType GetItemType() const { return ItemType; }

	UFUNCTION()
	virtual void OnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
