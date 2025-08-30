// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ABaseWeapon;
class ASecondaryWeapon;
class AThrowable;

/**
 * 
 */
UCLASS()
class PROJECT_A_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PlayEffects() override;

protected:

	virtual void BeginPlay() override;
	virtual void Die() override;
	virtual void StartLevel() override;
	virtual void Shoot() override;

	FVector2D CursorPosition = FVector2D(0.f);
	FVector2D ViewPortRes = FVector2D(0.f);

	UPROPERTY(EditAnywhere, Category = PlayerInput)
	float AimInterpSpeed = 10;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<ASecondaryWeapon> SecondaryWeaponClass;
	
	UPROPERTY(BlueprintReadOnly, Category = Weapon)
	ASecondaryWeapon* SecondaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AThrowable> GearClass;
	
	UPROPERTY(BlueprintReadOnly, Category = Weapon)
	AThrowable* Gear;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	TSubclassOf<UUserWidget> CursorWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = PlayerInput)
	UUserWidget* CursorWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerInput)
	float CursorSensitivity = 80.f;
	
private:

	void Move(const FInputActionValue& Value);
	void LookController(const FInputActionValue& Value);
	void LookMouse(const FInputActionValue& Value);
	void SwapWeapon();
	void SpawnSecondaryWeapon();
	void ReloadWeapon();
	void ReadyGear();
	void PlayThrowAnimation();
	bool IsThrowingGear();
	void PauseKeyPressed();
	bool CanSwapWeapon();
	void StartRecoveryTimer();
	void RecoveryTimerFinished();
	void SetSpringArmLength();
	
	UFUNCTION(BlueprintCallable)
	void ThrowGear();

	UFUNCTION()
	void NoiseHeard(APawn* OtherPawn, const FVector& Location, float Volume);

	bool bShouldHeal = false;
	float CurrentHealth;

	float WeaponCursorXOffset = 0.f;
	float WeaponCursorYOffset = 0.f;
	
	FVector CameraInitialLocation;
	FVector2D LookVector2D = FVector2D::Zero();
	
	FTimerHandle StartTimer;
	FTimerHandle RecoveryTimer;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	float RecoveryTime = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	float RecoveryRate = 20.f;

	UPROPERTY()
	class AProject_A_Controller* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 HeldGear = 2;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	float LevelStartTime = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	FVector MouseCameraOffsetDistance = FVector(0.2f);

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	FVector ControllerCameraOffsetDistance = FVector(100.f);

	/* Montages */

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* GearMontage;
	
	/* Components */
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	USceneComponent* SecondaryEndPointSocket;

	UPROPERTY(EditAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	/* Input */
	
	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* LookControllerAction;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* LookMouseAction;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* ShootAction;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* SwapAction;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* ReloadAction;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* ReadyGearAction;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* ThrowGearAction;

	UPROPERTY(EditDefaultsOnly, Category = PlayerInput)
	UInputAction* PauseAction;

public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetPrimaryWeaponClass(TSubclassOf<APrimaryWeapon> WeaponClass) { PrimaryWeaponClass = WeaponClass; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetSecondaryWeaponClass(TSubclassOf<ASecondaryWeapon> WeaponClass) { SecondaryWeaponClass = WeaponClass; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetGearClass(TSubclassOf<AThrowable> inGearClass) { GearClass = inGearClass; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetSensitivity(float Sensitivity) { CursorSensitivity = Sensitivity; }

	FORCEINLINE bool HasGear() const { return HeldGear > 0; }
};
