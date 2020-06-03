// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"


UCLASS()
class GROUPPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	//Used for healthbars
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMyPlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class ABoss* BossTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FORCEINLINE void SetBossTarget(ABoss* Target) { BossTarget = Target; }

	//------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	float WalkSpeed;
	float RunSpeed;
	bool bCasting;
	bool bShowCursor = false;

	void ManaBarColor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION()
		//void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			//UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
			//bool bFromSweep, const FHitResult& SweepResult);

	void MoveSideways(float Value);
	void MoveForward(float Value);
	FVector CurrentVelocity;


	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Shoot") //Bullet Spawning offset
	float SpellForwardOffset = 200.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Shoot") //Bullet Spawning offset
	float SpellHeightOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* FireballSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* WaterWaveSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* EarthBlastSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* AirGunSound;

	//float FireCooldown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float FireTimeSinceSpell;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float FireSpellCD;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float FireMana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float FireMaxMana;
	float FireManaCost;

	//float WaterCooldown;
	UPROPERTY(BlueprintReadOnly, Category = "Non Editable")
	float WaterTimeSinceSpell;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float WaterSpellCD;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float WaterMana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float WaterMaxMana;
	float WaterManaCost;


	//float EarthCooldown;
	UPROPERTY(BlueprintReadOnly, Category = "Non Editable")
	float EarthTimeSinceSpell;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float EarthSpellCD;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float EarthMana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float EarthMaxMana;
	float EarthManaCost;

	//float AirCooldown;
	UPROPERTY(BlueprintReadOnly, Category = "Non Editable")
	float AirTimeSinceSpell;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float AirSpellCD;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float AirMana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float AirMaxMana;
	float AirManaCost;

	float DashCooldown;
	float DashDuration;
	float DashTime;
	float TimeSinceDash;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bDash;


	UPROPERTY(EditAnywhere, Category = "FireBall | Stage 1")
	TSubclassOf<class AFireball> Fireball_BP;
	UPROPERTY(EditAnywhere, Category = "FireBall | Stage 2")
	TSubclassOf<class AFireball> FireballLv2_BP;
	UPROPERTY(EditAnywhere, Category = "WaterWave | Stage 1")
	TSubclassOf<class AWaterWave> WaterWave_BP;
	UPROPERTY(EditAnywhere, Category = "WaterWave | Stage 2")
	TSubclassOf<class AWaterWave> WaterWaveLv2_BP;
	UPROPERTY(EditAnywhere, Category = "EarthBlast | Stage 1")
	TSubclassOf<class AEarthBlast> EarthBlast_BP;
	UPROPERTY(EditAnywhere, Category = "AirGun | Stage 1")
	TSubclassOf<class AAirGun> AirGun_BP;

	//Mouse
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mouse Setup")
	UDecalComponent* CursorToWorld;
	FVector NewDirectionToCursor;
	FVector TempLocation;



	//Spells:
	float SpellChoosen;

	void CastSpell();
	void StartSpell();
	void StopSpell();
	void Dash();

	void SpellOne();
	void SpellTwo();
	void SpellThree();
	void SpellFour();

	//Taking Damage
	void FireDamage(float Damage);
	void WaterDamage(float Damage);
	void EarthDamage(float Damage);
	void AirDamage(float Damage);
	void Dead();

	//--------------------------Stats----------------------------

	float Kills;
	//Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	float HealthRegen;
	float ManaRegen;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrades")
	float SkillPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrades")
	float FireLvl;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrades")
	float WaterLvl;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrades")
	float EarthLvl;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrades")
	float AirLvl;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrades")
	float RegenLvl;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrades")
	float ManaRegenLvl;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Upgrades")
	float HealthRegenLvl;


	//Defence
	float FireResistance;
	float WaterResistance;
	float EarthResistance;
	float AirResistance;

	//Movement
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed;

	//ManaBarColor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ManaBar Color")
	bool bFireEnoughMana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ManaBar Color")
	bool bWaterEnoughMana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ManaBar Color")
	bool bEarthEnoughMana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ManaBar Color")
	bool bAirEnoughMana;

	void Regeneration(float HealthRegenerationRate, float RegenerationRate, float Time);

	//Experience
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	float XPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	float MaxXPoints;

	void GetPlayerExperience();
	//-----------------------------------------------------------

	void OpenTalentMenu();
	void Pause();
	UFUNCTION(BlueprintCallable)
	void ResumeButton();
	bool bPause;

	void NextLevel(FName LevelName);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	FName NextLevelName;

	UFUNCTION(BlueprintCallable)
	void SaveGame(bool SaveLevel);

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool LoadLevel);

	UFUNCTION(BlueprintCallable)
	void ResetGame();

	UFUNCTION(BlueprintCallable)
	void LevelUpFire();

	UFUNCTION(BlueprintCallable)
	void LevelUpWater();

	UFUNCTION(BlueprintCallable)
	void LevelUpEarth();

	UFUNCTION(BlueprintCallable)
	void LevelUpAir();

	UFUNCTION(BlueprintCallable)
	void LevelUpRegen();

	UFUNCTION(BlueprintCallable)
	void UpdateSpellStats();

};
