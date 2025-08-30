// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/AttributeComponent.h"
#include "Controller/Project_A_Controller.h"
#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameInstance/Project_A_GameInstance.h"
#include "GameMode/Project_A_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/PawnSensingComponent.h"
#include "Weapons/BaseWeapon.h"
#include "Weapons/PrimaryWeapons/PrimaryWeapon.h"
#include "Weapons/SecondaryWeapons/SecondaryWeapon.h"
#include "Weapons/Throwable.h"

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(PlayerController && GameMode && GameMode->GameIsRunning())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	
	if(bShouldHeal)
	{
		if(Attributes && Attributes->AddHealth(RecoveryRate * DeltaSeconds) == 1)
		{
			bShouldHeal = false;
		}
	}

	SetSpringArmLength();
}

APlayerCharacter::APlayerCharacter()
{
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	Tags.Add(TEXT("Player"));

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensingComponent->bOnlySensePlayers = false;
	PawnSensingComponent->bSeePawns = false;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->bDoCollisionTest = false;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	SecondaryEndPointSocket = CreateDefaultSubobject<USceneComponent>(TEXT("SecondaryEndPointSocket"));
	SecondaryEndPointSocket->SetupAttachment(GetRootComponent());
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookControllerAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookController);
		EnhancedInputComponent->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookMouse);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Shoot);
		EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SwapWeapon);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ReloadWeapon);
		EnhancedInputComponent->BindAction(ReadyGearAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ReadyGear);
		EnhancedInputComponent->BindAction(ThrowGearAction, ETriggerEvent::Triggered, this, &APlayerCharacter::PlayThrowAnimation);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &APlayerCharacter::PauseKeyPressed);
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(IsDead()) return DamageAmount;
	
	bShouldHeal = false;
	StartRecoveryTimer();
	
	return DamageAmount;
}

void APlayerCharacter::PlayEffects()
{
	Super::PlayEffects();
	
	if(EquippedWeapon && GameInstance)
	{
		if(GameInstance->GetScreenShakeEnabled())
		{
			UGameplayStatics::PlayWorldCameraShake(this, EquippedWeapon->GetCameraShake(), GetActorLocation(), 0.f, 5000.f);
		}

		if(PlayerController)
		{
			if(PlayerController->bForceFeedbackEnabled && EquippedWeapon->GetVibrationEffect())
			{
				PlayerController->ClientPlayForceFeedback(EquippedWeapon->GetVibrationEffect());
			}
		}
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AProject_A_Controller>(GetController());
	SetSpringArmLength();

	if(PlayerController)
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
		
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
		DisableInput(PlayerController);
	}
	SpawnSecondaryWeapon();

	if(World)
	{
		World->GetTimerManager().SetTimer(StartTimer, this, &APlayerCharacter::StartLevel, LevelStartTime);
	}

	if(CursorWidgetClass)
	{
		CursorWidget = Cast<UUserWidget>(CreateWidget(GetWorld(), CursorWidgetClass));
	
		if(CursorWidget)
		{
			CursorWidget->AddToViewport();
			CursorWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortRes);
	}

	if(Camera)
	{
		CameraInitialLocation = Camera->GetRelativeLocation();
	}

	if(PawnSensingComponent)
	{
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &APlayerCharacter::NoiseHeard);
	}
}

void APlayerCharacter::Die()
{
	Super::Die();
	
	if(PlayerController)
	{
		DisableInput(PlayerController);
	}

	if(World)
	{
		if(GameMode)
		{
			GameMode->PlayerLost();
		}
		World->GetTimerManager().ClearAllTimersForObject(this);
	}
}

void APlayerCharacter::SpawnSecondaryWeapon()
{
	if(World && SecondaryWeaponClass)
	{
		SecondaryWeapon = World->SpawnActor<ASecondaryWeapon>(SecondaryWeaponClass, GetMesh()->GetSocketTransform(FName("WeaponSocket")));
		SecondaryWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponSocket"));
		SecondaryEndPointSocket->SetWorldLocation(SecondaryWeapon->GetEndPoint()->GetComponentLocation());
		SecondaryWeapon->GetEndPoint()->AttachToComponent(SecondaryEndPointSocket, FAttachmentTransformRules::KeepWorldTransform);

		// Set Weapon's InitialEndPointLocation to Endpoint's initial location
		SecondaryWeapon->SetInitialEndPointRelativeLocation(SecondaryWeapon->GetEndPoint()->GetRelativeLocation());
		SecondaryWeapon->SetOwner(this);
		SecondaryWeapon->SetVisibility(false);
	}
}

