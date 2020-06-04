// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Fireball.h"
#include "WaterWave.h"
#include "EarthBlast.h"
#include "AirGun.h"
#include "MyPlayerController.h"
#include "Enemy.h"
#include "Boss.h"
#include "GameSaver.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/Decalcomponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"



// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1300.f;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 40.f; //Doesn't work.
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f)); // Y,Z,X
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bDoCollisionTest = false;

	//Make the camera follow
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f)); // Y,Z,X

	//Mouse
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	UE_LOG(LogTemp, Warning, TEXT("trying to add decal form hardcoded path!"));
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
		UE_LOG(LogTemp, Warning, TEXT("Succeded adding decal form hardcoded path!"));
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);



	SpellChoosen = 1.f;

	FireSpellCD = { 0.8f }; //0.8
	FireTimeSinceSpell = { 0 };


	WaterSpellCD = { 0.5f };
	WaterTimeSinceSpell = { 0 };

	EarthSpellCD = { 1.5f };
	EarthTimeSinceSpell = { 0 };

	AirSpellCD = { 1.5f };
	AirTimeSinceSpell = { 0 };


	//Movement
	CurrentVelocity = FVector(0.f);
	MaxSpeed = 0.f;
	bDash = false;
	DashCooldown = 1.f;
	DashTime = 0.15f;

	//Stats
	Health = 500.f;
	MaxHealth = 500.f;
	HealthRegen = 2.f;

	FireMaxMana = 200.f;
	WaterMaxMana = 200.f;
	EarthMaxMana = 200.f;
	AirMaxMana = 200.f;

	FireMana = FireMaxMana;
	WaterMana = WaterMaxMana;
	EarthMana = EarthMaxMana;
	AirMana = AirMaxMana;

	ManaRegen = 8.f;
	SkillPoints = 3.f;

	//Resistance
	FireResistance = 0.90f;
	WaterResistance = 0.90f;
	EarthResistance = 0.90f;
	AirResistance = 0.90f;

	//Cost
	FireManaCost = 65;
	WaterManaCost = 30;
	EarthManaCost = 65;
	AirManaCost = 65;

	//Upgrade Levels
	FireLvl		   = 1.f;
	WaterLvl	   = 1.f;
	EarthLvl   	   = 1.f;
	AirLvl		   = 1.f;
	HealthRegenLvl = 1.f;

	//Mana bar Color
	bFireEnoughMana  = false;
	bWaterEnoughMana = false;
	bEarthEnoughMana = false;
	bAirEnoughMana   = false;

	//Enemy HealthBar
	bHasCombatTarget = false;

	//Experience
	XPoints = 0;
	MaxXPoints = 0;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//RunSpeed = GetCharacterMovement()->MaxCustomMovementSpeed;
	PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	UpdateSpellStats();
	//LoadGame();
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Fire CD %f!"), FireSpellCD);
	if (!bPause)
	{
	//Movement:
	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	PlayerController->PlayerSpeed(CurrentVelocity);
	SetActorLocation(NewLocation);

		//Mouse:
		FHitResult Hit;
		bool HitResult = false;

		//using "ByChannel" to only get the World Static Meshes
		HitResult = GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldStatic), true, Hit);

		if (HitResult)
		{
			//Cursor location updates:





			FVector CursorLocation = Hit.Location;
			//UE_LOG(LogTemp, Warning, TEXT("Hit location %s!"), *Hit.Location.ToString());

			//Set cursor location above ground a little
			TempLocation = FVector(CursorLocation.X, CursorLocation.Y, 30.f);

			//Calculating direction
			NewDirectionToCursor = TempLocation - GetActorLocation();
			NewDirectionToCursor.Z = 0.f;
			NewDirectionToCursor.Normalize();
			SetActorRotation(NewDirectionToCursor.Rotation());

			//Cursor location updates:

			FVector CursorFV = Hit.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(Hit.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
		//End Mouse

		//Spell Casting
		FireTimeSinceSpell += DeltaTime;
		WaterTimeSinceSpell += DeltaTime;
		EarthTimeSinceSpell += DeltaTime;
		AirTimeSinceSpell += DeltaTime;
		Regeneration(HealthRegen, ManaRegen, DeltaTime); //Handles all the regeneration of mana and health
		if (bCasting == true)
		{
			CastSpell();
		}
		//End Spell Casting

		//Dashing
		TimeSinceDash += DeltaTime;
		DashDuration -= DeltaTime;
		if (DashDuration <= 0)
		{
			bDash = false;
			MaxSpeed = 0.f;
		}


		//ManaBarColor
		ManaBarColor();

		if (Health <= 0)
		{
			Dead();
		}

		if (CombatTarget)
		{
			CombatTargetLocation = CombatTarget->GetActorLocation();
			if (PlayerController)
			{
				PlayerController->EnemyLocation = CombatTargetLocation;
			}
		}
		if (BossTarget)
		{
			CombatTargetLocation = BossTarget->GetActorLocation();
			if (PlayerController)
			{
				PlayerController->BossLocation = CombatTargetLocation;
			}
		}
		GetPlayerExperience();
	}	
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveSideways", this, &AMainCharacter::MoveSideways);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::StartSpell);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AMainCharacter::StopSpell);

	PlayerInputComponent->BindAction("SpellOne", IE_Pressed, this, &AMainCharacter::SpellOne);
	PlayerInputComponent->BindAction("SpellTwo", IE_Pressed, this, &AMainCharacter::SpellTwo);
	PlayerInputComponent->BindAction("SpellThree", IE_Pressed, this, &AMainCharacter::SpellThree);
	PlayerInputComponent->BindAction("SpellFour", IE_Pressed, this, &AMainCharacter::SpellFour);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMainCharacter::Dash);
	PlayerInputComponent->BindAction("OpenTalentMenu", IE_Pressed, this, &AMainCharacter::OpenTalentMenu);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AMainCharacter::Pause);
}

