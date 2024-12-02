// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "EnemyCharacter.h"
#include "AdventureCharacter.h"
#include "BlackDogPlayerController.h"
#include "EnemyAIController.h"
#include "WaveMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnInterval = 2.0f;
	SpawnedEnemiesCount = 0;
	MaxSpawnCount = 0;
	bShouldSpawn = false;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetBoxExtent(FVector(500.f, 500.f, 100.f));
	SpawnArea->SetupAttachment(RootComponent);
}

void AEnemySpawner::StartSpawning(int32 NumberOfEnemies)
{
	if (EnemyClass && WaveMode && NumberOfEnemies > 0)
	{
		SpawnedEnemiesCount = 0;
		MaxSpawnCount = NumberOfEnemies;  // Set the number of enemies this spawner should handle

		//UE_LOG(LogTemp, Warning, TEXT("%s starting to spawn %d enemies."), *GetName(), MaxSpawnCount);

		// Start spawning enemies at the specified interval
		bShouldSpawn = true;
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, true);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("EnemyClass or WaveMode is not set in %s, or invalid spawn count!"), *GetName());
	}
}

void AEnemySpawner::StopSpawning()
{
    bShouldSpawn = false;  // Disable spawning
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);  // Clear the timer
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemySpawner::SpawnEnemy()
{

	if (bShouldSpawn && SpawnedEnemiesCount < MaxSpawnCount)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpawnLocation = GetRandomPointInSpawnArea();

		AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation, GetActorRotation(), SpawnParams);
		if (SpawnedEnemy)
		{
			SpawnedEnemiesCount++;
			//UE_LOG(LogTemp, Warning, TEXT("Enemy spawned successfully at %s. Enemies spawned: %d/%d"), *GetName(), SpawnedEnemiesCount, MaxSpawnCount);
			WaveMode->OnEnemySpawned();

			// If we've spawned all assigned enemies, stop spawning
			if (SpawnedEnemiesCount >= MaxSpawnCount)
			{
				StopSpawning();
				//UE_LOG(LogTemp, Warning, TEXT("Finished spawning %d enemies at %s"), MaxSpawnCount, *GetName());
			}
		}
	}
	else
	{
		StopSpawning();
		//UE_LOG(LogTemp, Warning, TEXT("Spawner %s timer stopped, all enemies spawned."), *GetName());
	}
}

void AEnemySpawner::SetWaveMode(AWaveMode* InWaveMode)
{
    WaveMode = InWaveMode;
}

FVector AEnemySpawner::GetRandomPointInSpawnArea() const
{
	if (!SpawnArea)
	{
		return GetActorLocation();
	}

	FVector Origin = SpawnArea->GetComponentLocation();
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();

	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, BoxExtent);
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

