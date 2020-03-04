// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/Decalcomponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
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

	//Mouse
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	UE_LOG(LogTemp, Warning, TEXT("trying to add decal form hardcoded path!"));
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
		UE_LOG(LogTemp, Warning, TEXT("Succeded adding decal form hardcoded path!"));
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);

	SpellCD = { 0.5f };
	Cooldown = { 0 };
	TimeSinceSpell = { 0 };

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

	TimeSinceSpell += DeltaTime;
	if (bCasting == true)
	{
		CastSpell();
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveSideways", this, &AMainCharacter::MoveSideways);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	//InputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::CastSpell);// old without the ability to hold for fire
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::StartSpell);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AMainCharacter::StopSpell);
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

	if (SpellCD <= TimeSinceSpell)
	{
		if (SpellChoosen == 1)
		{
			if (World)
			{
				World->SpawnActor<AFireball>(Fireball_BP, SpellSpawnLocation, SpellSpawnRotation);
				TimeSinceSpell = 0;
			}
		}
	}
}

void AMainCharacter::StartSpell()
{
	bCasting = true;
}

void AMainCharacter::StopSpell()
{
	bCasting = false;
}

void AMainCharacter::SpellChooser()
{
	SpellChoosen = 1;
}
