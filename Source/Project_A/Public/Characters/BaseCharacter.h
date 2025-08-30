// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterStates.h"
#include "BaseCharacter.generated.h"

class UProject_A_GameInstance;
class AProject_A_GameMode;
class ABaseWeapon;
class UAttributeComponent;
class APrimaryWeapon;
class USoundCue;

UCLASS()
class PROJECT_A_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Die();
	virtual void PlayEffects();

	UFUNCTION(BlueprintCallable)
	void MakeShootingNoise(APawn* OwningPawn);

	UFUNCTION(BlueprintCallable)
	void MakeFootstepNoise();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMissingHealth();

protected:

	virtual void BeginPlay() override;
	void SpawnWeapon();
	virtual void Shoot();
	bool ActorIsSameType(const AActor* OtherActor);
	virtual void StartLevel();
	void SetWalkSpeed();

	UFUNCTION(BlueprintCallable)
	void FinishedReloading();

	UFUNCTION(BlueprintCallable)
	void InsertShell();

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	AProject_A_GameMode* GameMode;

	UPROPERTY()
	UProject_A_GameInstance* GameInstance;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<APrimaryWeapon> PrimaryWeaponClass;
	
	UPROPERTY(BlueprintReadOnly, Category = Weapon)
	APrimaryWeapon* PrimaryWeapon;

	UPROPERTY(BlueprintReadOnly, Category = Weapon)
	ABaseWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = States)
	ECharacterState CharacterState = ECharacterState::ECS_Neutral;

	/* Effects */

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	USoundCue* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	USoundCue* GruntSound;

	/* Montages */

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;
	
	/* Components */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	USceneComponent* PrimaryEndPointSocket;

	UPROPERTY(VisibleAnywhere, Category = Attributes)
	UAttributeComponent* Attributes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPawnNoiseEmitterComponent* NoiseEmitter;

private:

	bool CanShoot();
	void PlayHitSounds(AActor* DamageCauser);
	
public:
	
	/* States */
	FORCEINLINE void SetIsReloading() { CharacterState = ECharacterState::ECS_Reloading; }
	FORCEINLINE void SetIsNeutral() { CharacterState = ECharacterState::ECS_Neutral; }
	FORCEINLINE bool IsMoving() const { return GetVelocity().Size() > 0.f; }
	FORCEINLINE bool IsReloading() const { return CharacterState == ECharacterState::ECS_Reloading; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() const { return CharacterState == ECharacterState::ECS_Dead; }
};
