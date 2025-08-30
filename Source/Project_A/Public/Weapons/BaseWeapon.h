// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponData.h"
#include "BaseWeapon.generated.h"

class ABaseCharacter;
class UNiagaraSystem;
class USoundCue;

UCLASS()
class PROJECT_A_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	ABaseWeapon();
	void Fire();
	bool ActorIsSameType(const AActor* HitActor);
	virtual void Tick(float DeltaSeconds) override;
	void SetVisibility(bool bSetVisible);
	void Reload();
	virtual void ReloadFinished();
	void PlayFireMontage() const;
	void PlayReloadMontage() const;
	void PlayEquipSound();
	void ShellInserted();
	void InitializeWeapon();

protected:

	virtual void BeginPlay() override;
	virtual void PerformTraces();
	void ScanForTarget();
	void BulletTraceWithTarget();
	void BulletTraceWithoutTarget(bool bShouldUpdateAccuracy);
	void StartShootTimer();
	void ShootTimerEnded();

	UFUNCTION(BlueprintCallable)
	void SetMontages(UAnimMontage* FireMontageIn, UAnimMontage* ReloadMontageIn);

	UPROPERTY(VisibleInstanceOnly, Category = WeaponProperties)
	AActor* Target;

	UPROPERTY(VisibleAnywhere)
	ABaseCharacter* Character;

	bool bCanShoot = true;
	float BoxHeight = 200.f;

	FHitResult ScanHitResult;
	FHitResult BulletHitResult;
	FHitResult LaserHitResult;
	
	FVector BulletBoxSize = FVector(5.f);

	/* Unique Weapon Properties */

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	UForceFeedbackEffect* VibrationEffect;

	UPROPERTY(VisibleAnywhere, Category = WeaponProperties)
	FVector TraceBoxExtent = FVector(0.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponProperties)
	float WeaponRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponProperties)
	float LaserRange = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponProperties)
	float WeaponDamage = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponProperties)
	float FireRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponProperties)
	int32 MagSize = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WeaponProperties)
	int32 BulletsInMag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponProperties)
	int32 HeldAmmo = 240;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float MinSpreadOffset = -10.f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float MaxSpreadOffset = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float MinMovingSpreadOffset = -10.f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float MaxMovingSpreadOffset = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float EnemySpreadMultiplier = 1.3f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float EnemyDamageMultiplier = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float CharacterWalkSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float SpringArmLength = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float CursorXOffset = 12.f;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float CursorYOffset = 13.f;

	/* Effects */

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	UNiagaraSystem* BulletTrail;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	UNiagaraSystem* LaserSight;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	UNiagaraSystem* BloodParticles;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	UNiagaraSystem* BulletImpactParticles;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	USoundCue* EquipSound;

	/* Components */
	
	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	USceneComponent* StartPoint;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	USceneComponent* EndPoint;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	USceneComponent* LaserStartPoint;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	USceneComponent* LaserEndPoint;

private:

	void SpawnBulletTrail(const FVector& TrailEnd);
	void SpawnLaser();
	void UpdateLaser();
	void InitializeCharacter();
	void SpawnParticles(UNiagaraSystem* ParticlesToPlay, FVector Location);
	void CalculateAccuracy();
	void CalculateTraceBoxExtent();

	ETraceTypeQuery BoxTraceChannel;
	FVector InitialEndPointLocation;
	
	FTimerHandle ShootTimer;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
	FName EnemyCheck = "Enemy";

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* SpawnedLaser;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess), Category = Effects)
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = WeaponProperties)
	EWeaponCategory WeaponCategory;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess), Category = WeaponProperties)
	EWeaponFireType FireType;

	/* Montages */

	UPROPERTY(VisibleAnywhere, Category = Animation)
	UAnimMontage* FireMontage;

	UPROPERTY(VisibleAnywhere, Category = Animation)
	UAnimMontage* ReloadMontage;

public:
		
	void GetWeaponXYOffset(float& XOffset, float& YOffset);
	FORCEINLINE USceneComponent* GetEndPoint() const { return EndPoint; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE EWeaponFireType GetFireType() const { return FireType; }
	FORCEINLINE TSubclassOf<UCameraShakeBase> GetCameraShake() const { return CameraShake; }
	FORCEINLINE UForceFeedbackEffect* GetVibrationEffect() const { return VibrationEffect; }
	FORCEINLINE void SetInitialEndPointRelativeLocation(FVector Location) { InitialEndPointLocation = Location; }
	FORCEINLINE float GetWalkSpeed() const { return CharacterWalkSpeed; }
	FORCEINLINE float GetSpringArmLength() const { return SpringArmLength; }
};
