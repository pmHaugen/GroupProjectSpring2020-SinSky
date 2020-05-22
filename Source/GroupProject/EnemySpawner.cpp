// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "AIController.h"
#include "Boss.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (Actor_1 && Actor_2 && Actor_3 && Actor_4) //Add more actors when nececcary
	{
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
		SpawnArray.Add(Actor_5);

	}
	//GetSpawnerDifficultyStatus();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

FVector AEnemySpawner::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	FVector Origin = SpawningBox->GetComponentLocation();

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	return Point;
}

void AEnemySpawner::SpawnActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World)	
		{
			AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);

			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				Enemy->SpawnDefaultController();

				AAIController* AICont = Cast<AAIController>(Enemy->GetController());
				if (AICont)
				{
					Enemy->AIController = AICont;
				}
			}
			ABoss* Boss = Cast<ABoss>(Actor);
			if (Boss)
			{
				Boss->SpawnDefaultController();

				AAIController* AICont = Cast<AAIController>(Boss->GetController());
				if (AICont)
				{
					Boss->AIController = AICont;
				}
			}
		}
	}
}

TSubclassOf<AActor> AEnemySpawner::GetSpawnActor()
{
	if (SpawnArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);

		return SpawnArray[Selection];
	}
	else
	{
		return nullptr;
	}
}

/**void AEnemySpawner::GetSpawnerDifficultyStatus()
{

	/**
	******* NEEDED FIX
	*/
	//AEnemy& Enemy = Cast<AEnemy>(Enemy);
	//AEnemy* Difficulty = Cast<AEnemy::SetEnemyDifficultyStatus>(Enemy);
/**
	switch (SpawnerDifficultyStatus)
	{
	case ESpawnerDifficultyStatus::SDS_Easy:
		//AEnemy::SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Easy);
		//Enemy.SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Easy);
		break;
	case ESpawnerDifficultyStatus::SDS_Medium:
		//Enemy.SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Medium);
		break;
	case ESpawnerDifficultyStatus::SDS_Hard:
		//Enemy.SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Hard);
		break;
	}
}*/
