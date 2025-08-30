// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "PrimaryWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API APrimaryWeapon : public ABaseWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess), Category = WeaponProperties)
	EPrimaryWeaponType PrimaryWeaponType;
};
