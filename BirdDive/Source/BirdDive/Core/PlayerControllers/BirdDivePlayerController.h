#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "BirdDivePlayerController.generated.h"

/**
 * Player controller for Bird Dive Challenge
 * Enhanced Input implementation with multi-device support
 */
UCLASS()
class BIRDDIVE_API ABirdDivePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Constructor
	ABirdDivePlayerController();

	// Input setup with Enhanced Input
	virtual void SetupInputComponent() override;

	// Possession overrides (using new UE5 events)
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// Enhanced Input accessor methods (for testing)
	class UInputAction* GetInputActionMove() const { return InputActionMove; }
	class UInputAction* GetInputActionLook() const { return InputActionLook; }
	class UInputAction* GetInputActionDive() const { return InputActionDive; }
	class UInputAction* GetInputActionBrake() const { return InputActionBrake; }

	// Input processing methods (for testing)
	void ProcessMoveInput(const FVector2D& InputVector);
	void ProcessLookInput(const FVector2D& InputVector);
	FVector2D GetMoveInputVector() const { return CurrentMoveInput; }
	FVector2D GetLookInputVector() const { return CurrentLookInput; }

	// Sensitivity methods (for testing)
	void SetMouseSensitivity(float SensitivityX, float SensitivityY);
	float GetMouseSensitivityX() const { return MouseSensitivityX; }
	float GetMouseSensitivityY() const { return MouseSensitivityY; }
	void SetGamepadLookSensitivity(float Sensitivity);
	float GetGamepadLookSensitivity() const { return GamepadLookSensitivity; }

	// Deadzone methods (for testing)
	void SetStickDeadzone(float DeadzoneRadius);
	float GetStickDeadzone() const { return StickDeadzone; }

	// Device management methods (for testing)
	bool IsKeyboardMouseAvailable() const;
	bool IsGamepadConnected() const;

protected:
	// Lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Enhanced Input Assets
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* InputActionMove;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* InputActionLook;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* InputActionDive;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* InputActionBrake;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	// Input callbacks
	void OnMoveInput(const FInputActionValue& Value);
	void OnLookInput(const FInputActionValue& Value);
	void OnDiveInput(const FInputActionValue& Value);
	void OnBrakeInput(const FInputActionValue& Value);

	// Current input state
	FVector2D CurrentMoveInput;
	FVector2D CurrentLookInput;
	
	// Settings
	float MouseSensitivityX = 1.0f;
	float MouseSensitivityY = 1.0f;
	float GamepadLookSensitivity = 1.0f;
	float StickDeadzone = 0.2f;
};