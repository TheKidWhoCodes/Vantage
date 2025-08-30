// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyPosition.h"
#include "Components/SphereComponent.h"

AEnemyPosition::AEnemyPosition()
{
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

void AEnemyPosition::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(TEXT("EnemyPosition"));
}
