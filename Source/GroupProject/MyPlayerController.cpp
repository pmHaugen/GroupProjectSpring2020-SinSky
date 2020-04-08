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
	HUDOverlay->SetVisibility(ESlateVisibility::Hidden);

	if (HUDOverlayAsset)
	{
		HUDTalentTree = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}
	HUDTalentTree->AddToViewport();
	HUDTalentTree->SetVisibility(ESlateVisibility::Visible);
}