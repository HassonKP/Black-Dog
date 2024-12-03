// Fill out your copyright notice in the Description page of Project Settings.


#include "Fists.h"
#include "AdventureCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/DamageType.h"
#include "Engine/EngineTypes.h"

AFists::AFists()
{
    // Setup for the Left Fist Collision
    LeftFistCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFistCollision"));
    LeftFistCollision->SetCollisionProfileName("NoCollision");

    // Setup for the Right Fist Collision
    RightFistCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistCollision"));
    RightFistCollision->SetCollisionProfileName("NoCollision");

    WeaponType = "Fists";
}

void AFists::BeginPlay()
{
    Super::BeginPlay();

    // Get the owning character and attach the fist colliders to the correct sockets on the skeletal mesh
    AAdventureCharacter* OwningCharacter = Cast<AAdventureCharacter>(GetOwner());
    if (OwningCharacter && OwningCharacter->GetMesh())
    {
        USkeletalMeshComponent* CharacterMesh = OwningCharacter->GetMesh();
        LeftFistCollision->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_L"));
        RightFistCollision->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_R"));

        UE_LOG(LogTemp, Warning, TEXT("Fist collision components successfully attached to sockets."));

        // Draw persistent debug spheres at the initial positions of both fists
        DrawDebugSphere(GetWorld(), LeftFistCollision->GetComponentLocation(), 15.0f, 12, FColor::Blue, true, -1.0f);  // Left Fist
        DrawDebugSphere(GetWorld(), RightFistCollision->GetComponentLocation(), 15.0f, 12, FColor::Green, true, -1.0f);  // Right Fist
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to attach fist collisions: Owning character or mesh is invalid."));
    }
}

void AFists::DealDamage()
{
    // Log attack counter and draw debug spheres on the fists for visual feedback
    AAdventureCharacter* OwnerCharacter = Cast<AAdventureCharacter>(GetOwner());

    if (OwnerCharacter)
    {
        int32 AttackCounter = OwnerCharacter->GetAttackCounter();
        UE_LOG(LogTemp, Warning, TEXT("Current Attack Counter: %d"), AttackCounter);

        // Draw debug spheres to indicate which fist is being used
        if (AttackCounter == 0 || AttackCounter == 1)
        {
            DrawDebugSphere(GetWorld(), RightFistCollision->GetComponentLocation(), 15.0f, 12, FColor::Green, false, 1.0f);  // Right Fist
        }
        else if (AttackCounter == 2)
        {
            DrawDebugSphere(GetWorld(), LeftFistCollision->GetComponentLocation(), 15.0f, 12, FColor::Blue, false, 1.0f);  // Left Fist
        }
    }
}

void AFists::StartNewAttack()
{
    // Simply draw debug spheres on both fists to visualize positions
    DrawDebugSphere(GetWorld(), LeftFistCollision->GetComponentLocation(), 15.0f, 12, FColor::Red, false, 1.0f);
    DrawDebugSphere(GetWorld(), RightFistCollision->GetComponentLocation(), 15.0f, 12, FColor::Red, false, 1.0f);
}

void AFists::ActivateLeftFist()
{
    // Visualize left fist activation
    DrawDebugSphere(GetWorld(), LeftFistCollision->GetComponentLocation(), 15.0f, 12, FColor::Blue, false, 1.0f);
    UE_LOG(LogTemp, Warning, TEXT("Left fist debug sphere activated"));
}

void AFists::ActivateRightFist()
{
    // Visualize right fist activation
    DrawDebugSphere(GetWorld(), RightFistCollision->GetComponentLocation(), 15.0f, 12, FColor::Green, false, 1.0f);
    UE_LOG(LogTemp, Warning, TEXT("Right fist debug sphere activated"));
}