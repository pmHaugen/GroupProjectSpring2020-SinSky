// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterWave.generated.h"

UCLASS()
class GROUPPROJECT_API AWaterWave : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterWave();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Collider;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	class UParticleSystemComponent* IdleParticlesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	class UParticleSystem* OverlapParticles;

	UPROPERTY(EditAnywhere)
	FVector Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float Damage;


	float TimeSinceSpawned{ 0 };

	UPROPERTY(VisibleAnywhere)
	float SpellDuration;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
