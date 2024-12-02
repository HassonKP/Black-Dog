// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "AdventureCharacter.h"
#include "EnemyAIController.h"
#include "WaveMode.h"
#include "BlackDogPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

    // Initialize Health values
    Health = MaxHealth;

    bIsAttacking = false;
    Damage = 10;
    AttackCounter = 0;
    AttackSpeed = 1.0f;


    DamageNumberWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageNumberWidget"));
    DamageNumberWidgetComponent->SetupAttachment(RootComponent);
    DamageNumberWidgetComponent->SetRelativeLocation(FVector(0, 0, 50));  // Adjust position as needed
    DamageNumberWidgetComponent->SetDrawSize(FVector2D(100, 100));         // Adjust draw size as needed
    DamageNumberWidgetComponent->SetVisibility(false);
    DamageNumberWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    DamageNumberWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
    DamageNumberWidgetComponent->SetDrawAtDesiredSize(true);
    DamageNumberWidgetComponent->SetTwoSided(true);
    DamageNumberWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AccumulatedDamage = 0.0f;
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    DamageToApply = FMath::Min(Health, DamageToApply);
    ShowDamageNumber(DamageToApply);
    Health -= DamageToApply;
    PlayRandomReactionSound();
    if (IsDead())
    {
        AWaveMode* GameMode = Cast<AWaveMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->PawnKilled(this, EventInstigator);
        }
        DropLoot();
        DetachFromControllerPendingDestroy();
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            // Get the player character from the DamageCauser's owner
            AAdventureCharacter* PlayerCharacter = nullptr;
            if (DamageCauser)
            {
                AActor* OwnerActor = DamageCauser->GetOwner();
                if (OwnerActor)
                {
                    PlayerCharacter = Cast<AAdventureCharacter>(OwnerActor);
                }
            }

            if (PlayerCharacter)
            {
                // Update the blackboard
                BlackboardComp->SetValueAsObject(TEXT("Player"), PlayerCharacter);
                BlackboardComp->SetValueAsVector(TEXT("PlayerLocation"), PlayerCharacter->GetActorLocation());
                BlackboardComp->SetValueAsBool(TEXT("PlayerDetected"), true);
            }
            else
            {
                // Could not find the player character
                UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is null in TakeDamage."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BlackboardComp is null in TakeDamage."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is null in TakeDamage."));
    }

    return DamageToApply;
}

float AEnemyCharacter::GetValue()
{
    return CurrencyValue;
}

bool AEnemyCharacter::IsInRange(FVector PlayerLocation) const
{
    FVector EnemyLocation = GetActorLocation();

    float Distance = FVector::Dist(EnemyLocation, PlayerLocation);
    //UE_LOG(LogTemp, Display, TEXT("Distance: %f"), Distance);
    return Distance <= AttackRange;

}

void AEnemyCharacter::ResetEnemyAttackState()
{
    //UE_LOG(LogTemp, Warning, TEXT("ResetAttackState Called. Setting bIsAttacking to False"));
    bIsAttacking = false;

    // Debug to track which function is calling this
    //UE_LOG(LogTemp, Display, TEXT("bIsAttacking Set to False in: %s"), *FString(__FUNCTION__));
}

void AEnemyCharacter::ShowDamageNumber(float DamageAmount)
{
    AccumulatedDamage += DamageAmount;

    if (DamageNumberWidgetComponent && DamageNumberWidgetComponent->GetUserWidgetObject())
    {
        // Update the damage text in the widget
        FText DamageText = FText::AsNumber(FMath::RoundToInt(AccumulatedDamage));

        UUserWidget* DamageWidget = DamageNumberWidgetComponent->GetUserWidgetObject();
        UFunction* SetDamageTextFunction = DamageWidget->FindFunction(FName("SetDamageText"));

        if (SetDamageTextFunction)
        {
            struct FDamageTextParams { FText Damage; };
            FDamageTextParams Params;
            Params.Damage = DamageText;
            DamageWidget->ProcessEvent(SetDamageTextFunction, &Params);
        }

        // Make sure the widget is visible
        DamageNumberWidgetComponent->SetVisibility(true);

        // Reset the timer to hide the widget after a delay
        GetWorldTimerManager().ClearTimer(DamageNumberHideTimerHandle);
        GetWorldTimerManager().SetTimer(DamageNumberHideTimerHandle, this, &AEnemyCharacter::HideDamageNumber, 1.0f, false);
    }
}

