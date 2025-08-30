// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Enemy = Cast<AEnemy>(TryGetPawnOwner());

	if(Enemy)
	{
		MovementComponent = Enemy->GetCharacterMovement();
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(Enemy == nullptr) { return; }
	
	EnemyState = Enemy->GetEnemyState();
	
	// Set Ground Speed
	if(MovementComponent)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
	}

	// Offset Yaw for Strafing
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Enemy->GetVelocity());
	FRotator ControlRotation = Enemy->GetActorRotation();
	//YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, ControlRotation).Yaw;
	
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, ControlRotation);
	DesiredRotation = FMath::RInterpTo(DesiredRotation, DeltaRotation, DeltaSeconds, 10.f);
	YawOffset = DesiredRotation.Yaw;
}
