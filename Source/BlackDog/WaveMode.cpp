// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveMode.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "AdventureCharacter.h"
#include "EnemyCharacter.h"
#include "BlackDogPlayerController.h"
#include "EnemyAIController.h"
#include "EnemySpawner.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

AWaveMode::AWaveMode()
{
	PlayerControllerClass = ABlackDogPlayerController::StaticClass();
	CurrentWaveNumber = 0;
	BaseEnemiesPerWave = 5;
	TotalEnemiesKilled = 0;
	WavesSurvived = 0;
}

void AWaveMode::StartNextWave()
{
	// Increase the wave number
	CurrentWaveNumber++;

	// Reset wave-related data
	ResetWave();

	// Calculate the number of enemies to spawn based on wave number
	TotalEnemiesRemaining = BaseEnemiesPerWave + (CurrentWaveNumber - 1) * 2;
	EnemiesRemaining = TotalEnemiesRemaining;

	CurrentActiveEnemies = 0;

	// Start spawning process with a small delay for effect
	GetWorldTimerManager().SetTimer(WaveStartTimerHandle, this, &AWaveMode::SpawnEnemies, 3.0f, false);
}

void AWaveMode::PawnKilled(APawn* PawnKilled, AController* KillerController)
{
	Super::PawnKilled(PawnKilled);

	// Check if the killed pawn is a player
	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController)
	{
		EndGame(false);  // If player is killed, end game as a loss
		return;
	}

	// Check if the killed pawn is an enemy
	AEnemyCharacter* KilledEnemy = Cast<AEnemyCharacter>(PawnKilled);
	if (KilledEnemy)
	{
		EnemiesRemaining--;
		CurrentActiveEnemies--;
		TotalEnemiesKilled++;

		// Grant currency to player if killed by a player
		AAdventureCharacter* PlayerCharacter = KillerController ? Cast<AAdventureCharacter>(KillerController->GetPawn()) : nullptr;
		if (PlayerCharacter)
		{
			PlayerCharacter->GainCurrency(KilledEnemy->GetValue());
		}

		// Check if all enemies are defeated for the current wave
		CheckWaveCompletion();
	}
}

void AWaveMode::CheckWaveCompletion()
{
	// If no enemies are left start the next wave
	if (CurrentActiveEnemies <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All enemies defeated. Starting next wave!"));
		WavesSurvived = CurrentWaveNumber;
		StartNextWave();
	}
}

void AWaveMode::CloseIntroWidget()
{
	if (IntroWidget)
	{
		IntroWidget->RemoveFromParent();
		IntroWidget = nullptr;

		// Restore input and game state
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = false;
			UGameplayStatics::SetGamePaused(GetWorld(), false);
		}
	}
}

FText AWaveMode::GetTotalEnemiesKilled()
{
	return FText::FromString(FString::Printf(TEXT("Total Enemies Killed: %d"), TotalEnemiesKilled));
}

FText AWaveMode::GetTotalWaves()
{
	return FText::FromString(FString::Printf(TEXT("Total Waves Survived: %d"), WavesSurvived));
}

void AWaveMode::RestartGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		// Get the current level name
		FName CurrentLevelName = *World->GetName();

		// Restart the level
		UGameplayStatics::OpenLevel(World, CurrentLevelName);
	}
}

void AWaveMode::CloseGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		// Optionally, specify a player controller. nullptr applies to all.
		APlayerController* PlayerController = nullptr;

		// Call QuitGame via KismetSystemLibrary
		UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, false);
	}
}

void AWaveMode::BeginPlay()
{
	Super::BeginPlay();

	if (IntroWidgetClass)
	{
		IntroWidget = CreateWidget<UUserWidget>(GetWorld(), IntroWidgetClass);
		if (IntroWidget)
		{
			IntroWidget->AddToViewport();

			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				FInputModeUIOnly InputModeData;
				InputModeData.SetWidgetToFocus(IntroWidget->TakeWidget());
				PlayerController->SetInputMode(InputModeData);
				PlayerController->bShowMouseCursor = true;
				UGameplayStatics::SetGamePaused(GetWorld(), true);
			}
		}
	}

	for (TActorIterator<AEnemySpawner> SpawnerItr(GetWorld()); SpawnerItr; ++SpawnerItr)
	{
		EnemySpawners.Add(*SpawnerItr);
		(*SpawnerItr)->SetWaveMode(this);  // Use the setter method
	}

	StartNextWave();
}

void AWaveMode::EndGame(bool bIsPlayerWinner)
{
	if (GameOverClass)
	{
		OutroWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverClass);
		if (OutroWidget)
		{
			OutroWidget->AddToViewport();

			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				FInputModeUIOnly InputModeData;
				InputModeData.SetWidgetToFocus(OutroWidget->TakeWidget());
				PlayerController->SetInputMode(InputModeData);
				PlayerController->bShowMouseCursor = true;
			}
		}
	}
}

void AWaveMode::SpawnEnemies()
{
	if (EnemySpawners.Num() == 0 || EnemiesRemaining <= 0) return;

	int32 EnemiesToSpawn = EnemiesRemaining;
	int32 SpawnIndex = 0;

	UE_LOG(LogTemp, Warning, TEXT("Total Enemies to Spawn: %d"), EnemiesToSpawn);

	// Calculate the base number of enemies each spawner should handle
	int32 BaseEnemiesPerSpawner = EnemiesToSpawn / EnemySpawners.Num();
	int32 RemainingEnemies = EnemiesToSpawn % EnemySpawners.Num();

	// Distribute enemies to each spawner
	for (AEnemySpawner* Spawner : EnemySpawners)
	{
		if (EnemiesRemaining <= 0) break;

		// Calculate number of enemies for this spawner
		int32 EnemiesForThisSpawner = BaseEnemiesPerSpawner;

		// Distribute remaining enemies
		if (RemainingEnemies > 0)
		{
			EnemiesForThisSpawner += 1;
			RemainingEnemies--;
		}

		// Instruct the spawner to handle its calculated count
		Spawner->StartSpawning(EnemiesForThisSpawner);
		EnemiesRemaining -= EnemiesForThisSpawner;

		UE_LOG(LogTemp, Warning, TEXT("Spawner %s started spawning %d enemies. Enemies remaining: %d"), *Spawner->GetName(), EnemiesForThisSpawner, EnemiesRemaining);
	}
}

void AWaveMode::ResetWave()
{

}

void AWaveMode::OnEnemySpawned()
{
	CurrentActiveEnemies++;
	UE_LOG(LogTemp, Warning, TEXT("Enemy spawned. Active enemies: %d"), CurrentActiveEnemies);

	// If the total enemies for this wave have been spawned
	if (CurrentActiveEnemies >= TotalEnemiesRemaining)
	{
		UE_LOG(LogTemp, Warning, TEXT("All enemies have been spawned for the wave. Stopping all spawners."));

		// Stop all spawners to ensure no further enemies are spawned
		for (AEnemySpawner* Spawner : EnemySpawners)
		{
			Spawner->StopSpawning();
		}
	}
}