void AEnemyCharacter::DropLoot()
{
    for (const FLootTableEntry& Entry : LootTable)
    {
        float RandomRoll = UKismetMathLibrary::RandomFloatInRange(0.0f, 100.0f);

        if (RandomRoll <= Entry.DropChance && Entry.ItemClass)
        {
            FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
            FRotator SpawnRotation = FRotator::ZeroRotator;
            FActorSpawnParameters SpawnParams;

            // Ensure the item is always spawned even if there's something at the spawn location
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            // Spawn the item
            GetWorld()->SpawnActor<AItem>(Entry.ItemClass, SpawnLocation, SpawnRotation, SpawnParams);
            UE_LOG(LogTemp, Display, TEXT("Dropping LOOT!"));
        }
    }
}

void AEnemyCharacter::HideDamageNumber()
{
    if (DamageNumberWidgetComponent)
    {
        DamageNumberWidgetComponent->SetVisibility(false);
    }

    // Reset the accumulated damage
    AccumulatedDamage = 0.0f;
}

void AEnemyCharacter::PlayRandomReactionSound()
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

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;

    
    FVector StartLocation = GetMesh()->GetSocketLocation(TEXT("Start_R"));
    FVector EndLocation = GetMesh()->GetSocketLocation(TEXT("End_R"));

    if (DamageDisplay)
    {
        DamageNumberWidgetComponent->SetWidgetClass(DamageDisplay);
    }
}

void AEnemyCharacter::Attack()
{
    if (bIsAttacking) { return; }

    //UE_LOG(LogTemp, Warning, TEXT("Attack Function Called. Setting bIsAttacking to True"));
    bIsAttacking = true;
    AttackCounter = (AttackCounter + 1) % 2;
    
    //UE_LOG(LogTemp, Display, TEXT("AttackCounter: %i"), AttackCounter);
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (DamageNumberWidgetComponent && DamageNumberWidgetComponent->IsVisible())
    {
        // Get the player's camera location
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PlayerController && PlayerController->PlayerCameraManager)
        {
            FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
            FVector ComponentLocation = DamageNumberWidgetComponent->GetComponentLocation();

            // Calculate the rotation so that the widget faces the camera
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ComponentLocation, CameraLocation);

            // Optionally, maintain the widget's upright orientation
            LookAtRotation.Pitch = 0.0f;
            LookAtRotation.Roll = 0.0f;

            // Set the rotation of the widget component
            DamageNumberWidgetComponent->SetWorldRotation(LookAtRotation);
        }
    }
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyCharacter::DealDamage()
{
    // Clear previously hit actors
    HitActors.Empty();

    FVector StartLocation;
    FVector EndLocation;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // Determine the socket locations based on AttackCounter
    if (AttackCounter == 0)
    {
        StartLocation = GetMesh()->GetSocketLocation(TEXT("Start_R"));
        EndLocation = GetMesh()->GetSocketLocation(TEXT("End_R"));
    }
    else if (AttackCounter == 1)
    {
        StartLocation = GetMesh()->GetSocketLocation(TEXT("Start_L"));
        EndLocation = GetMesh()->GetSocketLocation(TEXT("End_L"));
    }

    // Perform the line trace
    bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1, Params);

    if (bSuccess)
    {
        AActor* HitActor = Hit.GetActor();
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitActor->GetActorLocation());
        if (HitActor && !HitActors.Contains(HitActor))
        {
            HitActors.Add(HitActor);
            FPointDamageEvent DamageEvent(Damage, Hit, Hit.ImpactNormal, nullptr);
            AController* OwnerController = GetOwnerController();
            HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
        }
    }
}

bool AEnemyCharacter::GetIsAttacking() const
{
    return bIsAttacking;
}

bool AEnemyCharacter::IsDead() const
{
    return Health <= 0;
}

AController* AEnemyCharacter::GetOwnerController()
{
    APawn* Pawn = Cast<APawn>(GetOwner());
    if (Pawn)
    {
        return Pawn->GetController();
    }
    return nullptr;
}


