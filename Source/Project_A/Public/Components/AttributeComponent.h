// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_A_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ReceiveDamage(float Damage);
	float AddHealth(float Amount);

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere)
	float Health = 100;

	UPROPERTY(VisibleAnywhere)
	float MaxHealth = 100;

public:

	FORCEINLINE float GetHealthPercentage() { return Health / MaxHealth; }
	FORCEINLINE bool IsAlive() { return Health > 0; }
	float GetMissingHealthPercentage();
};
