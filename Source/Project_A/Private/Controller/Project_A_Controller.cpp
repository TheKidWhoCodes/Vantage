// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/Project_A_Controller.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void AProject_A_Controller::SetSensitivity(float inSensitivity)
{
	Sensitivity = inSensitivity;

	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		PlayerCharacter->SetSensitivity(Sensitivity);
	}
}
