// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

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

	void OpenSkillMenu();

	void KillCount(float Amount);

	void BossKillCount(float Amount);

	void FoesAliveCount(float Amount);

	void FoesDefeatedCount(float Amount);

	bool bIsLevelCleared();

	float Kills;

	float BossKills;

	float FoesAlive;

	float FoesDefeated;

	bool bLevelCleared;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	bool bIsVisible;
	


protected:
	virtual void BeginPlay() override;
};
