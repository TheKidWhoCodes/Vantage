// Fill out your copyright notice in the Description page of Project Settings.


#include "Levels/BaseLevel.h"
#include "GameMode/Project_A_GameMode.h"

void ABaseLevel::BeginPlay()
{
	Super::BeginPlay();

	if(GetWorld())
	{
		if(AProject_A_GameMode* GameMode = Cast<AProject_A_GameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->CreateStartWidget(LevelName, LevelTime);
			GameMode->CreatePlayerWidget();
		}
	}
}
