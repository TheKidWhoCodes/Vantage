// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Throwable.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraFunctionLibrary.h"

AThrowable::AThrowable()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	SetRootComponent(Sphere);
	Mesh->SetupAttachment(GetRootComponent());
	
	Sphere->SetSimulatePhysics(false);
	Sphere->SetUseCCD(true);
	Sphere->SetLinearDamping(0.5f);
	Sphere->SetAngularDamping(1.f);
	Sphere->SetCollisionObjectType(ECC_Destructible);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Sphere->SetCollisionResponseToAllChannels(ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	Sphere->SetSphereRadius(10.f);
}

void AThrowable::BeginPlay()
{
	Super::BeginPlay();

	ActorsToIgnore.AddUnique(this);
	Sphere->OnComponentHit.AddDynamic(this, &AThrowable::OnHit);
	
	StartFuseTimer();
}

void AThrowable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(BounceSound && NormalImpulse.Length() > 500.f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation(), GetActorRotation());
	}
}

void AThrowable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AThrowable::Throw()
{
	if(Sphere && ThrowSound)
	{
		Sphere->SetSimulatePhysics(true);
		Sphere->SetPhysicsLinearVelocity(GetActorForwardVector() * FVector(Speed));
		UGameplayStatics::PlaySoundAtLocation(this, ThrowSound, GetActorLocation(), GetActorRotation());
	}
}

void AThrowable::StartFuseTimer()
{
	GetWorld()->GetTimerManager().SetTimer(FuseTimer, this, &AThrowable::FuseTimerEnded, Timer);
}

void AThrowable::Explode()
{
	UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), Radius, UDamageType::StaticClass(), ActorsToIgnore);
}

void AThrowable::FuseTimerEnded()
{
	Explode();

	if(ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), GetActorRotation());
	}
	if(ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation(), GetActorRotation());
	}

	Destroy();
}