void AMainCharacter::MoveForward(float Value)
{
	//FVector Direction;
	//FRotator Rotation = CameraBoom->GetComponentRotation();
	//Rotation -= GetViewRotation();
	//Rotation.Pitch = 0.f;
	//Direction = Rotation.Vector();
	//AddMovementInput(Direction, Value);
	/*
	if (!bPause)
	{
		CurrentVelocity.X = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
		AnimSpeedX = CurrentVelocity.X;
	}
	*/
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value+MaxSpeed);
}

void AMainCharacter::MoveSideways(float Value)
{
	//FVector Direction;
	//FRotator Rotation = CameraBoom->GetComponentRotation();
	//Rotation -= GetViewRotation();
	//Rotation.Pitch = 0.f;
	//Rotation.Yaw += 90.f;
	//Direction = Rotation.Vector();
	//AddMovementInput(Direction, Value);
	/*
	if (!bPause)
	{
		CurrentVelocity.Y = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
		AnimSpeedY = CurrentVelocity.Y;
	}
	*/
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}


void AMainCharacter::Dash()
{

	if (TimeSinceDash >= DashCooldown)
	{
		DashDuration = DashTime;
		bDash = true;
		TimeSinceDash = 0;
		MaxSpeed = 5000;
	}

}

void AMainCharacter::Pause()
{
	if (PlayerController && !bPause)
	{
		bPause = true;
		PlayerController->TogglePauseMenu();

		UE_LOG(LogTemp, Warning, TEXT("Pause menu"));
		
	}
	else if (PlayerController && bPause)
	{
		bPause = false;
		PlayerController->TogglePauseMenu();
		UE_LOG(LogTemp, Warning, TEXT("Pause menu"));
		
	}

}

void AMainCharacter::ResumeButton()
{
	PlayerController->TogglePauseMenu();
	bPause = false;
}

