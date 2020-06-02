// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Components/SphereComponent.h"
#include "AiController.h"
#include "MainCharacter.h"
#include "MyPlayerController.h"
#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABoss::ABoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(1500.f);


	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(90.f);

	bOverLappingCombatSphere = false;

	MaxHealth = 1;
	Health = 1;

	HitDamage = 1;
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
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapEnd);

	SetBossElementalStatus(EBossElementalStatus::BES_Fire);

	GetBossDifficultyStatus();
	GetPlayerProgress();

	MaxHealth = 250 * DifficultyScaling * ProgressScaling;
	Health = MaxHealth;
	
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ABoss::TakeFireDamage(float Damage)
{
	switch (BossElementalStatus)
	{
	case EBossElementalStatus::BES_Fire:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Water:
		DamageGiven = Damage * HighResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Earth:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Air:
		DamageGiven = Damage * NoResistance;
		DamageTaken(DamageGiven);
		break;
	}
}

void ABoss::TakeWaterDamage(float Damage)
{
	switch (BossElementalStatus)
	{
	case EBossElementalStatus::BES_Fire:
		DamageGiven = Damage * NoResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Water:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Earth:
		DamageGiven = Damage * HighResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Air:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	}
}

void ABoss::TakeEarthDamage(float Damage)
{
	switch (BossElementalStatus)
	{
	case EBossElementalStatus::BES_Fire:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Water:
		DamageGiven = Damage * NoResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Earth:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Air:
		DamageGiven = Damage * HighResistance;
		DamageTaken(DamageGiven);
		break;
	}
}

void ABoss::TakeAirDamage(float Damage)
{
	switch (BossElementalStatus)
	{
	case EBossElementalStatus::BES_Fire:
		DamageGiven = Damage * HighResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Water:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Earth:
		DamageGiven = Damage * NoResistance;
		DamageTaken(DamageGiven);
		break;
	case EBossElementalStatus::BES_Air:
		DamageGiven = Damage * LowResistance;
		DamageTaken(DamageGiven);
		break;
	}
}

void ABoss::DamageTaken(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Damage taken: %f"), Amount);
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		AMyPlayerController* Kills = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		Kills->BossKillCount(1.f);
		Death();
	}
	else
	{
		Health -= Amount;
		UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);

		if (Health - Amount <= MaxHealth * 0.75f && Health > MaxHealth * 0.5f)
		{
			SetBossElementalStatus(EBossElementalStatus::BES_Water);
			UE_LOG(LogTemp, Warning, TEXT("Health left: %f, Watertype"), Health);
		}
		else if (Health <= MaxHealth * 0.5 && Health > MaxHealth * 0.25f)
		{
			SetBossElementalStatus(EBossElementalStatus::BES_Air);
			UE_LOG(LogTemp, Warning, TEXT("Health left: %f, AirType"), Health);
		}
		else if (Health <= MaxHealth * 0.25)
		{
			SetBossElementalStatus(EBossElementalStatus::BES_Earth);
			UE_LOG(LogTemp, Warning, TEXT("Health left: %f, EarthType"), Health);
		}
	}
}

void ABoss::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		
		if (MainCharacter)
		{
			SetBossMovementStatus(EBossMovementStatus::BMS_MoveToTarget);
			MainCharacter->SetBossTarget(this);
			MainCharacter->SetHasCombatTarget(true);
			if (MainCharacter->PlayerController)
			{
				MainCharacter->PlayerController->DisplayBossHealthBar();
			}
			MoveToTarget(MainCharacter);
		}
	}
}

void ABoss::AgroSphereOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			if (MainCharacter->BossTarget == this)
			{
				MainCharacter->SetCombatTarget(nullptr);
				MainCharacter->SetHasCombatTarget(false);
			}
			if (MainCharacter->PlayerController)
			{
				MainCharacter->PlayerController->HideBossHealthBar();
			}
			SetBossMovementStatus(EBossMovementStatus::BMS_Idle);
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void ABoss::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			SetBossMovementStatus(EBossMovementStatus::BMS_Attacking);

			if (bFireStatus)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Fire Damage!"));
				MainCharacter->FireDamage(HitDamage);
			}

			if (bWaterStatus)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Water Damage!"));
				MainCharacter->WaterDamage(HitDamage);
			}

			if (bEarthStatus)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Earth Damage!"));
				MainCharacter->EarthDamage(HitDamage);
			}

			if (bAirStatus)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Air Damage!"));
				MainCharacter->AirDamage(HitDamage);
			}
		}
	}
}

void ABoss::CombatSphereOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			SetBossMovementStatus(EBossMovementStatus::BMS_MoveToTarget);
			MoveToTarget(MainCharacter);
		}
	}
}

void ABoss::MoveToTarget(AMainCharacter* Target)
{
	SetBossMovementStatus(EBossMovementStatus::BMS_MoveToTarget);

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

			//UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		}
	}
}

void ABoss::Death()
{
	Destroy();
}

void ABoss::GetBossDifficultyStatus()
{
	AMyPlayerController* Difficulty = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	Difficulty->bGetGameDifficulty();

	if (Difficulty->bEasy)
	{
		SetBossDifficultyStatus(EBossDifficultyStatus::BDS_Easy);
		DifficultyScaling = 0.5f;
		bEasy = true;
	}
	if (Difficulty->bMedium)
	{
		SetBossDifficultyStatus(EBossDifficultyStatus::BDS_Medium);
		DifficultyScaling = 1.f;
		bMedium = true;
	}
	if (Difficulty->bHard)
	{
		SetBossDifficultyStatus(EBossDifficultyStatus::BDS_Hard);
		DifficultyScaling = 1.5f;
		bHard = true;
	}
}

void ABoss::GetPlayerProgress()
{
	AMyPlayerController* Progress = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());

	Progress->SetScaling();
	ProgressScaling = Progress->Scaling + 1.f;
	UE_LOG(LogTemp, Warning, TEXT("Progress Scaling: %f"), ProgressScaling);
}
