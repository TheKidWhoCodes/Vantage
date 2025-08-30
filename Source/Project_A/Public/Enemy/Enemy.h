// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterStates.h"
#include "Enemy.generated.h"

class UWidgetComponent;
class AAIController;
class UPawnSensingComponent;
class UBoxComponent;
class AEnemyPosition;

/**
 * 
 */
UCLASS()
class PROJECT_A_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemy();
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void Enemyheard();

	UFUNCTION(BlueprintCallable)
	void StartVisibilityTimer();

	UFUNCTION(BlueprintCallable)
	void ClearAllTimers();

protected:

	virtual void BeginPlay() override;
	void StartMoveTimer();
	void StartShooting();
	void StopShooting();
	void UpdatePosition(AEnemyPosition* NewPosition);
	void MoveToTarget(AActor* Target);
	void Engage(AActor* Target);
	bool IsInOnlyCombatPosition();
	bool NoValidCombatPosition();
	void FindNewPosition();
	bool PositionsAreOccupied();
	virtual void Die() override;;
	void SetInvisible();

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/* Effects */

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	USoundCue* AggroSound;
	
	/* Components */

	UPROPERTY(VisibleAnywhere)
	AAIController* EnemyController;

	UPROPERTY(EditAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;

private:
	
	bool PawnIsNotPlayer(APawn* SeenPawn);
	void Disengage();
	void StartDangerVisibilityTimer();
	void HideDangerWidget();
	void StartPawnSeenTimer();
	void PawnSeenTimerEnded();
	bool ShouldReact(APawn* OtherPawn);

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void NoiseHeard(APawn* OtherPawn, const FVector& Location, float Volume);
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Neutral;

	UPROPERTY(VisibleAnywhere)
	AActor* CombatTarget;

	float CapsuleHeight = 120.f;
	float CapsuleRadius = 50.f;

	const FVector BoxExtent = FVector(400.f, 400.f, 50.f);

	FTimerHandle VisibilityTimer;
	FTimerHandle InitialVisibilityTimer;
	
	FTimerHandle StartShootingTimer;
	FTimerHandle StopShootingTimer;
	FTimerHandle PawnSeenTimer;

	FTimerHandle DangerVisibilityTimer;
	
	bool bCanSeePlayer = false;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float PawnSeenTime = 2.f;

	UPROPERTY(EditAnywhere, Category = AI)
	bool bShouldReposition = true;

	UPROPERTY(VisibleAnywhere)
	bool bShouldShoot = false;

	/* Components */

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* DangerWidget;
	
	/* AI */

	UPROPERTY(VisibleInstanceOnly, Category = AI)
	TArray<AActor*> CombatPositions;

	UPROPERTY()
	AEnemyPosition* CurrentPosition;
	
	FTimerHandle MoveTimer;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MinMoveWaitTime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MaxMoveWaitTime = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	float SightRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	float VisionAngle = 45.f;

public:

	FORCEINLINE EEnemyState GetEnemyState() { return EnemyState; }

	UFUNCTION(BlueprintCallable)
	void SetWeaponVisibility(bool Visibility);
	
};
