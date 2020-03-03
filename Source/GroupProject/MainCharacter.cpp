// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Fireball.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1500.f;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 40.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f)); // Y,Z,X
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bDoCollisionTest = false;

	//Make the camera follow
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f)); // Y,Z,X

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//RunSpeed = GetCharacterMovement()->MaxCustomMovementSpeed;
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveSideways", this, &AMainCharacter::MoveSideways);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	InputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::CastSpell);
}

void AMainCharacter::MoveForward(float Value)
{
	FVector Direction;
	FRotator tempRotation = CameraBoom->GetComponentRotation();
	tempRotation -= GetViewRotation();
	tempRotation.Pitch = 0.f;
	Direction = tempRotation.Vector();
	AddMovementInput(Direction, Value);
}

void AMainCharacter::MoveSideways(float Value)
{
	FVector Direction;
	FRotator tempRotation = CameraBoom->GetComponentRotation();
	tempRotation -= GetViewRotation();
	tempRotation.Pitch = 0.f;
	tempRotation.Yaw += 90.f;
	Direction = tempRotation.Vector();
	AddMovementInput(Direction, Value);
}

void AMainCharacter::CastSpell()
{
	UWorld* World = GetWorld();

	FVector SpellSpawnLocation = GetActorLocation() + (GetActorForwardVector() * SpellForwardOffset);
	FRotator SpellSpawnRotation = GetActorRotation();

	if (SpellChoosen == 1)
	{
		if (World)
		{
			World->SpawnActor<AFireball>(Fireball_BP, SpellSpawnLocation, SpellSpawnRotation);
		}
	}
}

void AMainCharacter::SpellChooser()
{
	SpellChoosen = 1;
}
