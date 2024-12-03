// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "BlackDogPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UInventorySlotWidget.h"
#include "AdventureCharacter.h"

// Sets default values
AShop::AShop()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create the SphereComponent to act as the trigger
    ShopTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ShopTrigger"));
    ShopTrigger->SetupAttachment(RootComponent);
    ShopTrigger->SetSphereRadius(200.f);
    ShopTrigger->OnComponentBeginOverlap.AddDynamic(this, &AShop::OnOverlapBegin);
    ShopTrigger->OnComponentEndOverlap.AddDynamic(this, &AShop::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AShop::BeginPlay()
{
    Super::BeginPlay();

    // Only spawn shop items once
    for (TSubclassOf<AItem> ItemClass : ShopInventory)
    {
        if (ItemClass)
        {
            AItem* TempItem = GetWorld()->SpawnActor<AItem>(ItemClass);
            if (TempItem)
            {
                TempItem->SetActorHiddenInGame(true);
                TempItem->SetActorEnableCollision(false);
                ItemsForSale.Add(TempItem);  // Store the item reference
            }
        }
    }

}

// Called every frame
void AShop::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

TArray<AItem*> AShop::GetItemsForSale() const
{
    return ItemsForSale;
}

void AShop::DisplayShopItems()
{
    ABlackDogPlayerController* PlayerController = Cast<ABlackDogPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null!"));
        return;
    }
    PlayerController->PopulateShopWidget(this);
    PlayerController->ToggleInventoryAndShopUI(true);
    PlayerController->SetShopUIOpen(true);
}

void AShop::CloseShopUI()
{
    ABlackDogPlayerController* PlayerController = Cast<ABlackDogPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    if (PlayerController)
    {
        PlayerController->ToggleInventoryAndShopUI(false);
        PlayerController->SetShopUIOpen(false);
    }
}

void AShop::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AAdventureCharacter* PlayerCharacter = Cast<AAdventureCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        PlayerCharacter->SetCurrentShop(this);

        ABlackDogPlayerController* PlayerController = Cast<ABlackDogPlayerController>(PlayerCharacter->GetController());
        if (PlayerController)
        {
            PlayerController->ShowInteractPrompt();
        }
    }
}

void AShop::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AAdventureCharacter* PlayerCharacter = Cast<AAdventureCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        PlayerCharacter->SetCurrentShop(nullptr);

        ABlackDogPlayerController* PlayerController = Cast<ABlackDogPlayerController>(PlayerCharacter->GetController());
        if (PlayerController)
        {
            PlayerController->HideInteractPrompt();
        }
    }
}