void AMainCharacter::CastSpell()
{
	//UWorld* World = GetWorld(); 

	FVector SpellSpawnLocation = GetActorLocation() + (GetActorForwardVector() * SpellForwardOffset);
	FRotator SpellSpawnRotation = GetActorRotation();
	Attack();

	if (FireSpellCD <= FireTimeSinceSpell && SpellChoosen == 1 && FireMana >= FireManaCost && !bPause)
	{
		if (FireLvl < 10)
		{
			GetWorld()->SpawnActor<AFireball>(Fireball_BP, SpellSpawnLocation, SpellSpawnRotation);
			FireTimeSinceSpell = 0;
			FireMana -= FireManaCost;
			UGameplayStatics::PlaySound2D(this, FireballSound);
		}
		if (FireLvl >= 10)
		{
			GetWorld()->SpawnActor<AFireball>(FireballLv2_BP, SpellSpawnLocation, SpellSpawnRotation);
			FireTimeSinceSpell = 0;
			FireMana -= FireManaCost;
			UGameplayStatics::PlaySound2D(this, FireballSound);
		}
	}
	if (WaterSpellCD <= WaterTimeSinceSpell && SpellChoosen == 2 && WaterMana >= WaterManaCost)
	{
		if(WaterLvl < 10)
		{
			GetWorld()->SpawnActor<AWaterWave>(WaterWave_BP, SpellSpawnLocation, SpellSpawnRotation);
			WaterTimeSinceSpell = 0;
			WaterMana -= WaterManaCost;
			UGameplayStatics::PlaySound2D(this, WaterWaveSound);
		}
		if (WaterLvl >= 10)
		{
			GetWorld()->SpawnActor<AWaterWave>(WaterWaveLv2_BP, SpellSpawnLocation, SpellSpawnRotation);
			WaterTimeSinceSpell = 0;
			WaterMana -= WaterManaCost;
			UGameplayStatics::PlaySound2D(this, WaterWaveSound);
		}
	}
	if (EarthSpellCD <= EarthTimeSinceSpell && SpellChoosen == 3 && EarthMana >= EarthManaCost)
	{
		GetWorld()->SpawnActor<AEarthBlast>(EarthBlast_BP, SpellSpawnLocation, SpellSpawnRotation);
		EarthTimeSinceSpell = 0;
		EarthMana -= EarthManaCost;
		UGameplayStatics::PlaySound2D(this, EarthBlastSound);
	}
	
	if (AirSpellCD <= AirTimeSinceSpell && SpellChoosen == 4 && AirMana >= AirManaCost)
	{
		GetWorld()->SpawnActor<AAirGun>(AirGun_BP, SpellSpawnLocation, SpellSpawnRotation);
		AirTimeSinceSpell = 0;
		AirMana -= AirManaCost;
		UGameplayStatics::PlaySound2D(this, AirGunSound);
	}
}

void AMainCharacter::Dead()
{
	//Destroy();
	ResetGame();
	NextLevel("MainMenu");

}

void AMainCharacter::StartSpell()
{
	bCasting = true;
}

void AMainCharacter::StopSpell()
{
	bCasting = false;
}

void AMainCharacter::SpellOne()
{
	SpellChoosen = 1.f;
}
void AMainCharacter::SpellTwo()
{
	SpellChoosen = 2.f;
}
void AMainCharacter::SpellThree()
{
	SpellChoosen = 3.f;
}
void AMainCharacter::SpellFour()
{
	SpellChoosen = 4.f;
}

void AMainCharacter::OpenTalentMenu()
{
	if (PlayerController && !bPause)
	{
		PlayerController->ToggleSkillMenu();
		bPause = true;
	}
	else if (PlayerController && bPause)
	{
		PlayerController->ToggleSkillMenu();
		bPause = false;
	}
}

//Getting Attacked
void AMainCharacter::FireDamage(float Damage)
{
	//if (Health - Damage <= 0.f)
	//{
	//	Health -= Damage;
	//	Dead();
	//}
	//else
	//{
	//	Health -= Damage;
	//	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth is %f"), Health);
	//}
	//------Borrowing this function for Enemy Damage!§-------
	UE_LOG(LogTemp, Warning, TEXT("Damage Before Resistance:  %f!"), Damage);
	Damage *= FireResistance;
	UE_LOG(LogTemp, Warning, TEXT("Damage taken:  %f!"), Damage);
	Health -= Damage;
}
void AMainCharacter::WaterDamage(float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Damage Before Resistance:  %f!"), Damage);
	Damage *= WaterResistance;
	UE_LOG(LogTemp, Warning, TEXT("Damage taken:  %f!"), Damage);
	Health -= Damage;
}
void AMainCharacter::EarthDamage(float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Damage Before Resistance:  %f!"), Damage);
	Damage *= EarthResistance;
	UE_LOG(LogTemp, Warning, TEXT("Damage taken:  %f!"), Damage);
	Health -= Damage;
}
void AMainCharacter::AirDamage(float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Damage Before Resistance:  %f!"), Damage);
	Damage *= AirResistance;
	UE_LOG(LogTemp, Warning, TEXT("Damage taken:  %f!"), Damage);
	Health -= Damage;
}

