// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AiController.h"
#include "MainCharacter.h"
#include "Fireball.h"
#include "WaterWave.h"
#include "MyPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(1500.f);


	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(90.f);

	bOverLappingCombatSphere= false;

	//Reduced Health for testing.
	MaxHealth = 150;
	Health = 30;

	HitDamage = 150;
	DamageGiven = 0;

	bFireStatus = false;
	bWaterStatus = false;
	bEarthStatus = false;
	bAirStatus = false;

	HighResistance = 0.35;
	LowResistance = 0.7;
	NoResistance = 1;

	bEasy = false;
	bMedium = false;
	bHard = false;


}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AMyPlayerController* Alive = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	Alive->FoesAliveCount(1.f);

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	GetEnemyElementalStatus();
	GetEnemyDifficultyStatus();

	UE_LOG(LogTemp, Warning, TEXT("Difficulty is Easy: %s"), (bEasy ? TEXT("TRUE"):TEXT("FALSE")));
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::TakeFireDamage(float Damage)
{
	switch (EnemyElementalStatus)
	{
	case EEnemyElementalStatus::EES_Fire:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Water:
		DamageGiven = Damage * HighResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Earth:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Air:
		DamageGiven = Damage * NoResistance;
		DamageTaken(DamageGiven);
		break;
	}
}

void AEnemy::TakeWaterDamage(float Damage)
{
	switch (EnemyElementalStatus)
	{
	case EEnemyElementalStatus::EES_Fire:
		DamageGiven = Damage * NoResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Water:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Earth:
		DamageGiven = Damage * HighResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Air:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	}
}

void AEnemy::TakeEarthDamage(float Damage)
{
	switch (EnemyElementalStatus)
	{
	case EEnemyElementalStatus::EES_Fire:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Water:
		DamageGiven = Damage * NoResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Earth:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Air:
		DamageGiven = Damage * HighResistance;
		DamageTaken(DamageGiven);
		break;
	}
}

void AEnemy::TakeAirDamage(float Damage)
{
	switch (EnemyElementalStatus)
	{
	case EEnemyElementalStatus::EES_Fire:
		DamageGiven = Damage * HighResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Water:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Earth:
		DamageGiven = Damage * NoResistance;
		DamageTaken(DamageGiven);
		break;
	case EEnemyElementalStatus::EES_Air:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	}
}



void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MoveToTarget(MainCharacter);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{	
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);

			if (bFireStatus)
			{
				UE_LOG(LogTemp, Warning, TEXT("Fire Damage!"));
				MainCharacter->FireDamage(HitDamage);
			}

			if (bWaterStatus)
			{
				UE_LOG(LogTemp, Warning, TEXT("Water Damage!"));
				MainCharacter->WaterDamage(HitDamage);
			}

			if (bEarthStatus)
			{ 
				UE_LOG(LogTemp, Warning, TEXT("Earth Damage!"));
				MainCharacter->EarthDamage(HitDamage);
			}
			
			if (bAirStatus)
			{
				UE_LOG(LogTemp, Warning, TEXT("Air Damage!"));
				MainCharacter->AirDamage(HitDamage);
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
			MoveToTarget(MainCharacter);
		}
	}
}

void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(5.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		auto PathPoints = NavPath->GetPathPoints();
		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		}
	}
}

void AEnemy::Death()
{
	Destroy();
}

void AEnemy::GetEnemyElementalStatus()
{
	switch (EnemyElementalStatus)
	{
	case EEnemyElementalStatus::EES_Fire:
		bFireStatus = true;
		break;
	case EEnemyElementalStatus::EES_Water:
		bWaterStatus = true;
		break;
	case EEnemyElementalStatus::EES_Earth:
		bEarthStatus = true;
		break;
	case EEnemyElementalStatus::EES_Air:
		bAirStatus = true;
		break;
	}
}

void AEnemy::DamageTaken(float Amount)
{
	//UE_LOG(LogTemp, Warning, TEXT("Damage taken: %f"), Amount);
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		AMyPlayerController* Kills = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		Kills->KillCount(1.f);
		Death();
	}
	else
	{
		Health -= Amount;
		UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);
	}
}

void AEnemy::GetEnemyDifficultyStatus()
{
	AMyPlayerController* Difficulty = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	Difficulty->bIsLevelCleared();

	if (Difficulty->bEasy)
	{
		SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Easy);
		bEasy = true;
	}
	if (Difficulty->bMedium)
	{
		SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Medium);
		bMedium = true;
	}
	if (Difficulty->bHard)
	{
		SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Hard);
		bHard = true;
	}
}

