// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
	HUDTalentTree->SetVisibility(ESlateVisibility::Visible);
}

void AMyPlayerController::OpenSkillMenu()
{



	if (HUDOverlayTalentTree)
	{
		UE_LOG(LogTemp, Warning, TEXT("SKILL MENU"));
	//	HUDTalentTree->SetVisibility(ESlateVisibility::Visible);
	}

	//bIsVisible = !bIsVisible;
}