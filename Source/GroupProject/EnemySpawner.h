// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UENUM(BlueprintType)
enum class ESpawnerDifficultyStatus : uint8
{
	SDS_Easy	UMETA(DisplayName = "Easy"),
	SDS_Medium	UMETA(DisplayName = "Medium"),
	SDS_Hard	UMETA(DisplayName = "Hard"),

	SDS_MAX		UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class GROUPPROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();
	
	//Declaring a body for the spawner
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class UBoxComponent* SpawningBox;

	//Which actors to spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_4;

	//In case of boss
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> Actor_5;

	TArray<TSubclassOf<AActor>> SpawnArray;

	//Difficulty on spawn
	bool bEasy;
	bool bMedium;
	bool bHard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElementalStatus")
	ESpawnerDifficultyStatus SpawnerDifficultyStatus;

	FORCEINLINE void SetSpanwerDifficultyStatus(ESpawnerDifficultyStatus Status) { SpawnerDifficultyStatus = Status; }

	void GetSpawnerDifficultyStatus();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetSpawnPoint();

	UFUNCTION(BlueprintPure, Category = "Spawning")
	TSubclassOf<AActor> GetSpawnActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void SpawnActor(UClass* ToSpawn, const FVector& Location);

};
