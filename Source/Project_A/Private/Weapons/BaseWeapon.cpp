// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "Characters/BaseCharacter.h"
#include "Sound/SoundCue.h"

#define COLLISION_SHOOTING	ECC_GameTraceChannel1	// ??????

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(Mesh);
	StartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StartPoint"));
	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	LaserStartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LaserStartPoint"));
	LaserEndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LaserEndPoint"));
	StartPoint->SetupAttachment(GetRootComponent());
	EndPoint->SetupAttachment(GetRootComponent());
	LaserStartPoint->SetupAttachment(GetRootComponent());
	LaserEndPoint->SetupAttachment(GetRootComponent());

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void ABaseWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// DrawDebugBox(
	// 	GetWorld(),
	// 	EndPoint->GetComponentLocation(),
	// 	FVector(5.f),
	// 	FColor::Red
	// 	);

	UpdateLaser();
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Add offset to the weapon's EndPoint
	EndPoint->SetWorldLocation(StartPoint->GetComponentLocation());
	EndPoint->AddLocalOffset(FVector(0.f, WeaponRange, 0.f));

	// Add offset to the LaserSight's EndPoint
	LaserEndPoint->SetWorldLocation(LaserStartPoint->GetComponentLocation());
	LaserEndPoint->AddLocalOffset(FVector(0.f, LaserRange, 0.f));
	
	BulletsInMag = MagSize;
	BoxTraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2);
}

void ABaseWeapon::PerformTraces()
{
	if(Target)
	{
		BulletTraceWithTarget();
	}
	else
	{
		BulletTraceWithoutTarget(true);
	}
}

void ABaseWeapon::Fire()
{
	if(!bCanShoot) return;
	
	// If weapon is empty, call Reload() and return
	if(BulletsInMag <= 0)
	{
		if(Character && !Character->IsReloading())
		{
			Reload();
			return;
		}
		return;
	}
	
	if(Character && Character->IsReloading())
	{
		bCanShoot = true;

		if(Character)
		{
			Character->SetIsNeutral();
			Character->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, ReloadMontage);
		}
	}

	// Sweep box
	ScanForTarget();

	// Return if Target is teammate
	if(Target && ActorIsSameType(Target))
	{
		return;
	}
	
	// Decrement bullets from magazine
	BulletsInMag--;
	
	// Perform bullet trace and damage Target
	PerformTraces();

	PlayFireMontage();
	StartShootTimer();

	if(Character)
	{
		Character->MakeShootingNoise(Character);

		if(Character->ActorHasTag(TEXT("Player")))
		{
			Character->PlayEffects();
		}
	}
}

void ABaseWeapon::ScanForTarget()
{
	EndPoint->SetRelativeLocation(InitialEndPointLocation);
	CalculateTraceBoxExtent();
	
	// DrawDebugBox(GetWorld(), StartPoint->GetComponentLocation(), TraceBoxExtent, FColor::Red);
	GetWorld()->SweepSingleByChannel(
		ScanHitResult,
		StartPoint->GetComponentLocation(),
		EndPoint->GetComponentLocation(),
		GetActorRotation().Quaternion(),
		ECC_GameTraceChannel1,
		FCollisionShape::MakeBox(TraceBoxExtent)
		);
	// DrawDebugBox(GetWorld(), ScanHitResult.ImpactPoint, TraceBoxExtent, FColor::Red);

	if(ScanHitResult.GetActor() && ScanHitResult.GetActor()->ActorHasTag(TEXT("Pawn")))
	{
		Target = ScanHitResult.GetActor();
	}
	else
	{
		Target = nullptr;
	}
}

void ABaseWeapon::BulletTraceWithTarget()
{
	CalculateAccuracy();
	
	// Use the weapon's endpoint with the Target's Z location
	const FVector EndPointOffset = FVector(
		EndPoint->GetComponentLocation().X,
		EndPoint->GetComponentLocation().Y,
		Target->GetTargetLocation(this).Z
		);
	
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		StartPoint->GetComponentLocation(),
		EndPointOffset,
		BulletBoxSize,
		GetActorRotation(),
		BoxTraceChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BulletHitResult,
		true
		);
	
	// Check if GetActor() has tag Pawn, if not then don't apply damage and just play bullet effects without vertical offset.
	if(BulletHitResult.GetActor() && BulletHitResult.GetActor()->ActorHasTag(TEXT("Pawn")))
	{
		// Damage target
		UGameplayStatics::ApplyDamage(
			BulletHitResult.GetActor(),
			WeaponDamage * EnemyDamageMultiplier,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
		
		SpawnBulletTrail(BulletHitResult.ImpactPoint);

		if(BulletHitResult.GetActor() && !BulletHitResult.GetActor()->IsHidden())
		{
			SpawnParticles(BloodParticles, BulletHitResult.ImpactPoint);
		}
	}
	else
	{
		BulletTraceWithoutTarget(false);
	}
}

