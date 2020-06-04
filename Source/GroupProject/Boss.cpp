// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Components/SphereComponent.h"
#include "AiController.h"
#include "MainCharacter.h"
#include "MyPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

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

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("DamageSocket"));

	bOverLappingCombatSphere = false;

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
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoss::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &ABoss::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SetBossElementalStatus(EBossElementalStatus::BES_Fire);
	bFireStatus = true;

	GetBossDifficultyStatus();
	GetPlayerProgress();

	MaxHealth = 500 * DifficultyScaling * ProgressScaling;
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
		//UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);

		if (Health - Amount <= MaxHealth * 0.75f && Health > MaxHealth * 0.5f)
		{
			SetBossElementalStatus(EBossElementalStatus::BES_Water);
			bWaterStatus = true;
			//UE_LOG(LogTemp, Warning, TEXT("Health left: %f, Watertype"), Health);
		}
		else if (Health <= MaxHealth * 0.5 && Health > MaxHealth * 0.25f)
		{
			bAirStatus = true;
			SetBossElementalStatus(EBossElementalStatus::BES_Air);
			//UE_LOG(LogTemp, Warning, TEXT("Health left: %f, AirType"), Health);
		}
		else if (Health <= MaxHealth * 0.25)
		{
			bEarthStatus = true;
			SetBossElementalStatus(EBossElementalStatus::BES_Earth);
			//UE_LOG(LogTemp, Warning, TEXT("Health left: %f, EarthType"), Health);
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
			MainCharacter->BossTarget = this;
			MainCharacter->SetHasCombatTarget(true);
			MainCharacter->PlayerController->DisplayBossHealthBar();
			//Keep me or the project will crash
			SetBossMovementStatus(EBossMovementStatus::BMS_MoveToTarget);

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
			CombatTarget = MainCharacter;
			bOverLappingCombatSphere = true;
			Attack();
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
			bOverLappingCombatSphere = false;
			if (BossMovementStatus != EBossMovementStatus::BMS_Attacking)
			{
				MoveToTarget(MainCharacter);
				CombatTarget = nullptr;
			}
			if (MainCharacter == nullptr)
			{
				MoveToTarget(nullptr);
				SetBossMovementStatus(EBossMovementStatus::BMS_Idle);
			}
		}
	}
}
void ABoss::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Functions"));
	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Combatonoverlapbegin"));
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

void ABoss::CombatOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABoss::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABoss::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABoss::Attack()
{
	if (AIController)
	{
		AIController->StopMovement();
		SetBossMovementStatus(EBossMovementStatus::BMS_Attacking);
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

void ABoss::AttackEnd()
{
	bAttacking = false;
	if (bOverLappingCombatSphere)
	{
		Attack();
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

		AIController->MoveTo(MoveRequest, &NavPath);
		if (Target == nullptr)
		{
			AIController->StopMovement();
			SetBossMovementStatus(EBossMovementStatus::BMS_Idle);
		}
	}
}

void ABoss::Death()
{
	//Change values.

	AMyPlayerController* XpTrack = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	XpTrack->AquireXp(60);
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
	//UE_LOG(LogTemp, Warning, TEXT("Progress Scaling: %f"), ProgressScaling);
}
