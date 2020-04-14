// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterWave.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy.h"
#include "MainCharacter.h"

// Sets default values
AWaterWave::AWaterWave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AWaterWave::OnOverlapBegin);
	//Collider->SetSphereRadius(50.f);

	RootComponent = Collider;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fireball Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SpellDuration = 10;
	Speed = { 50.f, 0.f, 0.f };
	Damage = 5;
}

// Called when the game starts or when spawned
void AWaterWave::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaterWave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Speed += { 1500.f * DeltaTime, 0.f, 0.f };


	//Location.Y += (Direction.Y * DeltaTime) * Speed;
	//Location.X += (Direction.X * DeltaTime) * Speed;

	//Improved:

	AddActorLocalOffset(Speed * DeltaTime);

	TimeSinceSpawned += DeltaTime;

	if (TimeSinceSpawned >= SpellDuration)
	{
		Destroy();
	}

}

void AWaterWave::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor); //Sender til Main. Om det ikke er main sender den NULL
		if (Main) //Om det er MainCharacter:
		{
			Main->WaterDamage(Damage);
			Destroy();
		}

		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->TakeDamage(Damage);
			Destroy();
		}
	}
}
