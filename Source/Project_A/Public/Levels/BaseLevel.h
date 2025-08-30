// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "BaseLevel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API ABaseLevel : public ALevelScriptActor
{

protected:

	virtual void BeginPlay() override;

private:
	
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = LevelProperties)
	FString LevelName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = LevelProperties)
	FString LevelTime;
};
