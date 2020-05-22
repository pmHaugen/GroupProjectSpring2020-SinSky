// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "MainCharacter.h"
#include "MyPlayerController.h"

// Sets default values
ALevelTrigger::ALevelTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerPoint"));
	RootComponent = TriggerPoint;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(GetRootComponent());

	//To be changed!
	NextLevelName = "StartLevel";

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
	if (OtherActor)
	{
		//If main overlaps and the level is declared cleared from Playercontroller the map will change.
		AMainCharacter* Main = Cast<AMainCharacter>(OtherActor);
		if (Main)
		{
			AMyPlayerController* Clear = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
			Clear->bIsLevelCleared();
			if (Clear->bLevelCleared)
			{
				UE_LOG(LogTemp, Warning, TEXT("LevelCleared!"));
				//Main->NextLevel(NextLevelName);
			}
		}
	}
}


