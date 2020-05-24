// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTrigger.generated.h"

UCLASS()
class GROUPPROJECT_API ALevelTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelTrigger();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	class UBoxComponent* TriggerPoint;

	class UBillboardComponent* Billboard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelSelect")
	FName NextLevelName;

	FName LevelName = "AirLevel";

	//Selection of levels can be done in blueprints.

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSelect")
	TAssetPtr<UWorld> Level_1;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSelect")
	TAssetPtr<UWorld> Level_2;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSelect")
	TAssetPtr<UWorld> Level_3;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSelect")
	TAssetPtr<UWorld> Level_4;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "LevelSelect")
	TAssetPtr<UWorld> Level_5;

	TArray<TAssetPtr<UWorld>> LevelArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintPure, Category = "Spawning")
	TAssetPtr<UWorld> GetLevel();

};
