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
			if (Pawn)
			{
				MainCharacter = Cast<AMainCharacter>(Pawn);
			}
		}

		if (Pawn)
		{
		//	MovementSpeed = MainCharacter->CurrentAnimSpeed;
		//	UE_LOG(LogTemp, Warning, TEXT("animspeed:  %f!"), MovementSpeed);
			FVector Speed = Pawn->GetVelocity();
			FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);;
			MovementSpeed = LateralSpeed.Size();

			//UE_LOG(LogTemp, Warning, TEXT("Enemy Movement speed: %f"), MovementSpeed);
		}
	
}