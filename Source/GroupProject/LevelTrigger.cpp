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
	NextLevelName = "StartLevel";

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
				//Main->NextLevel(NextLevelName);

				//Preferred way
				/**if (Level_1 && Level_2 && Level_3 && Level_4 && Level_5)
				{
					LevelArray.Add(Level_1);
					LevelArray.Add(Level_2);
					LevelArray.Add(Level_3);
					LevelArray.Add(Level_4);
					LevelArray.Add(Level_5);

					//FString LevelName(GetLevel);

					LevelName = LevelArray[1];

					//FName NewLevelName(*LevelName);

					Main->NextLevel(LevelName);
				}


				//Working way
				/**int32 Level = FMath::RandRange(1, 2);

				if (Level ==1)
				{
					UE_LOG(LogTemp, Warning, TEXT("Random 1"));
					NextLevelName = "BossTestingLevel";
				}
				if (Level == 2)
				{
					UE_LOG(LogTemp, Warning, TEXT("Random 2"));
					NextLevelName = "StartLevel";
				}*/

				LevelSelection();

				Main->NextLevel(NextLevelName);

			}
			else
			{
				//Notify the player that there are still enemies left
				UE_LOG(LogTemp, Warning, TEXT("Still Enemies left"));
			}
		}
	}
}

/**TAssetPtr<UWorld> ALevelTrigger::GetLevel()
{
	if (LevelArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, LevelArray.Num() - 1);

		return LevelArray[Selection];
	}
	else
	{
		return nullptr;
	}
}*/

void ALevelTrigger::LevelSelection()
{
	int32 Level;

	UWorld*World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);

		if (CurrentLevel == "StartLevel")
		{
			Level = FMath::RandRange(1, 5);
			switch (Level)
			{
			case 1: 
				NextLevelName = "BossTestingLevel";
				break;
			case 2:
				NextLevelName = "AirLevel";
				break;
			case 3:
				NextLevelName = "FireLevel";
				break;
			case 4:
				NextLevelName = "EarthLevel";
				break;
			case 5:
				NextLevelName = "WaterLevel";
				break;
			}
		}
	}
}


