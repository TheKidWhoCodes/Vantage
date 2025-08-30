// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterStates.h"
#include "EnemyAnimInstance.generated.h"

class AEnemy;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class PROJECT_A_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
	AEnemy* Enemy;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;
	
	UPROPERTY(BlueprintReadWrite)
	EEnemyState EnemyState = EEnemyState::EES_Neutral;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float YawOffset;

	FRotator DesiredRotation;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	UAnimSequenceBase* NeutralPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	UAnimSequenceBase* IdlePose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	UBlendSpace* RunBlendSpace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Death)
	UAnimSequenceBase* DeathAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Death)
	UAnimSequenceBase* DeathPose;
	
};
