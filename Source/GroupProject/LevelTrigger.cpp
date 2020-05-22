// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"

// Sets default values
ALevelTrigger::ALevelTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerPoint"));
	RootComponent = TriggerPoint;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ALevelTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerPoint->OnComponentBeginOverlap.AddDynamic(this, &ALevelTrigger::OnOverlapBegin);
	
}

// Called every frame
void ALevelTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}


