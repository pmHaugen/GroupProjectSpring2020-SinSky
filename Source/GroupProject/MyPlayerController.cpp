// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"

AMyPlayerController::AMyPlayerController()
{
	Kills = 0;

	BossKills = 0;

	FoesAlive = 0;

	FoesDefeated = 0;

	bLevelCleared = false;

	bEasy = false;
	bMedium = false;
	bHard = false;
}
void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//MyController->bEnableClickEvents = true;
	//MyController->bShowMouseCursor = true;
	//MyController->bEnableMouseOverEvents = true;

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (HUDOverlayTalentTree)
	{
		HUDTalentTree = CreateWidget<UUserWidget>(this, HUDOverlayTalentTree);
	}
	HUDTalentTree->AddToViewport();
	HUDTalentTree->SetVisibility(ESlateVisibility::Hidden);

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}
	if (WBossHealthBar)
	{
		BossHealthBar = CreateWidget<UUserWidget>(this, WBossHealthBar);
		if (BossHealthBar)
		{
			BossHealthBar->AddToViewport();
			BossHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		FVector2D Alignment(0.f, 0.f);
		BossHealthBar->SetAlignmentInViewport(Alignment);
	}
}

void AMyPlayerController::OpenSkillMenu()
{
		UE_LOG(LogTemp, Warning, TEXT("SKILL MENU"));

		if (bIsVisible)
		{
			if (HUDTalentTree)
			{
				HUDTalentTree->SetVisibility(ESlateVisibility::Visible);
			}
		}
		if (!bIsVisible)
		{
			if (HUDTalentTree)
			{
				HUDTalentTree->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	bIsVisible = !bIsVisible;
}

void AMyPlayerController::KillCount(float Amount)
{
	Kills += Amount;
	UE_LOG(LogTemp, Warning, TEXT("The amount of kills are %f"), Kills);

	FoesDefeatedCount(Amount);

	//put if statements here.
}

void AMyPlayerController::BossKillCount(float Amount)
{
	BossKills += Amount;
	UE_LOG(LogTemp, Warning, TEXT("The amount of boss-kills are %f"), BossKills);
}

void AMyPlayerController::FoesAliveCount(float Amount)
{
	FoesAlive += Amount;
	//UE_LOG(LogTemp, Warning, TEXT("Enemies alive: %f"), FoesAlive - FoesDefeated);
}

void AMyPlayerController::FoesDefeatedCount(float Amount)
{
	FoesDefeated += Amount;
	//UE_LOG(LogTemp, Warning, TEXT("Enemies Defeated: %f"), FoesDefeated);
	//UE_LOG(LogTemp, Warning, TEXT("Enemies alive: %f"), FoesAlive - FoesDefeated);
}

bool AMyPlayerController::bIsLevelCleared()
{
	if (FoesAlive == FoesDefeated)
	{
		//UE_LOG(LogTemp, Warning, TEXT("All Enemies Defeated"));
		bLevelCleared = true;
	}
	return bLevelCleared;
}

bool AMyPlayerController::bGetGameDifficulty()
{
	switch (GameDifficulty)
	{
	case EGameDifficultyStatus::GDS_Easy:
		bEasy = true;
		break;
	case EGameDifficultyStatus::GDS_Medium:
		bMedium = true;
		break;
	case EGameDifficultyStatus::GDS_Hard:
		bHard = true;
		break;
	case EGameDifficultyStatus::GDS_MAX:
		break;
	}
	return bEasy, bMedium, bHard;
}

void AMyPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMyPlayerController::HideEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMyPlayerController::DisplayBossHealthBar()
{
	if (BossHealthBar)
	{
		bEnemyHealthBarVisible = true;
		BossHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMyPlayerController::HideBossHealthBar()
{
	if (BossHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		FVector2D PositionInVeiwport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInVeiwport);
		PositionInVeiwport.Y -= 85.f;
		PositionInVeiwport.X -= 60.f;

		FVector2D SizeInVeiwport = FVector2D(200.f, 15.f);

		EnemyHealthBar->SetPositionInViewport(PositionInVeiwport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInVeiwport);
	}
	if (BossHealthBar)
	{
		FVector2D PositionInVeiwport;
		ProjectWorldLocationToScreen(BossLocation, PositionInVeiwport);
		PositionInVeiwport.Y -= 85.f;
		PositionInVeiwport.X -= 60.f;

		FVector2D SizeInVeiwport = FVector2D(200.f, 15.f);

		BossHealthBar->SetPositionInViewport(PositionInVeiwport);
		BossHealthBar->SetDesiredSizeInViewport(SizeInVeiwport);
	}
}