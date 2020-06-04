// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameSaver.h"

AMyPlayerController::AMyPlayerController()
{
	//Kills = 0;

	BossKills = 0;

	FoesAlive = 0;

	FoesDefeated = 0;

	LevelsCleared = 0;

	Scaling = 0;
		//Kills * 0.02f + BossKills * 0.5f + LevelsCleared * 1.f;

	bLevelCleared = false;

	bEasy = false;
	bMedium = false;
	bHard = false;

	XPoints = 0;
	XPointsDifference = 0;
	MaxXp = 1;
	//MaxXp = MaxXp = PlayerLevel * 35 + 100;
	XpToken = 100;
	PlayerLevel = 0;
	bIsVisible = false;


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
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
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
	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Kills: %f"), Kills);
}

void AMyPlayerController::ToggleSkillMenu()
{
		//UE_LOG(LogTemp, Warning, TEXT("SKILL MENU"));
		if (bIsVisible)
		{
			RemoveSkillMenu();
		}
		else
		{
			OpenSkillMenu();
		}
}
void AMyPlayerController::OpenSkillMenu()
{
	if (HUDTalentTree)
	{
		bIsVisible = true;
		HUDTalentTree->SetVisibility(ESlateVisibility::Visible);
		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);
		//UE_LOG(LogTemp, Warning, TEXT("Skillpoints playercontroller open: %f"), XpToken);

		bShowMouseCursor = true;
	}
}
void AMyPlayerController::RemoveSkillMenu() 
{
	if (HUDTalentTree)
	{
		bIsVisible = false;
		HUDTalentTree->SetVisibility(ESlateVisibility::Hidden);
		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		//UE_LOG(LogTemp, Warning, TEXT("Skillpoints playercontroller remove: %f"), XpToken);

		bShowMouseCursor = false;
	}
}

void AMyPlayerController::KillCount(float Amount)
{
	Kills += Amount;
	//UE_LOG(LogTemp, Warning, TEXT("The amount of kills are %f"), Kills);

	FoesDefeatedCount(Amount);

	//put if statements here.
}

void AMyPlayerController::BossKillCount(float Amount)
{
	BossKills += Amount;
	//UE_LOG(LogTemp, Warning, TEXT("The amount of boss-kills are %f"), BossKills);
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

void AMyPlayerController::GetGameDifficulty()
{
	switch (GameDifficulty)
	{
	case EGameDifficultyStatus::GDS_Easy:
		UE_LOG(LogTemp, Warning, TEXT("Easy XP"));
		bEasy = true;
		break;
	case EGameDifficultyStatus::GDS_Medium:
		UE_LOG(LogTemp, Warning, TEXT("Meduim XP"));
		bMedium = true;
		break;
	case EGameDifficultyStatus::GDS_Hard:
		UE_LOG(LogTemp, Warning, TEXT("Hard XP"));
		bHard = true;
		break;
	}
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

void AMyPlayerController::AquireXp(float Amount)
{
	XPoints += Amount;

	if (XPoints >= MaxXp)
	{
		CalculateXp();
	}
}

void AMyPlayerController::CalculateXp()
{
	XPointsDifference = XPoints - MaxXp;
	XPoints = 0;
	
	PlayerLevel += 1;

	MaxXp = PlayerLevel * 35 + 100;

	XpToken += 1;

	XPoints += XPointsDifference;
	XPointsDifference = 0;

	//UE_LOG(LogTemp, Warning, TEXT("Calculate XP"));

	if (XPoints >= MaxXp)
	{
		CalculateXp();
	}
}
void AMyPlayerController::SetScaling()
{
	Scaling = (Kills * 0.02) + (BossKills * 0.5) + (LevelsCleared * 1);
	//UE_LOG(LogTemp, Warning, TEXT("Progress Scaling: %f"), Scaling);
	//UE_LOG(LogTemp, Warning, TEXT("Kills Scaling: %f"), Kills);
}

//Pause Menu//
void AMyPlayerController::ShowPauseMenu()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);

		bShowMouseCursor = true;
	}
}

void AMyPlayerController::RemovePauseMenu()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = false;
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);


		bShowMouseCursor = false;
	}
}


void AMyPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		ShowPauseMenu();
	}

}

void AMyPlayerController::SaveStats()
{
	UGameSaver* SaveStats = Cast<UGameSaver>(UGameplayStatics::CreateSaveGameObject(UGameSaver::StaticClass()));

	SaveStats->CharacterStats.Kills = Kills;

	UGameplayStatics::SaveGameToSlot(SaveStats, SaveStats->PlayerName, SaveStats->UserIndex);
	//UE_LOG(LogTemp, Warning, TEXT("Saved kills: %f"), Kills);
}

void AMyPlayerController::LoadStats()
{
	UGameSaver* LoadStats = Cast<UGameSaver>(UGameplayStatics::CreateSaveGameObject(UGameSaver::StaticClass()));
	LoadStats = Cast<UGameSaver>(UGameplayStatics::LoadGameFromSlot(LoadStats->PlayerName, LoadStats->UserIndex));

	Kills = LoadStats->CharacterStats.Kills;
	BossKills = LoadStats->CharacterStats.BossKills;
	LevelsCleared = LoadStats->CharacterStats.LevelsCleared;
	XPoints = LoadStats->CharacterStats.Xpoints;
	PlayerLevel = LoadStats->CharacterStats.PlayerLevel;
	XpToken = LoadStats->CharacterStats.SkillPoints;
//	FoesDefeated = LoadStats->CharacterStats.FoesDefeated;
//	FoesAlive = LoadStats->CharacterStats.FoesAlive;

	MaxXp = PlayerLevel * 35 + 100;
	UE_LOG(LogTemp, Warning, TEXT("Loaded kills: %f"), Kills);

}

void AMyPlayerController::LoadDif()
{
	UGameSaver* LoadStats = Cast<UGameSaver>(UGameplayStatics::CreateSaveGameObject(UGameSaver::StaticClass()));
	LoadStats = Cast<UGameSaver>(UGameplayStatics::LoadGameFromSlot(LoadStats->PlayerName, LoadStats->UserIndex));

	bEasy = LoadStats->CharacterStats.bEasy;
	bMedium = LoadStats->CharacterStats.bMedium;
	bHard = LoadStats->CharacterStats.bHard;
}

void AMyPlayerController::PlayerSpeed(FVector Velocity)
{
	CurrentPlayerSpeed = Velocity.Size();
}

void AMyPlayerController::GetDif()
{
	LoadDif();

	if (bEasy)
	{
		SetGameDifficultyStatus(EGameDifficultyStatus::GDS_Easy);
	}
	if (bMedium)
	{
		SetGameDifficultyStatus(EGameDifficultyStatus::GDS_Medium);
	}
	if (bHard)
	{
		SetGameDifficultyStatus(EGameDifficultyStatus::GDS_Hard);
	}
}

//kill, boss, kills, exp, tokens