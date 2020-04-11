// Fill out your copyright notice in the Description page of Project Settings.


#include "EarthBlast.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MainCharacter.h"
#include "Enemy.h"


// Sets default values
AEarthBlast::AEarthBlast()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AEarthBlast::OnOverlapBegin);
	Collider->SetSphereRadius(50.f);

	RootComponent = Collider;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fireball Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SpellDuration = 2;
	Speed = { 100.f, 0.f, 0.f };
	Damage = 100;
}

// Called when the game starts or when spawned
void AEarthBlast::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEarthBlast::Tick(float DeltaTime)
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

void AEarthBlast::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->IsA(AMainCharacter::StaticClass()))
	{

		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor); //Sender til Main. Om det ikke er main sender den NULL
		if (Main) //Om det er Main:
		{
			Main->FireDamage(Damage);
			Destroy();
		}
	}
	/**------MOVED TO ENEMY.CPP

	if (OtherActor->IsA(AEnemy::StaticClass())) /
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
				Enemy->TakeDamage(Damage);
				Destroy();
		}


		//UE_LOG(LogTemp, Warning, TEXT("Destroyed"));

	}*/
}