void APlayerCharacter::ReloadWeapon()
{
	if(GameMode && !GameMode->GameIsRunning()) return;
	
	if(EquippedWeapon && (CharacterState == ECharacterState::ECS_Neutral))
	{
		EquippedWeapon->Reload();
	}
}

void APlayerCharacter::ReadyGear()
{
	if(GameMode && !GameMode->GameIsRunning()) return;
	
	if(CharacterState == ECharacterState::ECS_Neutral && HasGear() && GearClass)
	{
		CharacterState = ECharacterState::ECS_UsingGear;
		EquippedWeapon->SetVisibility(false);
	
		Gear = World->SpawnActor<AThrowable>(GearClass, GetMesh()->GetSocketTransform(FName("WeaponSocket")));
		Gear->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("WeaponSocket"));

		// Play gear equip and hold animation
		if(GearMontage)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(GearMontage);
		}
	}
}

void APlayerCharacter::PlayThrowAnimation()
{
	if(GameMode && !GameMode->GameIsRunning()) return;
	
	// Return if throw animation is currently playing
	if(IsThrowingGear()) return;
	
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("GearThrow"));
}

void APlayerCharacter::ThrowGear()
{
	// Decrement HeldGear, detach gear, and then call Throw()
	HeldGear = FMath::Clamp(HeldGear - 1, 0, 2);
	Gear->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Gear->Throw();
	
	EquippedWeapon->SetVisibility(true);
	CharacterState = ECharacterState::ECS_Neutral;
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if(GameMode && !GameMode->GameIsRunning()) return;
	
	const FVector2D Input = Value.Get<FVector2d>().GetSafeNormal();
	
	// World direction of X and Y
	const FVector XDirection = FVector::XAxisVector;
	const FVector YDirection = FVector::YAxisVector;

	AddMovementInput(XDirection, Input.X);
	AddMovementInput(YDirection, Input.Y);
}

void APlayerCharacter::LookController(const FInputActionValue& Value)
{
	if(GameMode && !GameMode->GameIsRunning()) return;
	
	// const FVector2D LookVector2D = Value.Get<FVector2D>().GetSafeNormal();
	LookVector2D = Value.Get<FVector2D>();

	if(CursorWidget)
	{
		CursorWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	
	const FRotator Direction = UKismetMathLibrary::MakeRotationFromAxes(FVector(LookVector2D.X, LookVector2D.Y, 0), FVector::ZeroVector, FVector::ZeroVector);
	
	if(World && PlayerController)
	{
		PlayerController->SetControlRotation(FMath::RInterpTo(GetControlRotation(), Direction, World->GetDeltaSeconds(), AimInterpSpeed));
		PlayerController->SetUsingMouse_Implementation(false);
	}

	if(Camera)
	{
		FVector CameraOffset = GetActorForwardVector() * ControllerCameraOffsetDistance;
		Camera->SetRelativeLocation(CameraInitialLocation + FVector(0.f, -CameraOffset.X, CameraOffset.Y));
	}

	UE_LOG(LogTemp, Warning, TEXT("X: %f Y: %f"), LookVector2D.X, LookVector2D.Y)
	
	// FindLookAtRotation Method
	// Controller->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, LookVector));
}

void APlayerCharacter::LookMouse(const FInputActionValue& Value)
{
	if(GameMode && !GameMode->GameIsRunning()) return;

	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortRes);
	}
	
	 LookVector2D = Value.Get<FVector2D>();
	
	 // Update widget position
	 if(CursorWidget)
	 {
		CursorWidget->SetVisibility(ESlateVisibility::Visible);
	
	 	FVector2D WidgetPosition = FVector2D(
	 		CursorPosition.X - CursorWidget->GetDesiredSize().X/4.f + (WeaponCursorXOffset * GetActorForwardVector().Y),
	 		CursorPosition.Y - CursorWidget->GetDesiredSize().Y/2.f - (WeaponCursorYOffset * GetActorForwardVector().X)
	 		);
	 	
	 	CursorWidget->SetPositionInViewport(WidgetPosition);
	 }
	
	 CursorPosition.X = FMath::Clamp(CursorPosition.X + LookVector2D.X * CursorSensitivity, 0., ViewPortRes.X);
	 CursorPosition.Y = FMath::Clamp(CursorPosition.Y + LookVector2D.Y * CursorSensitivity, 0., ViewPortRes.Y);
	
	 // Rotate character
	 if(PlayerController)
	 {
	 	FHitResult HitResult;
	 	PlayerController->SetMouseLocation(CursorPosition.X, CursorPosition.Y);
	 	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	 	PlayerController->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.Location));
	 	PlayerController->SetUsingMouse_Implementation(true);
	
	 	if(Camera)
	 	{
	 		FVector2D MousePosition;
	 		PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	 		FVector CameraOffset = FVector(0.f, (MousePosition.X - ViewPortRes.X * 0.5), -(MousePosition.Y - ViewPortRes.Y * 0.5)) * MouseCameraOffsetDistance;
	 		Camera->SetRelativeLocation(CameraInitialLocation + CameraOffset);
	 	}
	 }

	// DrawDebugSphere(GetWorld(), HitResult.Location, 5.f, 10.f, FColor::Red, true, 10.f);
	// UE_LOG(LogTemp, Warning, TEXT("X: %f Y: %f"), LookVector2D.X, LookVector2D.Y)
}

