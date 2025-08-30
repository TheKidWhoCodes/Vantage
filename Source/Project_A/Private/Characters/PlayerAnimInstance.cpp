// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerAnimInstance.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());

	if(PlayerCharacter)
	{
		MovementComponent = PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if(PlayerCharacter == nullptr) { return; }

	// Set Ground Speed
	if(MovementComponent)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
	}

	// Offset Yaw for Strafing
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());
	FRotator ControlRotation = PlayerCharacter->GetControlRotation();
	//YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, ControlRotation).Yaw;
	
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, ControlRotation);
	DesiredRotation = FMath::RInterpTo(DesiredRotation, DeltaRotation, DeltaSeconds, 9.f);
	YawOffset = DesiredRotation.Yaw;
}
