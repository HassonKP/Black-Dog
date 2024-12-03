// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "EnemySpawner.generated.h"

UCLASS()
class BLACKDOG_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	UPROPERTY(EditAnywhere, Category = "Enemy")
	TSubclassOf<class AEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	float SpawnInterval;

	UPROPERTY(EditAnywhere, Category = "Spawn Area")
	UBoxComponent* SpawnArea;

	void StartSpawning(int32 NumberOfEnemies);

	void StopSpawning();

	void SetWaveMode(class AWaveMode* InWaveMode);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnEnemy();

	FVector GetRandomPointInSpawnArea() const;

	FTimerHandle SpawnTimerHandle;

	AWaveMode* WaveMode;

	// Variables to track spawning
	int32 MaxSpawnCount;
	int32 SpawnedEnemiesCount;
	bool bShouldSpawn;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