void AMainCharacter::Regeneration(float HealthRegenerationRate, float ManaRegenerationRate, float Time)
{

		HealthRegenerationRate = ManaRegen;
	if (Health <= MaxHealth)
	{
		Health += HealthRegenerationRate * Time; //Health per second
		if (Health >= MaxHealth)
		{
			Health = MaxHealth;
		}
	}
	if (FireMana <= FireMaxMana)
	{
		FireMana += ManaRegenerationRate * Time; //Regeneration per second
		if (FireMana >= FireMaxMana)
		{
			FireMana = FireMaxMana;
		}
	}
	if (WaterMana <= WaterMaxMana)
	{
		WaterMana += ManaRegenerationRate * Time; //Regeneration per second
		if (WaterMana >= WaterMaxMana)
		{
			WaterMana = WaterMaxMana;
		}
	}
	if (EarthMana <= EarthMaxMana)
	{
		EarthMana += ManaRegenerationRate * Time; //Regeneration per second
		if (EarthMana >= EarthMaxMana)
		{
			EarthMana = EarthMaxMana;
		}
	}
	if (AirMana <= AirMaxMana)
	{
		AirMana += ManaRegenerationRate * Time; //Regeneration per second
		if (AirMana >= AirMaxMana)
		{
			AirMana = AirMaxMana;
		}
	}
}

void AMainCharacter::ManaBarColor()
{
	if (FireMana >= FireManaCost)
	{
		bFireEnoughMana = true;
	}
	else
	{
		bFireEnoughMana = false;
	}
	if (WaterMana >= WaterManaCost)
	{
		bWaterEnoughMana = true;
	}
	else
	{
		bWaterEnoughMana = false;
	}
	if (EarthMana >= EarthManaCost)
	{
		bEarthEnoughMana = true;
	}
	else
	{
		bEarthEnoughMana = false;
	}
	if (AirMana >= AirManaCost)
	{
		bAirEnoughMana = true;
		//UE_LOG(LogTemp, Warning, TEXT("True"));
	}
	else
	{
		bAirEnoughMana = false;
		//UE_LOG(LogTemp, Warning, TEXT("false"));
	}
}

void AMainCharacter::NextLevel(FName LevelName)
{
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *MapName)

	UWorld* World = GetWorld();
	if (World)
	{
		NextLevelName = LevelName;
		SaveGame(true);
		UGameplayStatics::OpenLevel(World, LevelName);
	}
}

