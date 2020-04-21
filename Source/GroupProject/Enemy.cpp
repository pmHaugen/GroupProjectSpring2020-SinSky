// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AiController.h"
#include "MainCharacter.h"
#include "Fireball.h"
#include "WaterWave.h"
#include "MyPlayerController.h"

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

	MaxHealth = 150;
	Health = 130;

	HitDamage = 150;

	bFireStatus = false;
	bWaterStatus = false;
	bEarthStatus = false;
	bAirStatus = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);
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

void AEnemy::TakeDamage(float Damage)
{
	if (Health - Damage <= 0.f)
	{
		Health -= Damage;
		AMyPlayerController* Kills = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		Kills->KillCount(1.f);
		Death();
	}
	else
	{
		Health -= Damage;
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

}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{	
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