void ABaseWeapon::BulletTraceWithoutTarget(bool bShouldUpdateAccuracy)
{
	if(bShouldUpdateAccuracy)
	{
		CalculateAccuracy();
	}
	
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		StartPoint->GetComponentLocation(),
		EndPoint->GetComponentLocation(),
		BulletBoxSize,
		GetActorRotation(),
		BoxTraceChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BulletHitResult,
		true
		);

	if(BulletHitResult.GetActor())
	{
		if(BulletHitResult.GetActor()->ActorHasTag("Pawn"))
		{
			SpawnBulletTrail(BulletHitResult.ImpactPoint);
		}
		else
		{
			// Spawn trail to the ImpactPoint
			SpawnBulletTrail(BulletHitResult.ImpactPoint);
			SpawnParticles(BulletImpactParticles, BulletHitResult.ImpactPoint);
		}
	}
	else
	{
		// Spawn trail to the range of the gun
		SpawnBulletTrail(EndPoint->GetComponentLocation());
	}
}

void ABaseWeapon::SetVisibility(bool bSetVisible)
{
	if(bSetVisible)
	{
		Mesh->SetVisibility(true, true);
	}
	else
	{
		Mesh->SetVisibility(false, true);
	}
}

void ABaseWeapon::Reload()
{
	// Return if magazine is full
	if((BulletsInMag == MagSize) || HeldAmmo == 0) return;

	if(Character)
	{
		Character->SetIsReloading();
	}
	
	PlayReloadMontage();
}

void ABaseWeapon::ShellInserted()
{
	// Can load shell
	if(HeldAmmo > 0 && BulletsInMag != MagSize)
	{
		BulletsInMag++;
		HeldAmmo--;
	}

	FName NextSection;

	if(BulletsInMag == MagSize)
	{
		NextSection = "ReloadEnd";
	}
	else
	{
		NextSection = "InsertShell";
	}
	
	if(Character)
	{
		Character->GetMesh()->GetAnimInstance()->Montage_SetNextSection(
			Character->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(ReloadMontage),
			NextSection,
			ReloadMontage
			);
	}
}

void ABaseWeapon::ReloadFinished()
{
	// Check how many bullets are missing from the magazine
	const int16 MissingBullets = MagSize - BulletsInMag;

	// We have enough held ammo to completely refill the magazine
	if(HeldAmmo >= MissingBullets)
	{
		BulletsInMag += MissingBullets;
		HeldAmmo -= MissingBullets;
	}
	// We do not have enough held ammo to completely refill the magazine, so we just put whatever we hold into the magazine
	else
	{
		BulletsInMag += HeldAmmo;
		HeldAmmo = 0;
	}

	if(Character)
	{
		Character->SetIsNeutral();
	}
}

void ABaseWeapon::PlayFireMontage() const
{
	if(Character && FireMontage)
	{
		Character->GetMesh()->GetAnimInstance()->Montage_Play(FireMontage);
	}
}

void ABaseWeapon::PlayReloadMontage() const
{
	if(Character && ReloadMontage)
	{
		Character->GetMesh()->GetAnimInstance()->Montage_Play(ReloadMontage);
	}
}

void ABaseWeapon::PlayEquipSound()
{
	if(EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation(), GetActorRotation());
	}
}

bool ABaseWeapon::ActorIsSameType(const AActor* HitActor)
{
	if(HitActor)
	{
		return GetOwner()->ActorHasTag(TEXT("Enemy")) && HitActor->ActorHasTag(TEXT("Enemy"));
	}
	return false;
}

void ABaseWeapon::SpawnBulletTrail(const FVector& TrailEnd)
{
	if(BulletTrail)
	{
		UNiagaraComponent* SpawnedComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			BulletTrail,
			StartPoint->GetComponentLocation(),
			GetActorRotation()
		);

		// Set end point of trail
		if(SpawnedComponent)
		{
			SpawnedComponent->SetNiagaraVariableVec3(FString("Beam End"), TrailEnd);
		}
	}
}

