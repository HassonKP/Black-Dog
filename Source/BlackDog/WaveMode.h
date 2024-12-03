// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlackDogGameMode.h"
#include "Blueprint/UserWidget.h"
#include "WaveMode.generated.h"

/**
 * 
 */

UCLASS()
class BLACKDOG_API AWaveMode : public ABlackDogGameMode
{
	GENERATED_BODY()
public:
	AWaveMode();

	void StartNextWave();

	virtual void PawnKilled(APawn* PawnKilled, AController* KillerController);

	void CheckWaveCompletion();

	void OnEnemySpawned();

	UFUNCTION(BlueprintPure, Category = "Wave Info")
	int32 GetCurrentWave() { return CurrentWaveNumber; }

	int32 GetEnemiesToSpawn() { return EnemiesRemaining; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseIntroWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	FText GetTotalEnemiesKilled();

	UFUNCTION(BlueprintCallable, Category = "UI")
	FText GetTotalWaves();

	UFUNCTION(BlueprintCallable, Category = "Game Management")
	void RestartGame();

	UFUNCTION(BlueprintCallable, Category = "Game Management")
	void CloseGame();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> IntroWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverClass;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Wave Config")
	TArray<class AEnemySpawner*> EnemySpawners;

	int32 CurrentWaveNumber;

	int32 CurrentActiveEnemies;

	int32 EnemiesRemaining;

	int32 TotalEnemiesRemaining;

	UPROPERTY(EditDefaultsOnly, Category = "Wave Config")
	int32 BaseEnemiesPerWave;
	
	FTimerHandle WaveStartTimerHandle;

	void EndGame(bool bIsPlayerWinner);

	void SpawnEnemies();

	void ResetWave();

	UPROPERTY()
	UUserWidget* IntroWidget;

	UPROPERTY()
	UUserWidget* OutroWidget;
	
	int32 TotalEnemiesKilled;
	int32 WavesSurvived;
};
