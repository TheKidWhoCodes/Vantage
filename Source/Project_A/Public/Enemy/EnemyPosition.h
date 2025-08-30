// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "EnemyPosition.generated.h"

class USphereComponent;

/**
 * 
 */
UCLASS()
class PROJECT_A_API AEnemyPosition : public ATargetPoint
{
	GENERATED_BODY()

public:

	AEnemyPosition();
	
	UPROPERTY(VisibleAnywhere, Category = AINavigation)
	bool bIsOccupied = false;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* Sphere;
	
};