void ABaseWeapon::SpawnLaser()
{
	if(LaserSight)
	{
		SpawnedLaser = UNiagaraFunctionLibrary::SpawnSystemAttached(
			LaserSight,
			LaserStartPoint,
			NAME_None,
			LaserStartPoint->GetComponentLocation(),
			FRotator(0.f),
			EAttachLocation::KeepWorldPosition,
			false
		);

		if(SpawnedLaser)
		{
			SpawnedLaser->SetNiagaraVariableVec3(FString("LaserEnd"), FVector(LaserRange, 0.f, 0.f));
		}

		if(WeaponCategory == EWeaponCategory::EWC_Secondary)
		{
			SpawnedLaser->SetVisibility(false, true);
		}
	}
}

void ABaseWeapon::UpdateLaser()
{
	if(SpawnedLaser)
	{
		UKismetSystemLibrary::LineTraceSingle(
			this,
			LaserStartPoint->GetComponentLocation(),
			LaserEndPoint->GetComponentLocation(),
			TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			LaserHitResult,
			true
			);
		
		if(LaserHitResult.GetActor())
		{
			SpawnedLaser->SetNiagaraVariableVec3(FString("LaserEnd"), FVector(LaserHitResult.Distance, 0.f, 0.f));
		}
		else
		{
			SpawnedLaser->SetNiagaraVariableVec3(FString("LaserEnd"), FVector(LaserRange, 0.f, 0.f));
		}
	}
}

void ABaseWeapon::StartShootTimer()
{
	bCanShoot = false;
	GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &ABaseWeapon::ShootTimerEnded, FireRate);
}

void ABaseWeapon::ShootTimerEnded()
{
	bCanShoot = true;
	GetWorld()->GetTimerManager().ClearTimer(ShootTimer);
}

void ABaseWeapon::SetMontages(UAnimMontage* FireMontageIn, UAnimMontage* ReloadMontageIn)
{
	if(FireMontageIn)
	{
		FireMontage = FireMontageIn;
	}
	if(ReloadMontageIn)
	{
		ReloadMontage = ReloadMontageIn;
	}
}

void ABaseWeapon::InitializeWeapon()
{
	InitializeCharacter();
	ActorsToIgnore.AddUnique(GetOwner());

	if(Character && Character->ActorHasTag("Player"))
	{
		EnemySpreadMultiplier = 1.f;
		EnemyDamageMultiplier = 1.f;
		SpawnLaser();
	}
}

void ABaseWeapon::InitializeCharacter()
{
	Character = Cast<ABaseCharacter>(GetOwner());
}

void ABaseWeapon::SpawnParticles(UNiagaraSystem* ParticlesToPlay, FVector Location)
{
	if(ParticlesToPlay)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ParticlesToPlay,
			Location,
			FRotator(FMath::Rand())
			);
	}
}

void ABaseWeapon::CalculateAccuracy()
{
	if(Character && Character->IsMoving())
	{
		// Calculate weapon spread for when character is in motion
		// Reset Endpoint to it's initial location
		EndPoint->SetRelativeLocation(InitialEndPointLocation);
		const float Spread = FMath::RandRange(MinMovingSpreadOffset * EnemySpreadMultiplier, MaxMovingSpreadOffset * EnemySpreadMultiplier);
		EndPoint->AddLocalOffset(FVector(Spread, 0.f, 0.f));
	}
	else
	{
		// Calculate weapon spread for when character is standing still
		// Reset Endpoint to it's initial location to calculate spread
		EndPoint->SetRelativeLocation(InitialEndPointLocation);
		const float Spread = FMath::RandRange(MinSpreadOffset * EnemySpreadMultiplier, MaxSpreadOffset * EnemySpreadMultiplier);
		EndPoint->AddLocalOffset(FVector(Spread, 0.f, 0.f));
	}
}

void ABaseWeapon::CalculateTraceBoxExtent()
{
	if(Character && Character->IsMoving())
	{
		TraceBoxExtent = FVector(MaxMovingSpreadOffset - MinMovingSpreadOffset, 1.f, BoxHeight);
	}
	else
	{
		TraceBoxExtent = FVector(MaxSpreadOffset - MinSpreadOffset, 1.f, BoxHeight);
	}
}

void ABaseWeapon::GetWeaponXYOffset(float& XOffset, float& YOffset)
{
	XOffset = CursorXOffset;
	YOffset = CursorYOffset;
}
