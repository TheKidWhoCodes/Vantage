// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode/GameModeStates.h"
#include "Project_A_GameMode.generated.h"

class AProject_A_Controller;
/**
 * 
 */
UCLASS()
class PROJECT_A_API AProject_A_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	virtual void StartPlay() override;
	void PlayerLost();
	void IncrementEnemies();
	void DecrementEnemies();
	void CreatePlayerWidget();
	
	UFUNCTION(BlueprintCallable)
	void PauseGame();

	UFUNCTION(BlueprintImplementableEvent)
	void CreateStartWidget(const FString& LevelName, const FString& LevelTime);

private:

	void PlayerWon();
	
	bool CanUnpause = true;
	
	EGameModeState GameModeState = EGameModeState::EGMS_Running;

	AProject_A_Controller* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = Game)
	int32 NumOfEnemies;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UPROPERTY(VisibleInstanceOnly)
	UUserWidget* PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LossWidgetClass;

	UPROPERTY(VisibleInstanceOnly)
	UUserWidget* LossWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> WinWidgetClass;

	UPROPERTY(VisibleInstanceOnly)
	UUserWidget* WinWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerWidgetClass;

	UPROPERTY(VisibleInstanceOnly)
	UUserWidget* PlayerWidget;

public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCanUnpause(const bool InCanUnpause) { CanUnpause = InCanUnpause; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GameIsRunning() const { return GameModeState == EGameModeState::EGMS_Running; }
};
