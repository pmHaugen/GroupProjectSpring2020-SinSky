// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AiController.h"
#include "MainCharacter.h"
#include "Fireball.h"
#include "WaterWave.h"
#include "MyPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

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

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("DuckEnemySocket"));

	HPSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HPSphere"));
	HPSphere->SetupAttachment(GetRootComponent());
	HPSphere->InitSphereRadius(1000.f);

	bOverLappingCombatSphere= false;

	DifficultyScaling = 0;
	ProgressScaling = 0;

	MaxHealth = 1;
	Health = 1;

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

	AttackMinTime = 0.5f;
	AttackMaxTime = 2.5f;

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

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	HPSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::HPSphereOnOverlapBegin);
	HPSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::HPSphereOnOverlapEnd);

	GetEnemyElementalStatus();
	GetEnemyDifficultyStatus();
	GetPlayerProgress();
	MaxHealth = 100 * DifficultyScaling * ProgressScaling;
	Health = MaxHealth;

	/**UE_LOG(LogTemp, Warning, TEXT("Difficulty is Easy: %s"), (bEasy ? TEXT("TRUE"):TEXT("FALSE")));
	UE_LOG(LogTemp, Warning, TEXT("Difficulty is Medium: %s"), (bMedium ? TEXT("TRUE") : TEXT("FALSE")));
	UE_LOG(LogTemp, Warning, TEXT("Difficulty is Hard: %s"), (bHard ? TEXT("TRUE") : TEXT("FALSE")));*/
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
			//Redundant, but will cause crash if removed.
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

			MoveToTarget(MainCharacter);
			//MainCharacter->UpdateCombatTarget();
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
			MainCharacter->UpdateCombatTarget();

			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::HPSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			//UE_LOG(LogTemp, Warning, TEXT("HPSPhereoverlapbegin"));
			MainCharacter->SetCombatTarget(this);
			MainCharacter->SetHasCombatTarget(true);
			MainCharacter->UpdateCombatTarget();
		}
	}
}

void AEnemy::HPSphereOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			if (MainCharacter->CombatTarget == this)
			{
				MainCharacter->SetCombatTarget(nullptr);
				MainCharacter->SetHasCombatTarget(false);
				MainCharacter->UpdateCombatTarget();
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
			CombatTarget = MainCharacter;
			//SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
			bOverLappingCombatSphere = true;
			Attack();

			//MainCharacter->UpdateCombatTarget();
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
			bOverLappingCombatSphere = false;
			if (EnemyMovementStatus!= EEnemyMovementStatus::EMS_Attacking)
			{
				//SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
				MoveToTarget(MainCharacter);
				CombatTarget = nullptr;
			}
			//MainCharacter->UpdateCombatTarget();
			if (MainCharacter== nullptr)
			{
				MoveToTarget(nullptr);
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			}
		}
	}
}


void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			if (bFireStatus)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Fire Damage!"));
				Main->FireDamage(HitDamage);
			}

			if (bWaterStatus)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Water Damage!"));
				Main->WaterDamage(HitDamage);
			}

			if (bEarthStatus)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Earth Damage!"));
				Main->EarthDamage(HitDamage);
			}

			if (bAirStatus)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Air Damage!"));
				Main->AirDamage(HitDamage);
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	if (AIController)
	{
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}
	if (!bAttacking)
	{
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	if (bOverLappingCombatSphere)
	{
		Attack();
	}
}

void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(50.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);
		if (Target == nullptr)
		{
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		}
	}
}

void AEnemy::Death()
{
	AMyPlayerController* XpTrack = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	XpTrack->AquireXp(15);
	Destroy();
}

void AEnemy::GetEnemyElementalStatus()
{
	int32 Status;

	Status = FMath::RandRange(1, 4);
	switch (Status)
	{
	case 1:
		SetEnemyElementalStatus(EEnemyElementalStatus::EES_Fire);
		//UE_LOG(LogTemp, Warning, TEXT("Fire"));
		bFireStatus = true;
		break;
	case 2: 
		SetEnemyElementalStatus(EEnemyElementalStatus::EES_Water);
		//UE_LOG(LogTemp, Warning, TEXT("Water"));
		bWaterStatus = true;
		break;
	case 3:
		SetEnemyElementalStatus(EEnemyElementalStatus::EES_Earth);
		//UE_LOG(LogTemp, Warning, TEXT("Earth"));
		bEarthStatus = true;
		break;
	case 4:
		SetEnemyElementalStatus(EEnemyElementalStatus::EES_Air);
		//UE_LOG(LogTemp, Warning, TEXT("Air"));
		bAirStatus = true;
		break;
	}
}

void AEnemy::DamageTaken(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Damage taken: %f"), Amount);
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
	Difficulty->GetGameDifficulty();

	if (Difficulty->bEasy)
	{
		SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Easy);
		DifficultyScaling = 0.5f;
		bEasy = true;
	}
	if (Difficulty->bMedium)
	{
		SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Medium);
		DifficultyScaling = 1.f;
		bMedium = true;
	}
	if (Difficulty->bHard)
	{
		SetEnemyDifficultyStatus(EEnemyDifficultyStatus::EDS_Hard);
		DifficultyScaling = 1.5f;
		bHard = true;
	}
}

void AEnemy::GetPlayerProgress()
{
	AMyPlayerController* Progress = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());

	Progress->SetScaling();
	ProgressScaling = Progress->Scaling +1.f;
	//UE_LOG(LogTemp, Warning, TEXT("Progress Scaling: %f"), ProgressScaling);
}

