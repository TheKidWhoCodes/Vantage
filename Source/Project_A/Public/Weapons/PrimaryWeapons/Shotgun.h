// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/PrimaryWeapons/PrimaryWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API AShotgun : public APrimaryWeapon
{
	GENERATED_BODY()

public:
	
	virtual void ReloadFinished() override;

protected:

	virtual void PerformTraces() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponProperties)
	int32 NumOfPellets = 5;
	
};
