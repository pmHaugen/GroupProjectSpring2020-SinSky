// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DisplayName="Attacking"), 

	EMS_MAX				UMETA(DisplayName="DefaultMAX")

};

UENUM(BlueprintType)
enum class EEnemyElementalStatus : uint8
{
	EES_Fire		UMETA(DisplayName = "FireStatus"),
	EES_Water		UMETA(DisplayName = "WaterStatus"),
	EES_Earth		UMETA(DisplayName = "EarthStatus"),
	EES_Air			UMETA(DisplayName = "AirStatus"),

	EES_MAX			UMETA(DisplayName ="DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyDifficultyStatus : uint8
{
	EDS_Easy	UMETA(DisplayName = "Easy"),
	EDS_Medium	UMETA(DisplayName = "Medium"),
	EDS_Hard	UMETA(DisplayName = "Hard"),

	EDS_MAX		UMETA(DisplayName= "DefaultMax")
};

UCLASS()
class GROUPPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }

	/**
	******Enemy ElementalStatus
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElementalStatus")
	EEnemyElementalStatus EnemyElementalStatus;

	FORCEINLINE void SetEnemyElementalStatus(EEnemyElementalStatus Status) { EnemyElementalStatus = Status; }

	void GetEnemyElementalStatus();

	bool bFireStatus;
	bool bWaterStatus;
	bool bEarthStatus;
	bool bAirStatus; 

	//Follow Player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	//Attack Player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	/**
	*****Enemy Properties
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyProperties")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyProperties")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyProperties")
	float HitDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyProperties")
	float DamageGiven;

	/**
	*****Enemy Resistance Properties
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="ResistanceProperties")
	float HighResistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ResistanceProperties")
	float LowResistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ResistanceProperties")
	float NoResistance;

	/**
	*****Enemy Difficulty
	*/

	bool bEasy;
	bool bMedium;
	bool bHard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElementalStatus")
	EEnemyDifficultyStatus EnemyDifficultyStatus;

	FORCEINLINE void SetEnemyDifficultyStatus(EEnemyDifficultyStatus Status) { EnemyDifficultyStatus = Status; }

	void GetEnemyDifficultyStatus();

	void GetSpawnerValues(UClass* Actor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeFireDamage(float Damage);

	void TakeWaterDamage(float Damage);
	
	void TakeEarthDamage(float Damage);

	void TakeAirDamage(float Damage);

	void DamageTaken(float Amount);

	void Death();

	UFUNCTION()
	void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void AgroSphereOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void CombatSphereOnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	void MoveToTarget(class AMainCharacter* Target);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverLappingCombatSphere;

};