void AMainCharacter::UpdateCombatTarget()
{
	/**
	*
	Used for calculating which enemy is closest 
	So the closest enemy's health bar is visible
	*
	*/
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (PlayerController)
		{
			PlayerController->HideEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (PlayerController)
		{
			PlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMainCharacter::SaveGame(bool SaveLevel)
{
	float BossKills;
	float LevelsCleared;
	float Xpoints;
	float FoesDefeated;
	float FoesAlive;
	float PlayerLevel;

	UGameSaver* SaveStats = Cast<UGameSaver>(UGameplayStatics::CreateSaveGameObject(UGameSaver::StaticClass()));
	Kills         = PlayerController->Kills;
	BossKills     = PlayerController->BossKills;
	LevelsCleared = PlayerController->LevelsCleared;
	Xpoints       = PlayerController->XPoints;
	PlayerLevel   = PlayerController->PlayerLevel;
	//test if needed
	FoesDefeated  = PlayerController->FoesDefeated;
	FoesAlive     = PlayerController->FoesAlive;
	//
	SaveStats->CharacterStats.SkillPoints = SkillPoints;
	SaveStats->CharacterStats.MaxHealth = MaxHealth;
	SaveStats->CharacterStats.Health = Health;
	SaveStats->CharacterStats.FireLvl = FireLvl;
	SaveStats->CharacterStats.WaterLvl = WaterLvl;
	SaveStats->CharacterStats.EarthLvl = EarthLvl;
	SaveStats->CharacterStats.AirLvl = AirLvl;
	SaveStats->CharacterStats.HealthRegenLvl = HealthRegenLvl;
	SaveStats->CharacterStats.RegenLvl = RegenLvl;
	SaveStats->CharacterStats.ManaRegenLvl = ManaRegenLvl;
	SaveStats->CharacterStats.FireMana = FireMana;
	SaveStats->CharacterStats.WaterMana = WaterMana;
	SaveStats->CharacterStats.EarthMana = EarthMana;
	SaveStats->CharacterStats.AirMana = AirMana;
	SaveStats->CharacterStats.Kills = Kills;
	SaveStats->CharacterStats.BossKills = BossKills;
	SaveStats->CharacterStats.LevelsCleared = LevelsCleared;
	SaveStats->CharacterStats.Xpoints = Xpoints;
	SaveStats->CharacterStats.FoesDefeated = FoesDefeated;
	SaveStats->CharacterStats.FoesAlive = FoesAlive;
	SaveStats->CharacterStats.PlayerLevel = PlayerLevel;
	
	//UE_LOG(LogTemp, Warning, TEXT("Saving Level: %f"), NextLevelName);
	if (SaveLevel)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Saving Level: %f"), NextLevelName);
		SaveStats->CharacterStats.NextLevel = NextLevelName;
	}
	//SaveStats->CharacterStats.Location = GetActorLocation();
	//SaveStats->CharacterStats.Rotation = GetActorRotation();

	//PlayerController->SaveStats();

	UGameplayStatics::SaveGameToSlot(SaveStats, SaveStats->PlayerName, SaveStats->UserIndex);
}

void AMainCharacter::LoadGame(bool LoadLevel)
{
	UGameSaver* LoadStats = Cast<UGameSaver>(UGameplayStatics::CreateSaveGameObject(UGameSaver::StaticClass()));

	LoadStats = Cast<UGameSaver>(UGameplayStatics::LoadGameFromSlot(LoadStats->PlayerName, LoadStats->UserIndex));

	//SkillPoints = LoadStats->CharacterStats.SkillPoints;
	MaxHealth      = LoadStats->CharacterStats.MaxHealth;
	Health         = LoadStats->CharacterStats.Health;
	FireMana       = LoadStats->CharacterStats.FireMana;
	WaterMana      = LoadStats->CharacterStats.WaterMana;
	EarthMana      = LoadStats->CharacterStats.EarthMana;
	AirMana        = LoadStats->CharacterStats.AirMana;
	FireLvl        = LoadStats->CharacterStats.FireLvl;
	WaterLvl       = LoadStats->CharacterStats.WaterLvl;
	EarthLvl       = LoadStats->CharacterStats.EarthLvl;
	AirLvl         = LoadStats->CharacterStats.AirLvl;
	RegenLvl	   = LoadStats->CharacterStats.RegenLvl;
	ManaRegenLvl   = LoadStats->CharacterStats.ManaRegenLvl;
	HealthRegenLvl = LoadStats->CharacterStats.HealthRegenLvl;
	NextLevelName  = LoadStats->CharacterStats.NextLevel;

	PlayerController->LoadStats();

	//UE_LOG(LogTemp, Warning, TEXT("loading Level: %f"), NextLevelName);
	if (LoadLevel)
	{
		//UE_LOG(LogTemp, Warning, TEXT("loading Level: %f"), NextLevelName);
		NextLevel(NextLevelName);
	}
}

void AMainCharacter::GetPlayerExperience()
{
	XPoints = PlayerController->XPoints;
	MaxXPoints = PlayerController->MaxXp;

	SkillPoints = PlayerController->XpToken;

}
void AMainCharacter::LevelUpFire()
{
	if (SkillPoints >= 1.f)
	{
		SkillPoints -= 1.f;
		FireLvl += 1;
		UpdateSpellStats();
		PlayerController->XpToken -= 1.f;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No skill points"));
	}

}

void AMainCharacter::LevelUpWater()
{
	if (SkillPoints >= 1)
	{
		SkillPoints -= 1;
		PlayerController->XpToken -= 1.f;
		WaterLvl += 1;
		UpdateSpellStats();
	}


}

void AMainCharacter::LevelUpEarth()
{
	if (SkillPoints >= 1)
	{
		SkillPoints -= 1;
		PlayerController->XpToken -= 1.f;
		EarthLvl += 1;
		UpdateSpellStats();
	}

}

void AMainCharacter::LevelUpAir()
{
	if (SkillPoints >= 1)
	{
		SkillPoints -= 1;
		PlayerController->XpToken -= 1.f;
		AirLvl += 1;
		UpdateSpellStats();
	}

}
void AMainCharacter::LevelUpRegen()
{
	if (SkillPoints >= 1)
	{
		SkillPoints -= 1;
		PlayerController->XpToken -= 1.f;
		HealthRegenLvl += 1;
		RegenLvl += 1;
		UpdateSpellStats();
	}

}
void AMainCharacter::UpdateSpellStats()
{
	if (FireSpellCD >= 0.1f)
	{
		FireSpellCD = 2.f - (FireLvl / 20);
		UE_LOG(LogTemp, Warning, TEXT("Cooldown:  %f!"), FireSpellCD);
	}
	if (WaterSpellCD >= 0.1f)
	{
		WaterSpellCD = 1.f - (WaterLvl / 10);
		UE_LOG(LogTemp, Warning, TEXT("Cooldown:  %f!"), WaterSpellCD);
	}
	if (EarthSpellCD >= 0.1f)
	{
		EarthSpellCD = 2.f - (EarthLvl / 20);
		UE_LOG(LogTemp, Warning, TEXT("Cooldown:  %f!"), FireSpellCD);
	}
	if (AirSpellCD >= 0.1f)
	{
		AirSpellCD = 2.f - (AirLvl / 20);
		UE_LOG(LogTemp, Warning, TEXT("Cooldown:  %f!"), AirSpellCD);
	}
	FireMaxMana = 200 + (FireLvl * 10);
	WaterMaxMana = 200 + (WaterLvl * 10);
	EarthMaxMana = 200 + (WaterLvl * 10);
	AirMaxMana = 200 + (AirLvl * 10);

	HealthRegen = 1 + (HealthRegenLvl*2);
	ManaRegen = 6 + (RegenLvl*2);
	
}

void AMainCharacter::ResetGame()
{
	float BossKills;
	float LevelsCleared;
	float Xpoints;
	float PlayerLevel;

	Kills = PlayerController->Kills;
	BossKills = PlayerController->BossKills;
	LevelsCleared = PlayerController->LevelsCleared;
	Xpoints = PlayerController->XPoints;
	PlayerLevel = PlayerController->PlayerLevel;

	UGameSaver* SaveStats = Cast<UGameSaver>(UGameplayStatics::CreateSaveGameObject(UGameSaver::StaticClass()));

	SaveStats->CharacterStats.SkillPoints = 0;
	SaveStats->CharacterStats.MaxHealth = 500;
	SaveStats->CharacterStats.Health = 500;
	SaveStats->CharacterStats.FireLvl = 1;
	SaveStats->CharacterStats.WaterLvl = 1;
	SaveStats->CharacterStats.EarthLvl = 1;
	SaveStats->CharacterStats.AirLvl = 1;
	SaveStats->CharacterStats.HealthRegenLvl = 1;
	SaveStats->CharacterStats.RegenLvl = 1;
	SaveStats->CharacterStats.ManaRegenLvl = 1;
	SaveStats->CharacterStats.FireMana = 200;
	SaveStats->CharacterStats.WaterMana = 200;
	SaveStats->CharacterStats.EarthMana = 200;
	SaveStats->CharacterStats.AirMana = 200;
	SaveStats->CharacterStats.Kills = 0;
	SaveStats->CharacterStats.BossKills = 0;
	SaveStats->CharacterStats.LevelsCleared = 0;
	SaveStats->CharacterStats.Xpoints = 0;
	SaveStats->CharacterStats.PlayerLevel = 0;



	UGameplayStatics::SaveGameToSlot(SaveStats, SaveStats->PlayerName, SaveStats->UserIndex);

	LoadGame(false);
	PlayerController->LoadStats();
}

void AMainCharacter::Attack()
{
	if (!bAttacking)
	{
		bAttacking = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Selection = FMath::RandRange(0, 1);
			switch (Selection)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			default:
				;
			}
		}
	}
}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;
}


void AMainCharacter::SetEasy()
{
	bEasy = true;
	SaveDif();
}
void AMainCharacter::SetMedium()
{
	bMedium = true;
	SaveDif();
}

void AMainCharacter::SetHard()
{
	bHard = true;
	SaveDif();
}

void AMainCharacter::SaveDif()
{
	UGameSaver* SaveStats = Cast<UGameSaver>(UGameplayStatics::CreateSaveGameObject(UGameSaver::StaticClass()));

	SaveStats->CharacterStats.bEasy = bEasy;
	SaveStats->CharacterStats.bMedium = bMedium;
	SaveStats->CharacterStats.bHard = bHard;

	

	UE_LOG(LogTemp, Warning, TEXT("Difficulty is Easy: %s"), (bEasy ? TEXT("TRUE"):TEXT("FALSE")));

	UGameplayStatics::SaveGameToSlot(SaveStats, SaveStats->PlayerName, SaveStats->UserIndex);
}