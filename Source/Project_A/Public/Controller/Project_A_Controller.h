// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Project_A_Controller.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_A_API AProject_A_Controller : public APlayerController
{
	GENERATED_BODY()


public:
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, BlueprintPure)
	bool GetUsingMouse_Implementation();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUsingMouse_Implementation(bool UsingMouse);

	UFUNCTION(BlueprintCallable)
	void SetSensitivity(float inSensitivity);

protected:

	UPROPERTY(BlueprintReadWrite)
	bool bIsUsingMouse = false;

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float Sensitivity = 80.f;
	
};
