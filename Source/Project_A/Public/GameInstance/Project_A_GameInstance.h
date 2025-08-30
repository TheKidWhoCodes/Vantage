// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Weapons/WeaponData.h"
#include "Project_A_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API UProject_A_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	EPrimaryWeaponType PrimaryWeaponSelection = EPrimaryWeaponType::EWT_AssaultRifle;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	ESecondaryWeaponType SecondaryWeaponSelection = ESecondaryWeaponType::EWT_Handgun;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	EGearType GearSelection = EGearType::EGT_Grenade;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bVibrationEnabled = true;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bIsUsingMouse = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bScreenShakeEnabled = true;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	float Sensitivity = 80.f;

public:
	
	FORCEINLINE bool GetScreenShakeEnabled() const { return bScreenShakeEnabled; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetPrimaryWeaponSelection(const EPrimaryWeaponType WeaponType) { PrimaryWeaponSelection = WeaponType; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetSecondaryWeaponSelection(const ESecondaryWeaponType WeaponType) { SecondaryWeaponSelection = WeaponType; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetGearSelection(const EGearType GearType) { GearSelection = GearType; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EPrimaryWeaponType GetPrimaryWeaponSelection() const { return PrimaryWeaponSelection; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ESecondaryWeaponType GetSecondaryWeaponSelection() const { return SecondaryWeaponSelection; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EGearType GetGearSelection() const { return GearSelection; }
	
};