void APlayerCharacter::SwapWeapon()
{
	if(GameMode && !GameMode->GameIsRunning()) return;
	
	if(!(PrimaryWeapon && SecondaryWeapon && EquippedWeapon) || !CanSwapWeapon()) return;
	
	SetIsNeutral();
	GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);

	// Switching to secondary weapon
	if(EquippedWeapon->GetClass() == PrimaryWeapon->GetClass())
	{
		PrimaryWeapon->SetVisibility(false);
		EquippedWeapon = SecondaryWeapon;
		SecondaryWeapon->SetVisibility(true);
		EquippedWeapon->PlayEquipSound();
		SecondaryWeapon->GetWeaponXYOffset(WeaponCursorXOffset, WeaponCursorYOffset);
		SetWalkSpeed();
	}
	// Switching to primary weapon
	else
	{
		SecondaryWeapon->SetVisibility(false);
		EquippedWeapon = PrimaryWeapon;
		PrimaryWeapon->SetVisibility(true);
		EquippedWeapon->PlayEquipSound();
		PrimaryWeapon->GetWeaponXYOffset(WeaponCursorXOffset, WeaponCursorYOffset);
		SetWalkSpeed();
	}
}

bool APlayerCharacter::IsThrowingGear()
{
	return GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(GearMontage).IsEqual(FName("GearThrow"));
}

void APlayerCharacter::PauseKeyPressed()
{
	if(GameMode)
	{
		GameMode->PauseGame();
	}
}

bool APlayerCharacter::CanSwapWeapon()
{
	return CharacterState != ECharacterState::ECS_Dead &&
		CharacterState != ECharacterState::ECS_UsingGear;
}

void APlayerCharacter::StartLevel()
{
	Super::StartLevel();

	if(PrimaryWeapon)
	{
		PrimaryWeapon->GetWeaponXYOffset(WeaponCursorXOffset, WeaponCursorYOffset);
	}
	if(SecondaryWeapon)
	{
		SecondaryWeapon->InitializeWeapon();
	}
	if(PlayerController)
	{
		EnableInput(PlayerController);
		// PlayerController->SetMouseLocation(ViewPortRes.X/2, ViewPortRes.Y/2);
	}

	CursorPosition.X = ViewPortRes.X/2;
	CursorPosition.Y = ViewPortRes.Y/2;
	
	if(CursorWidget)
	{
		CursorWidget->SetPositionInViewport(FVector2d(ViewPortRes.X/2, ViewPortRes.Y/2));
	}
}

void APlayerCharacter::Shoot()
{
	if(GameMode && !GameMode->GameIsRunning()) return;
	
	Super::Shoot();
}

void APlayerCharacter::NoiseHeard(APawn* OtherPawn, const FVector& Location, float Volume)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherPawn);

	if(Enemy)
	{
		Enemy->Enemyheard();
	}
}

void APlayerCharacter::StartRecoveryTimer()
{
	if(World)
	{
		World->GetTimerManager().SetTimer(RecoveryTimer, this, &APlayerCharacter::RecoveryTimerFinished, RecoveryTime);
	}
}

void APlayerCharacter::RecoveryTimerFinished()
{
	bShouldHeal = true;
}

void APlayerCharacter::SetSpringArmLength()
{
	if(CameraBoom && EquippedWeapon && World)
	{
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, 1000.f + EquippedWeapon->GetSpringArmLength(), World->GetDeltaSeconds(), 5.f);
	}
}
