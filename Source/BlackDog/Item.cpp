// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "AdventureCharacter.h"

// Sets default values
AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = true;

    // Initialize the SphereCollision component
    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    RootComponent = SphereCollision;

    // Set some default values for the collision component
    SphereCollision->InitSphereRadius(50.0f); // Adjust the size as needed
    SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // Set appropriate collision profile
    SphereCollision->SetGenerateOverlapEvents(true); // Enable overlap events

    // Default item properties
    ItemName = "Default Name";
    ItemDescription = "Basic Item";
    ItemValue = 0;
    ItemType = EItemType::IT_Generic;
    bIsPickedUp = false;
}

void AItem::CopyFrom(AItem* SourceItem)
{
    if (SourceItem)
    {
        this->ItemName = SourceItem->ItemName;
        this->ItemDescription = SourceItem->ItemDescription;
        this->ItemType = SourceItem->ItemType;
        this->ItemImage = SourceItem->ItemImage;
        this->ItemValue = SourceItem->ItemValue;
        
    }
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
    Super::BeginPlay();

    if (SphereCollision)
    {
        SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverLapBegin);
    }

}

void AItem::BeginDestroy()
{
    Super::BeginDestroy();

    UE_LOG(LogTemp, Warning, TEXT("Item %s is being destroyed"), *ItemName);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AItem::OnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AAdventureCharacter* PlayerCharacter = Cast<AAdventureCharacter>(OtherActor);
    if (PlayerCharacter && !bIsPickedUp)  // Only trigger if the item hasn't already been picked up
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap detected with: %s"), *PlayerCharacter->GetName());

        // Prevent further overlap handling immediately
        bIsPickedUp = true;

        // Check if this item is the currently equipped weapon
        if (PlayerCharacter->GetEquippedWeapon() == this)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot add the equipped weapon to the inventory."));
            return;
        }

        // Proceed with adding to the inventory
        UE_LOG(LogTemp, Warning, TEXT("Adding item: %s to inventory"), *ItemName);

        // Store a reference to the item
        AItem* ItemToAdd = this;

        // Add the item to the inventory and then schedule destruction for the next tick to prevent immediate crashes
        PlayerCharacter->AddItemToInventory(ItemToAdd);

        // Defer destruction to prevent immediate crash due to references still being used
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
            {
                this->Destroy();
            });
    }
}

