// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"
#include "MyPlayerController.h"

void UMainCharacterAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
	if (Pawn)
	{
		MainCharacter = Cast<AMainCharacter>(Pawn);
	}
}

void UMainCharacterAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		/* Hello, I am the only thing that works. BUT I WILL crash the engine!
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		MovementSpeed = PlayerController->CurrentPlayerSpeed;*/

		/**FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();*/

		//UE_LOG(LogTemp, Warning, TEXT("Movement speed: %f"), MovementSpeed);

		if (MainCharacter == nullptr)
		{
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}
}