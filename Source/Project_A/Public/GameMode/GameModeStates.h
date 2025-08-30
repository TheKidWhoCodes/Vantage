#pragma once

UENUM(BlueprintType)
enum class EGameModeState : uint8
{
	EGMS_Running UMETA(DisplayName = "Running"),
	EGMS_Loss UMETA(DisplayName = "Loss"),
	EGMS_Win UMETA(DisplayName = "Win")
};