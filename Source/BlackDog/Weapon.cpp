// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

AWeapon::AWeapon() : AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// Attach the mesh to the inherited SphereCollision from AItem (which is the root component)
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);  // Attach the mesh to SphereCollision

	// Create and attach the collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(Mesh);  // Attach collision box to the mesh

	// Set weapon-specific properties
	Damage = 25.f;
	WeaponType = "Sword";
	AttackSpeed = 1.0f;
	ItemType = EItemType::IT_Weapon;

	// Configure the SphereCollision (if necessary, inherited from AItem)
	SphereCollision->SetSphereRadius(50.f);  // Adjust the radius if needed
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AWeapon::CopyFrom(AWeapon* SourceWeapon)
{
	if (SourceWeapon)
	{
		// Call parent class CopyFrom to copy common item properties
		AItem::CopyFrom(SourceWeapon);

		// Copy weapon-specific properties
		this->Damage = SourceWeapon->Damage;
		this->WeaponType = SourceWeapon->WeaponType;
		this->AttackSpeed = SourceWeapon->AttackSpeed;

		this->HitSounds = SourceWeapon->HitSounds;

		this->HitEffect = SourceWeapon->HitEffect;

		// Ensure we copy the skeletal mesh from the source weapon's Mesh component
		if (SourceWeapon->Mesh && SourceWeapon->Mesh->SkeletalMesh)
		{
			this->Mesh->SetSkeletalMesh(SourceWeapon->Mesh->SkeletalMesh); // Copy the mesh from the source
		}

		// Optionally, copy additional visual or physical properties if needed
		this->Mesh->SetRelativeTransform(SourceWeapon->Mesh->GetRelativeTransform());
	}
}

void AWeapon::DealDamage()
{
	USkeletalMeshComponent* WeaponMesh = Cast<USkeletalMeshComponent>(this->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (WeaponMesh)
	{
		FVector StartLocation = WeaponMesh->GetSocketLocation(TEXT("Start"));
		FVector EndLocation = WeaponMesh->GetSocketLocation(TEXT("End"));

		FHitResult Hit;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);


		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1, Params);
		if (bSuccess)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && !HitActors.Contains(HitActor))
			{
				HitActors.Add(HitActor);
				FPointDamageEvent DamageEvent(Damage, Hit, Hit.ImpactPoint, nullptr);
				AController* OwnerController = GetOwnerController();
				HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
				PlayRandomHitSound();
				if (HitEffect)
				{
					FRotator SpawnRotation = Hit.ImpactNormal.Rotation();
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, Hit.Location, SpawnRotation);
				}
			}
		}
	}
	return;
}

AController* AWeapon::GetOwnerController() const
{
	// Get the owning pawn (e.g., character) for this weapon
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return nullptr;
	}
	return OwnerPawn->GetController();
}

void AWeapon::StartNewAttack()
{
	//Clear the actors hit on this swing
	HitActors.Empty();
}

void AWeapon::PlayRandomHitSound()
{
	if (HitSounds.Num() > 0)
	{
		// Generate a random index within the array
		int32 RandomIndex = FMath::RandRange(0, HitSounds.Num() - 1);

		// Get the sound at the random index
		USoundBase* SelectedSound = HitSounds[RandomIndex];

		if (SelectedSound)
		{
			// Play the sound at the hit location
			UGameplayStatics::PlaySoundAtLocation(this, SelectedSound, GetActorLocation());
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

