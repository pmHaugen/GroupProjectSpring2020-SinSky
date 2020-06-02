// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

UENUM(BlueprintType)
enum class EGameDifficultyStatus : uint8
{
	GDS_Easy	UMETA(DisplayName = "Easy"),
	GDS_Medium	UMETA(DisplayName = "Medium"),
	GDS_Hard	UMETA(DisplayName = "Hard"),

	GDS_MAX		UMETA(DisplayName = "DefaultMax")
};
/**
 * 
 */
UCLASS()
class GROUPPROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayTalentTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDTalentTree;


	//Enemy Health Bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WBossHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* BossHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	void ShowPauseMenu();
	void RemovePauseMenu();
	void TogglePauseMenu();
	bool bPauseMenuVisible;
	bool bPause = false;

	bool bEnemyHealthBarVisible;

	void DisplayEnemyHealthBar();
	void HideEnemyHealthBar();

	void DisplayBossHealthBar();
	void HideBossHealthBar();

	FVector EnemyLocation;

	FVector BossLocation;

	bool bVisibleEnemy;
	bool bVisibleBoss;

	//------

	void ToggleSkillMenu();
	void OpenSkillMenu();
	void RemoveSkillMenu();


	//Functions used by Enemy
	void KillCount(float Amount);
	void BossKillCount(float Amount);
	void FoesAliveCount(float Amount);
	void FoesDefeatedCount(float Amount);
	bool bIsLevelCleared();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Kills")
	float Kills;
	float BossKills;
	float FoesAlive;
	float FoesDefeated;
	bool bLevelCleared;

	//Difficulty

	EGameDifficultyStatus GameDifficulty;

	FORCEINLINE void SetGameDifficultyStatus(EGameDifficultyStatus Status) { GameDifficulty = Status; }

	bool bEasy;
	bool bMedium;
	bool bHard;

	bool bGetGameDifficulty();

	//Progress
	float Scaling;

	float LevelsCleared;

	void SetScaling();

	//XP
	float XPoints;
	float XPointsDifference;
	float MaxXp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skillpoints")
	float XpToken;

	float PlayerLevel;

	void AquireXp(float Amount);

	void CalculateXp();

	//--------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	bool bIsVisible;
	
	//Saving
	UFUNCTION(BlueprintCallable)
	void SaveStats();
	UFUNCTION(BlueprintCallable)
	void LoadStats();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
