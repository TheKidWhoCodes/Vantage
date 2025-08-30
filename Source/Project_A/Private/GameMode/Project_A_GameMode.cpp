// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Project_A_GameMode.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Controller/Project_A_Controller.h"
#include "Kismet/GameplayStatics.h"

void AProject_A_GameMode::StartPlay()
{
	Super::StartPlay();

	PlayerController = Cast<AProject_A_Controller>(GetWorld()->GetFirstPlayerController());
}

void AProject_A_GameMode::PauseGame()
{
	if(UGameplayStatics::IsGamePaused(this))
	{
		// Game is paused. Unpause if possible.
		if(!CanUnpause) return;
		
		UGameplayStatics::SetGamePaused(this, false);

		TArray<UUserWidget*> FoundWidgets;
		
		if(PauseWidgetClass)
		{
			UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, PauseWidgetClass);
		}

		if(FoundWidgets[0])
		{
			FoundWidgets[0]->RemoveFromParent();

			if(PlayerController)
			{
				PlayerController->SetInputMode(FInputModeGameOnly());
				PlayerController->bShowMouseCursor = false;
			}
		}
	}
	else
	{
		// Game isn't paused. Pause the game.
		UGameplayStatics::SetGamePaused(this, true);

		if(PauseWidgetClass)
		{
			PauseMenuWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), PauseWidgetClass));

			if(PauseMenuWidget)
			{
				PauseMenuWidget->AddToViewport();

				if(PlayerController && PlayerController->GetUsingMouse_Implementation())
				{
					PlayerController->SetShowMouseCursor(true);
				}
			}
		}
	}
}

void AProject_A_GameMode::PlayerLost()
{
	if(LossWidgetClass)
	{
		LossWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), LossWidgetClass));

		if(LossWidget)
		{
			LossWidget->AddToViewport();
		}
	}
	GameModeState = EGameModeState::EGMS_Loss;
}

void AProject_A_GameMode::PlayerWon()
{
	if(WinWidgetClass)
	{
		WinWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), WinWidgetClass));

		if(WinWidget)
		{
			WinWidget->AddToViewport();
		}
	}
	GameModeState = EGameModeState::EGMS_Win;
}

void AProject_A_GameMode::CreatePlayerWidget()
{
	if(PlayerWidgetClass)
	{
		PlayerWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), PlayerWidgetClass));

		if(PlayerWidget)
		{
			PlayerWidget->AddToViewport();
		}
	}
}

void AProject_A_GameMode::IncrementEnemies()
{
	NumOfEnemies++;
}

void AProject_A_GameMode::DecrementEnemies()
{
	if(NumOfEnemies == 0) return;
	
	NumOfEnemies--;

	if(NumOfEnemies == 0)
	{
		PlayerWon();
	}
}
