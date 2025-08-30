// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameInstance/Project_A_GameInstance.h"
#include "Weapons/BaseWeapon.h"
#include "Weapons/PrimaryWeapons/PrimaryWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "GameMode/Project_A_GameMode.h"

ABaseCharacter::ABaseCharacter()
{
	// TODO: Disable tick
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	PrimaryEndPointSocket = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponEndPointSocket"));
	PrimaryEndPointSocket->SetupAttachment(GetRootComponent());
	
	NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	World = GetWorld();
	Tags.Add(TEXT("Pawn"));

	GameMode = Cast<AProject_A_GameMode>(UGameplayStatics::GetGameMode(this));
	GameInstance = Cast<UProject_A_GameInstance>(UGameplayStatics::GetGameInstance(this));
	
	// Spawn and attach weapon to socket
	SpawnWeapon();
}

void ABaseCharacter::SpawnWeapon()
{
	if(World && PrimaryWeaponClass)
	{
		PrimaryWeapon = World->SpawnActor<APrimaryWeapon>(PrimaryWeaponClass, GetMesh()->GetSocketTransform(FName("WeaponSocket")));

		if(PrimaryWeapon)
		{
			PrimaryWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponSocket"));
			PrimaryEndPointSocket->SetWorldLocation(PrimaryWeapon->GetEndPoint()->GetComponentLocation());
			PrimaryWeapon->GetEndPoint()->AttachToComponent(PrimaryEndPointSocket, FAttachmentTransformRules::KeepWorldTransform);

			// Set Weapon's InitialEndPointLocation to Endpoint's initial location
			PrimaryWeapon->SetInitialEndPointRelativeLocation(PrimaryWeapon->GetEndPoint()->GetRelativeLocation());
			PrimaryWeapon->SetOwner(this);
		
			EquippedWeapon = PrimaryWeapon;
			SetWalkSpeed();
		}
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (GetActorForwardVector() * FVector(100.f, 100.f, 100.f)), FColor::Red, false, 0.f, 0, 5.f);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	if(GameMode && !GameMode->GameIsRunning()) return 0.f;
	
	if(Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
		PlayHitSounds(DamageCauser);

		if(!Attributes->IsAlive())
		{
			Die();
		}
	}

	return DamageAmount;
}

void ABaseCharacter::Shoot()
{
	if(EquippedWeapon && CanShoot())
	{
		return EquippedWeapon->Fire();
	}
}

bool ABaseCharacter::ActorIsSameType(const AActor* OtherActor)
{
	if(OtherActor)
	{
		return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
	}
	return false;
}

void ABaseCharacter::StartLevel()
{
	if(PrimaryWeapon)
	{
		PrimaryWeapon->InitializeWeapon();
	}
}

void ABaseCharacter::SetWalkSpeed()
{
	if(EquippedWeapon)
	{
		GetCharacterMovement()->MaxWalkSpeed = EquippedWeapon->GetWalkSpeed();
	}
}

void ABaseCharacter::FinishedReloading()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->ReloadFinished();
	}
}

void ABaseCharacter::InsertShell()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->ShellInserted();
	}
}

bool ABaseCharacter::CanShoot()
{
	return CharacterState != ECharacterState::ECS_Dead &&
		CharacterState != ECharacterState::ECS_UsingGear;
}

void ABaseCharacter::Die()
{
	CharacterState = ECharacterState::ECS_Dead;
	Tags.Add(TEXT("Dead"));
	GetController()->StopMovement();
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
}

void ABaseCharacter::MakeShootingNoise(APawn* OwningPawn)
{
	MakeNoise(1.f, OwningPawn, GetActorLocation(), 0, FName("Gunfire"));
}

void ABaseCharacter::MakeFootstepNoise()
{
	MakeNoise(0.5f, this, GetActorLocation(), 0, FName("Footstep"));
}

void ABaseCharacter::PlayHitSounds(AActor* DamageCauser)
{
	if(HitSound && GruntSound && !ActorIsSameType(DamageCauser))
	{
		UGameplayStatics::PlaySound2D(this, HitSound);
		UGameplayStatics::PlaySoundAtLocation(this, GruntSound, GetActorLocation(), GetActorRotation());
	}
}

float ABaseCharacter::GetMissingHealth()
{
	if(Attributes) return Attributes->GetMissingHealthPercentage();
	return 0.f;
}

void ABaseCharacter::PlayEffects()
{
	return;
}
