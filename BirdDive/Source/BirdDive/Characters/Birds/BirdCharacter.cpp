#include "BirdCharacter.h"
#include "../../BirdDive.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "../../Systems/Physics/WindForceComponent.h"
#include "../../Systems/Physics/BirdCharacterMovementComponent.h"
#include "../../Systems/Flight/FlightStateManager.h"
#include "../../Systems/Flight/LandingDetectionComponent.h"

ABirdCharacter::ABirdCharacter()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Initialize state
	bHasBegunPlay = false;

	// Character setup
	GetCapsuleComponent()->SetCapsuleHalfHeight(50.0f);
	GetCapsuleComponent()->SetCapsuleRadius(30.0f);

	// Replace default CharacterMovementComponent with BirdCharacterMovementComponent
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DestroyComponent();
	}
	BirdMovementComponent = CreateDefaultSubobject<UBirdCharacterMovementComponent>(TEXT("BirdMovementComponent"));

	// Create physics components
	WindForceComponent = CreateDefaultSubobject<UWindForceComponent>(TEXT("WindForceComponent"));

	// Create flight system components
	FlightStateManager = CreateDefaultSubobject<UFlightStateManager>(TEXT("FlightStateManager"));
	LandingDetectionComponent = CreateDefaultSubobject<ULandingDetectionComponent>(TEXT("LandingDetectionComponent"));

	// Initialize flight parameters
	FlyForceMultiplier = 1000.0f;
	TurnSensitivity = 1.0f;
	MaxSafeSpeed = 2000.0f;

	// Input system will be initialized in Blueprint or by game mode
	DefaultMappingContext = nullptr;
	FlyAction = nullptr;
	LookAction = nullptr;

	UE_LOG(LogBirdDive, Log, TEXT("BirdCharacter constructed with full components"));
}

void ABirdCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	bHasBegunPlay = true;
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				UE_LOG(LogBirdDive, Log, TEXT("Added Enhanced Input Mapping Context"));
			}
		}
	}
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdCharacter BeginPlay complete"));
}

void ABirdCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bHasBegunPlay = false;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdCharacter EndPlay"));
	
	Super::EndPlay(EndPlayReason);
}

void ABirdCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Apply wind forces if available
	if (WindForceComponent && BirdMovementComponent)
	{
		FVector WindForce = WindForceComponent->CalculateWindForce(GetActorLocation(), GetActorLocation().Z);
		BirdMovementComponent->ApplyWindForce(WindForce, DeltaTime);
	}

	// Check for automatic landing detection
	if (FlightStateManager && FlightStateManager->GetCurrentState() == EFlightState::Landing)
	{
		if (LandingDetectionComponent)
		{
			float DistanceToGround;
			if (LandingDetectionComponent->IsNearGround(DistanceToGround) && DistanceToGround < 20.0f)
			{
				// Perform landing
				FLandingResult Result = LandingDetectionComponent->CheckLanding();
				if (Result.Quality != ELandingQuality::Miss)
				{
					FlightStateManager->TransitionToState(EFlightState::Landed);
				}
			}
		}
	}
}

void ABirdCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Flying
		if (FlyAction)
		{
			EnhancedInputComponent->BindAction(FlyAction, ETriggerEvent::Triggered, this, &ABirdCharacter::HandleFlyInput);
		}

		// Looking
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABirdCharacter::HandleLookInput);
		}

		UE_LOG(LogBirdDive, Log, TEXT("Enhanced Input actions bound"));
	}
	else
	{
		UE_LOG(LogBirdDive, Warning, TEXT("Enhanced Input Component not available"));
	}
}

// Flight state management
EFlightState ABirdCharacter::GetCurrentFlightState() const
{
	return FlightStateManager ? FlightStateManager->GetCurrentState() : EFlightState::Idle;
}

void ABirdCharacter::SetFlightState(EFlightState NewState)
{
	if (FlightStateManager)
	{
		FlightStateManager->TransitionToState(NewState);
	}
}

// Landing detection
bool ABirdCharacter::IsLanding() const
{
	return GetCurrentFlightState() == EFlightState::Landing;
}

FLandingResult ABirdCharacter::PerformLandingCheck()
{
	if (LandingDetectionComponent)
	{
		return LandingDetectionComponent->CheckLanding();
	}
	
	// Return empty result if no component
	FLandingResult EmptyResult;
	EmptyResult.Quality = ELandingQuality::Miss;
	return EmptyResult;
}

// Speed monitoring
float ABirdCharacter::GetCurrentSpeed() const
{
	return GetVelocity().Size();
}

bool ABirdCharacter::IsOverSpeedLimit() const
{
	return GetCurrentSpeed() > MaxSafeSpeed;
}

// Input handling
void ABirdCharacter::HandleFlyInput(const FInputActionValue& Value)
{
	// Get movement input
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Validate input
	if (!FMath::IsFinite(MovementVector.X) || !FMath::IsFinite(MovementVector.Y))
	{
		UE_LOG(LogBirdDive, Warning, TEXT("Invalid flight input detected: %s"), *MovementVector.ToString());
		return;
	}

	if (Controller != nullptr)
	{
		// Apply flight forces
		if (BirdMovementComponent)
		{
			// Forward/backward force
			if (MovementVector.Y != 0.0f)
			{
				FVector ForwardDirection = GetActorForwardVector();
				FVector ForwardForce = ForwardDirection * MovementVector.Y * FlyForceMultiplier;
				BirdMovementComponent->AddInputVector(ForwardForce);
			}

			// Right/left force
			if (MovementVector.X != 0.0f)
			{
				FVector RightDirection = GetActorRightVector();
				FVector RightForce = RightDirection * MovementVector.X * FlyForceMultiplier;
				BirdMovementComponent->AddInputVector(RightForce);
			}
		}

		// Update flight state if needed
		if (FlightStateManager && FlightStateManager->GetCurrentState() == EFlightState::Idle)
		{
			if (!MovementVector.IsNearlyZero())
			{
				FlightStateManager->TransitionToState(EFlightState::Flying);
			}
		}
	}
}

void ABirdCharacter::HandleLookInput(const FInputActionValue& Value)
{
	// Get look input
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Validate input
	if (!FMath::IsFinite(LookAxisVector.X) || !FMath::IsFinite(LookAxisVector.Y))
	{
		UE_LOG(LogBirdDive, Warning, TEXT("Invalid look input detected: %s"), *LookAxisVector.ToString());
		return;
	}

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * TurnSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * TurnSensitivity);

		// Update flight state for turning
		if (FlightStateManager && 
			(FlightStateManager->GetCurrentState() == EFlightState::Flying || 
			 FlightStateManager->GetCurrentState() == EFlightState::Gliding))
		{
			if (!LookAxisVector.IsNearlyZero(0.1f))
			{
				FlightStateManager->TransitionToState(EFlightState::Turning);
			}
		}
	}
}