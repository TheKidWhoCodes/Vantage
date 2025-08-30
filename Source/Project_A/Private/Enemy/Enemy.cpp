// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/EnemyPosition.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/Project_A_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/PawnSensingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Weapons/BaseWeapon.h"

AEnemy::AEnemy()
{
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	Tags.Add(TEXT("Enemy"));
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHeight);
	GetCapsuleComponent()->SetCapsuleRadius(CapsuleRadius);
	
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensingComponent->SightRadius = SightRadius;
	PawnSensingComponent->SetPeripheralVisionAngle(VisionAngle);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetBoxExtent(BoxExtent);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	BoxComponent->SetCollisionObjectType(ECC_WorldStatic);

	DangerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("FootstepsWidget"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);

	GetCharacterMovement()->GravityScale = 0.f;
}

void AEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(EnemyController && CombatTarget)
	{
		const FRotator Direction = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
		
		// TODO: EnemyController->SetControlRotation(FMath::RInterpTo(GetControlRotation(), Direction, World->GetDeltaSeconds(), 15.f));
		EnemyController->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation()));
		SetActorRotation(FRotator(GetActorRotation().Pitch, EnemyController->GetControlRotation().Yaw, GetActorRotation().Pitch));
	}
	
	if(bShouldShoot && bCanSeePlayer)
	{
		Shoot();
	}

	if(CombatTarget && CombatTarget->ActorHasTag(TEXT("Dead")))
	{
		Disengage();
	}

	if(DangerWidget)
	{
		DangerWidget->SetWorldLocation(GetActorLocation());
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AAIController>(GetController());

	if(World)
	{
		if(GameMode)
		{
			GameMode->IncrementEnemies();
		}
	}
	
	if(PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &AEnemy::NoiseHeard);
	}

	if(!bShouldReposition)
	{
		BoxComponent->DestroyComponent();
	}
	
	if(BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnBoxOverlap);
		BoxComponent->GetOverlappingActors(CombatPositions, AEnemyPosition::StaticClass());
	}

	StartLevel();
	GetWorld()->GetTimerManager().SetTimer(InitialVisibilityTimer, this, &AEnemy::SetInvisible, 0.1f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(!CombatTarget && DamageCauser)
	{
		Engage(DamageCauser->GetOwner());
	}
	else
	{
		EnemyState = EEnemyState::EES_Engaged;
	}

	return DamageAmount;
}

void AEnemy::Enemyheard()
{
	if(GetMesh()->IsVisible())
	{
		HideDangerWidget();
		return;
	}

	if(DangerWidget && DangerWidget->GetWidget())
	{
		DangerWidget->GetWidget()->SetVisibility(ESlateVisibility::Visible);
	}
	
	StartDangerVisibilityTimer();
}

void AEnemy::StartVisibilityTimer()
{
	GetWorld()->GetTimerManager().SetTimer(VisibilityTimer, this, &AEnemy::SetInvisible, 0.5f);
}

void AEnemy::ClearAllTimers()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AEnemy::StartMoveTimer()
{
	const float MoveWaitTime = FMath::RandRange(MinMoveWaitTime, MaxMoveWaitTime);
	
	GetWorld()->GetTimerManager().SetTimer(MoveTimer, this, &AEnemy::FindNewPosition, MoveWaitTime);
}

void AEnemy::StartShooting()
{
	bShouldShoot = true;
	const float ShootingTime = FMath::RandRange(0.5f, 1.5f);
	
	GetWorld()->GetTimerManager().SetTimer(StartShootingTimer, this, &AEnemy::StopShooting, ShootingTime);
}

void AEnemy::StopShooting()
{
	bShouldShoot = false;

	if(CombatTarget->ActorHasTag(TEXT("Dead")))
	{
		return;
	}
	
	const float WaitTime = FMath::RandRange(1.f, 2.5f);

	GetWorld()->GetTimerManager().SetTimer(StopShootingTimer, this, &AEnemy::StartShooting, WaitTime);
}

void AEnemy::UpdatePosition(AEnemyPosition* NewPosition)
{
	if(NewPosition)
	{
		NewPosition->bIsOccupied = true;

		if(CurrentPosition)
		{
			CurrentPosition->bIsOccupied = false;
			CurrentPosition = NewPosition;
		}
		else
		{
			CurrentPosition = NewPosition;
		}
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	AEnemyPosition* NewPosition = Cast<AEnemyPosition>(Target);

	if(NewPosition && NewPosition->bIsOccupied)
	{
		FindNewPosition();
		return;
	}
	
	if(NewPosition && EnemyController)
	{
		UpdatePosition(NewPosition);
		
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(50.f);
		EnemyController->MoveTo(MoveRequest);
	}

	StartMoveTimer();
}

void AEnemy::Engage(AActor* Target)
{
	if(IsDead() || Target->ActorHasTag(TEXT("Enemy")) || CombatTarget) return;

	if(EnemyState == EEnemyState::EES_Neutral && AggroSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AggroSound, GetActorLocation(), GetActorRotation());
	}

	EnemyState = EEnemyState::EES_Engaged;
	CombatTarget = Target;
	FindNewPosition();
	StartShooting();
}

