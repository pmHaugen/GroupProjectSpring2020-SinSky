// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnimInstance.h"
#include "Boss.h"

void UBossAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Boss = Cast<ABoss>(Pawn);
		}
	}
}

void UBossAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			FVector Speed = Pawn->GetVelocity();
			FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
			MovementSpeed = LateralSpeed.Size();

			UE_LOG(LogTemp, Warning, TEXT("Boss Movement speed: %f"), MovementSpeed);

			Boss = Cast<ABoss>(Pawn);
		}
	}
}
