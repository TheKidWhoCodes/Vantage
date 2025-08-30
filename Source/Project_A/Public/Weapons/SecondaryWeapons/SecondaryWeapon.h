// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "SecondaryWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API ASecondaryWeapon : public ABaseWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess), Category = WeaponProperties)
	ESecondaryWeaponType SecondaryWeaponType;
};
