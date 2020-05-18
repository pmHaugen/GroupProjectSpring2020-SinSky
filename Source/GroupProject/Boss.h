// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

UENUM(BlueprintType)
enum class EBossMovementStatus : uint8
{
	BMS_Idle			UMETA(DisplayName = "Idle"),
	BMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	BMS_Attacking		UMETA(DisplayName = "Attacking"),

	BMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EBossElementalStatus : uint8
{
	BES_Fire	UMETA(DisplayName = "Fire"),
	BES_Water	UMETA(DisplayName = "Water"),
	BES_Earth	UMETA(DisplayName = "Earth"),
	BES_Air		UMETA(DisplayName = "Air"),

	BES_MAX		UMETA(DisplayName = "DefaultMAX")

};

UENUM(BlueprintType)
enum class EBossDifficultyStatus :uint8
{
	BDS_Easy	UMETA(DisplayName = "Easy"),
	BDS_Medium	UMETA(DisplayName = "Medium"),
	BDS_Hard	UMETA(DisplayName = "Hard"),

	BDS_MAX		UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class GROUPPROJECT_API ABoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss();

	//Boss Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EBossMovementStatus BossMovementStatus;

	FORCEINLINE void SetBossMovementStatus(EBossMovementStatus Status) { BossMovementStatus = Status; }

	//Follow Player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	//Attack Player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	//Boss Elemental Status

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElementalStatus")
	EBossElementalStatus BossElementalStatus;

	FORCEINLINE void SetBossElementalStatus(EBossElementalStatus Status) { BossElementalStatus = Status; }
	
	void RandomElementalStatus();

	//void GetBossElementalStatus();

	bool bFire;
	bool bWater;
	bool bEarth;
	bool bAir;

	bool bFireStatus;
	bool bWaterStatus;
	bool bEarthStatus;
	bool bAirStatus;

	/**
	*****Boss Properties
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BossProperties")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BossProperties")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BossProperties")
	float HitDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossProperties")
	float DamageGiven;

	/**
	*****Boss Resistance Properties
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ResistanceProperties")
	float HighResistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ResistanceProperties")
	float LowResistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ResistanceProperties")
	float NoResistance;

	/**
	*****Boss Difficulty
	*/

	bool bEasy;
	bool bMedium;
	bool bHard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElementalStatus")
	EBossDifficultyStatus BossDifficultyStatus;

	FORCEINLINE void SetEnemyDifficultyStatus(EBossDifficultyStatus Status) { BossDifficultyStatus = Status; }

	//void GetBossDifficultyStatus();

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
