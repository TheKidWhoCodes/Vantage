// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Throwable.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class USoundCue;
class UNiagaraSystem;

UCLASS()
class PROJECT_A_API AThrowable : public AActor
{
	GENERATED_BODY()
	
public:
	
	AThrowable();
	virtual void Tick(float DeltaTime) override;
	void Throw();
	

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()    
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/* Weapon Properties */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponProperties)
	float Damage = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponProperties)
	float Radius = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponProperties)
	float Timer = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponProperties)
	float Speed = 1000.f;

	/* Effects */

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	UNiagaraSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	USoundWave* BounceSound;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	USoundWave* ThrowSound;

	/* Components */

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* Sphere;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;

private:

	void StartFuseTimer();
	void Explode();
	void FuseTimerEnded();

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	FTimerHandle FuseTimer;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetRadius() { return Radius; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetDamage() { return Damage; }

};