bool AEnemy::IsInOnlyCombatPosition()
{
	return (CombatPositions.Num() == 1 && CurrentPosition);
}

bool AEnemy::NoValidCombatPosition()
{
	return CombatPositions.IsEmpty() || IsInOnlyCombatPosition() || PositionsAreOccupied();
}

void AEnemy::FindNewPosition()
{
	if(NoValidCombatPosition() || IsDead()) return;
		
	int32 NumOfPositions = CombatPositions.Num();
	int32 Selection = FMath::RandRange(0, NumOfPositions - 1);
	AActor* Position = CombatPositions[Selection];

	if(Position)
	{
		MoveToTarget(Position);
	}
}

void AEnemy::Die()
{
	Super::Die();
	Disengage();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

	if(PawnSensingComponent)
	{
		PawnSensingComponent->DestroyComponent();
	}

	if(World)
	{
		if(GameMode)
		{
			GameMode->DecrementEnemies();
		}
	}

	if(DangerWidget && DangerWidget->GetWidget())
	{
		DangerWidget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool AEnemy::PositionsAreOccupied()
{
	for(AActor* Actor : CombatPositions)
	{
		AEnemyPosition* Position = Cast<AEnemyPosition>(Actor);

		if(!Position->bIsOccupied)
		{
			return false;
		}
	}
	return true;
}

void AEnemy::SetInvisible()
{
	if(IsDead())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}
	
	GetMesh()->SetVisibility(false, true);
}

void AEnemy::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                          int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->ActorHasTag(TEXT("EnemyPosition"))) return;
	
	CombatPositions.AddUnique(OtherActor);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if(!ShouldReact(SeenPawn)) return;
	
	bCanSeePlayer = true;
	StartPawnSeenTimer();
	Engage(SeenPawn);
}

void AEnemy::NoiseHeard(APawn* OtherPawn, const FVector& Location, float Volume)
{
	if(!ShouldReact(OtherPawn)) return;

	Engage(OtherPawn);
}

void AEnemy::SetWeaponVisibility(bool Visibility)
{
	if(EquippedWeapon)
	{
		EquippedWeapon->SetVisibility(Visibility);
	}
}

bool AEnemy::PawnIsNotPlayer(APawn* SeenPawn)
{
	if(SeenPawn)
	{
		return !SeenPawn->ActorHasTag(TEXT("Player"));
	}
	return false;
}

void AEnemy::Disengage()
{
	CombatTarget = nullptr;
	bShouldShoot = false;
	
	if(World)
	{
		World->GetTimerManager().ClearTimer(VisibilityTimer);
		World->GetTimerManager().ClearTimer(InitialVisibilityTimer);
		World->GetTimerManager().ClearTimer(PawnSeenTimer);
		World->GetTimerManager().ClearTimer(StartShootingTimer);
		World->GetTimerManager().ClearTimer(StopShootingTimer);

		if(IsDead() || (GameMode && !GameMode->GameIsRunning()))
		{
			GetController()->StopMovement();
			World->GetTimerManager().ClearTimer(MoveTimer);
		}
	}
}

void AEnemy::StartDangerVisibilityTimer()
{
	if(World)
	{
		World->GetTimerManager().SetTimer(DangerVisibilityTimer, this, &AEnemy::HideDangerWidget, 1.5f);
	}
}

void AEnemy::HideDangerWidget()
{
	if(DangerWidget && DangerWidget->GetWidget())
	{
		DangerWidget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AEnemy::StartPawnSeenTimer()
{
	GetWorld()->GetTimerManager().SetTimer(PawnSeenTimer, this, &AEnemy::PawnSeenTimerEnded, PawnSeenTime);
}

void AEnemy::PawnSeenTimerEnded()
{
	bCanSeePlayer = false;
	Disengage();
}

bool AEnemy::ShouldReact(APawn* OtherPawn)
{
	if(IsDead() || PawnIsNotPlayer(OtherPawn) || OtherPawn->ActorHasTag(TEXT("Dead")) || !GetCapsuleComponent()->IsCollisionEnabled()) {
		return false;
	}
	return true;
}