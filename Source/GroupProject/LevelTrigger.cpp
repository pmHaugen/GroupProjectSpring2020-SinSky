// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "MainCharacter.h"
#include "MyPlayerController.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ALevelTrigger::ALevelTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerPoint"));
	RootComponent = TriggerPoint;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(GetRootComponent());

	//To be changed!
	//NextLevelName = "StartLevel";

	bCanSwitchLevel = false;

}

// Called when the game starts or when spawned
void ALevelTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerPoint->OnComponentBeginOverlap.AddDynamic(this, &ALevelTrigger::OnOverlapBegin);
	
}

// Called every frame
void ALevelTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		//If main overlaps and the level is declared cleared from Playercontroller the map will change.
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			AMyPlayerController* Clear = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			Clear->bIsLevelCleared();
			if (Clear->bLevelCleared)
			{
				UE_LOG(LogTemp, Warning, TEXT("LevelCleared!"));

				LevelSelection();
				if (bCanSwitchLevel)
				{
					UE_LOG(LogTemp, Warning, TEXT("Switching level...."));
					Main->NextLevel(NextLevel);
				}

			}
			else
			{
				//Notify the player that there are still enemies left
				UE_LOG(LogTemp, Warning, TEXT("Still Enemies left"));
			}
		}
	}
}

void ALevelTrigger::LevelSelection()
{
	int32 Level;

	UWorld*World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);

		Level = FMath::RandRange(1, 5);
		switch (Level)
		{
		case 1:
			NextLevelName = "UEDPIE_0_FireLevel";
			NextLevel = "FireLevel";
			break;
		case 2:
			NextLevelName = "UEDPIE_0_WaterLevel";
			NextLevel = "WaterLevel";
			break;
		case 3:
			NextLevelName = "UEDPIE_0_EarthLevel";
			NextLevel = "EarthLevel";
			break;
		case 4:
			NextLevelName = "UEDPIE_0_AirLevel";
			NextLevel = "AirLevel";
			break;
		case 5:
			NextLevelName = "UEDPIE_0_StartLevel";
			NextLevel = "StartLevel";
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("LevelSwitch %i"), Level);
		UE_LOG(LogTemp, Warning, TEXT("Current Level %s"), *CurrentLevel);

		if (CurrentLevelName != NextLevelName)
		{
			bCanSwitchLevel = true;
		}
		else
		{
			LevelSelection();
		}
	}
}


