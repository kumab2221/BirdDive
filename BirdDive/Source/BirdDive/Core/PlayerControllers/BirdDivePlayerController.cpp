#include "BirdDivePlayerController.h"
#include "../../BirdDive.h"
#include "Components/InputComponent.h"
#include "Engine/LocalPlayer.h"

ABirdDivePlayerController::ABirdDivePlayerController()
{
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController constructed"));
}

void ABirdDivePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Enhanced Input Subsystem setup
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Add mapping context if available
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
			UE_LOG(LogBirdDive, Log, TEXT("Enhanced Input Mapping Context added"));
		}
	}
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController BeginPlay with Enhanced Input"));
}

void ABirdDivePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController EndPlay"));
	
	Super::EndPlay(EndPlayReason);
}

void ABirdDivePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Enhanced Input Component setup
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind Enhanced Input Actions
		if (InputActionMove)
		{
			EnhancedInputComponent->BindAction(InputActionMove, ETriggerEvent::Triggered, this, &ABirdDivePlayerController::OnMoveInput);
		}
		
		if (InputActionLook)
		{
			EnhancedInputComponent->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &ABirdDivePlayerController::OnLookInput);
		}
		
		if (InputActionDive)
		{
			EnhancedInputComponent->BindAction(InputActionDive, ETriggerEvent::Started, this, &ABirdDivePlayerController::OnDiveInput);
		}
		
		if (InputActionBrake)
		{
			EnhancedInputComponent->BindAction(InputActionBrake, ETriggerEvent::Started, this, &ABirdDivePlayerController::OnBrakeInput);
		}
		
		UE_LOG(LogBirdDive, Log, TEXT("Enhanced Input setup complete"));
	}
	else
	{
		UE_LOG(LogBirdDive, Warning, TEXT("Failed to get Enhanced Input Component"));
	}
}

void ABirdDivePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController possessed pawn: %s"), 
		InPawn ? *InPawn->GetName() : TEXT("null"));
}

void ABirdDivePlayerController::OnUnPossess()
{
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController unpossessing pawn"));
	
	Super::OnUnPossess();
}

// Enhanced Input Callbacks
void ABirdDivePlayerController::OnMoveInput(const FInputActionValue& Value)
{
	FVector2D MoveValue = Value.Get<FVector2D>();
	ProcessMoveInput(MoveValue);
	UE_LOG(LogBirdDive, VeryVerbose, TEXT("Move input: %s"), *MoveValue.ToString());
}

void ABirdDivePlayerController::OnLookInput(const FInputActionValue& Value)
{
	FVector2D LookValue = Value.Get<FVector2D>();
	// Apply sensitivity
	FVector2D ProcessedLook = FVector2D(LookValue.X * MouseSensitivityX, LookValue.Y * MouseSensitivityY);
	ProcessLookInput(ProcessedLook);
	UE_LOG(LogBirdDive, VeryVerbose, TEXT("Look input: %s"), *ProcessedLook.ToString());
}

void ABirdDivePlayerController::OnDiveInput(const FInputActionValue& Value)
{
	bool bDiving = Value.Get<bool>();
	UE_LOG(LogBirdDive, Log, TEXT("Dive input: %s"), bDiving ? TEXT("Start") : TEXT("Stop"));
}

void ABirdDivePlayerController::OnBrakeInput(const FInputActionValue& Value)
{
	bool bBraking = Value.Get<bool>();
	UE_LOG(LogBirdDive, Log, TEXT("Brake input: %s"), bBraking ? TEXT("Start") : TEXT("Stop"));
}

// Input Processing Methods (for testing)
void ABirdDivePlayerController::ProcessMoveInput(const FVector2D& InputVector)
{
	// Apply deadzone processing
	FVector2D ProcessedInput = InputVector;
	if (InputVector.Size() < StickDeadzone)
	{
		ProcessedInput = FVector2D::ZeroVector;
	}
	
	CurrentMoveInput = ProcessedInput;
}

void ABirdDivePlayerController::ProcessLookInput(const FVector2D& InputVector)
{
	CurrentLookInput = InputVector;
}

// Sensitivity Methods
void ABirdDivePlayerController::SetMouseSensitivity(float SensitivityX, float SensitivityY)
{
	MouseSensitivityX = FMath::Clamp(SensitivityX, 0.1f, 10.0f);
	MouseSensitivityY = FMath::Clamp(SensitivityY, 0.1f, 10.0f);
	UE_LOG(LogBirdDive, Log, TEXT("Mouse sensitivity set to X:%.2f Y:%.2f"), MouseSensitivityX, MouseSensitivityY);
}

void ABirdDivePlayerController::SetGamepadLookSensitivity(float Sensitivity)
{
	GamepadLookSensitivity = FMath::Clamp(Sensitivity, 0.1f, 10.0f);
	UE_LOG(LogBirdDive, Log, TEXT("Gamepad look sensitivity set to %.2f"), GamepadLookSensitivity);
}

// Deadzone Methods
void ABirdDivePlayerController::SetStickDeadzone(float DeadzoneRadius)
{
	StickDeadzone = FMath::Clamp(DeadzoneRadius, 0.0f, 0.9f);
	UE_LOG(LogBirdDive, Log, TEXT("Stick deadzone set to %.2f"), StickDeadzone);
}

// Device Management Methods (minimal implementation)
bool ABirdDivePlayerController::IsKeyboardMouseAvailable() const
{
	// Simple implementation - assume keyboard/mouse is always available on PC
	return true;
}

bool ABirdDivePlayerController::IsGamepadConnected() const
{
	// Simple implementation - for now just return false
	// In a full implementation, this would check actual gamepad connection status
	return false;
}