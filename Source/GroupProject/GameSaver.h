// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameSaver.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float SkillPoints;
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float FireLvl;
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float WaterLvl;
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float EarthLvl;
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float AirLvl;
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float RegenLvl;
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float ManaRegenLvl;
};
UCLASS()
class GROUPPROJECT_API UGameSaver : public USaveGame
{
	GENERATED_BODY()
public:
		UGameSaver();

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterSave;
	
};
