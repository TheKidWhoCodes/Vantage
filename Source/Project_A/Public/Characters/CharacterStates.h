#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Neutral UMETA(DisplayName = "Neutral"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_UsingGear UMETA(DisplayName = "UsingGear"),
	ECS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Neutral UMETA(DisplayName = "Neutral"),
	EES_Engaged UMETA(DisplayName = "Engaged")
};