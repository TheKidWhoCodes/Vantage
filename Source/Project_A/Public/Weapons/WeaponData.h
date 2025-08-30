#pragma once

UENUM(BlueprintType)
enum class EPrimaryWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun")
};

UENUM(BlueprintType)
enum class ESecondaryWeaponType : uint8
{
	EWT_Handgun UMETA(DisplayName = "Handgun")
};

UENUM(BlueprintType)
enum class EGearType : uint8
{
	EGT_Grenade UMETA(DisplayName = "Grenade")
};

UENUM(BlueprintType)
enum class EWeaponCategory : uint8
{
	EWC_Primary UMETA(DisplayName = "Primary"),
	EWC_Secondary UMETA(DisplayName = "Secondary")
};

UENUM(BlueprintType)
enum class EWeaponFireType : uint8
{
	EWFT_FullAuto UMETA(DisplayName = "FullAuto"),
	EWFT_SemiAuto UMETA(DisplayName = "SemiAuto"),
};
