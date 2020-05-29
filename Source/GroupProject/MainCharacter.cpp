// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Fireball.h"
#include "WaterWave.h"
#include "EarthBlast.h"
#include "AirGun.h"

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
#include "MyPlayerController.h"
#include "Enemy.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 2000.f;
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

	FireSpellCD = { 0.8f };
	FireTimeSinceSpell = { 0 };


	WaterSpellCD = { 0.1f };
	WaterTimeSinceSpell = { 0 };

	EarthSpellCD = { 2.f };
	EarthTimeSinceSpell = { 0 };

	AirSpellCD = { 1.f };
	AirTimeSinceSpell = { 0 };


	//Movement
	CurrentVelocity = FVector(0.f);
	MaxSpeed = 400.f;
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
	FireManaCost = 50;
	WaterManaCost = 5;
	EarthManaCost = 50;
	AirManaCost = 50;

	//Upgrade Levels
	FireLvl = 1.f;
	WaterLvl = 1.f;

	//Mana bar Color
	bFireEnoughMana = false;
	bWaterEnoughMana = false;
	bEarthEnoughMana = false;
	bAirEnoughMana = false;

	//Enemy HealthBar
	bHasCombatTarget = false;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//RunSpeed = GetCharacterMovement()->MaxCustomMovementSpeed;
	PlayerController = Cast<AMyPlayerController>(GetController());
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Fire CD %f!"), FireSpellCD);

	//Movement:
	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
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
			MaxSpeed = 400.f;
		}
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
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveSideways", this, &AMainCharacter::MoveSideways);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	//InputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::CastSpell);// old without the ability to hold for fire
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::StartSpell);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AMainCharacter::StopSpell);

	PlayerInputComponent->BindAction("SpellOne", IE_Pressed, this, &AMainCharacter::SpellOne);
	PlayerInputComponent->BindAction("SpellTwo", IE_Pressed, this, &AMainCharacter::SpellTwo);
	PlayerInputComponent->BindAction("SpellThree", IE_Pressed, this, &AMainCharacter::SpellThree);
	PlayerInputComponent->BindAction("SpellFour", IE_Pressed, this, &AMainCharacter::SpellFour);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMainCharacter::Dash);
	PlayerInputComponent->BindAction("OpenTalentMenu", IE_Pressed, this, &AMainCharacter::OpenTalentMenu);
}

void AMainCharacter::MoveForward(float Value)
{
	//FVector Direction;
	//FRotator Rotation = CameraBoom->GetComponentRotation();
	//Rotation -= GetViewRotation();
	//Rotation.Pitch = 0.f;
	//Direction = Rotation.Vector();
	//AddMovementInput(Direction, Value);
	CurrentVelocity.X = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
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
	CurrentVelocity.Y = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
}

void AMainCharacter::Dash()
{
	if (TimeSinceDash >= DashCooldown)
	{
		DashDuration = DashTime;
		TimeSinceDash = 0;
		MaxSpeed = 5000;
	}
}

void AMainCharacter::CastSpell()
{
	//UWorld* World = GetWorld(); 

	FVector SpellSpawnLocation = GetActorLocation() + (GetActorForwardVector() * SpellForwardOffset);
	FRotator SpellSpawnRotation = GetActorRotation();

	if (FireSpellCD <= FireTimeSinceSpell && SpellChoosen == 1 && FireMana >= FireManaCost)
	{
		if (FireLvl == 1)
		{
			GetWorld()->SpawnActor<AFireball>(Fireball_BP, SpellSpawnLocation, SpellSpawnRotation);
			FireTimeSinceSpell = 0;
			FireMana -= FireManaCost;
			UGameplayStatics::PlaySound2D(this, FireballSound);
		}
		if (FireLvl == 2)
		{
			GetWorld()->SpawnActor<AFireball>(FireballLv2_BP, SpellSpawnLocation, SpellSpawnRotation);
			FireTimeSinceSpell = 0;
			FireMana -= FireManaCost;
			UGameplayStatics::PlaySound2D(this, FireballSound);
		}
	}
	if (WaterSpellCD <= WaterTimeSinceSpell && SpellChoosen == 2 && WaterMana >= WaterManaCost)
	{
		if(WaterLvl == 1)
		{
			GetWorld()->SpawnActor<AWaterWave>(WaterWave_BP, SpellSpawnLocation, SpellSpawnRotation);
			WaterTimeSinceSpell = 0;
			WaterMana -= WaterManaCost;
			UGameplayStatics::PlaySound2D(this, WaterWaveSound);
		}
		if (WaterLvl == 2)
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
	Destroy();
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
	AMyPlayerController* MenuController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	MenuController->OpenSkillMenu();

	if (bShowCursor == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->bShowMouseCursor = true;
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;
		}
	}
	if (bShowCursor == false) 
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->bShowMouseCursor = false;
			PC->bEnableClickEvents = false;
			PC->bEnableMouseOverEvents = false;
		}
	}
	bShowCursor = !bShowCursor;
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
	if (RegenLvl==2)
	{
		HealthRegenerationRate = 50;
	}
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
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::OpenLevel(World, LevelName);
	}
}