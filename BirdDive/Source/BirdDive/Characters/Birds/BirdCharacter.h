#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "../../BirdDiveTypes.h"
#include "BirdCharacter.generated.h"

// Forward declarations
class UInputMappingContext;
class UInputAction;
class UWindForceComponent;
class UBirdCharacterMovementComponent;
class UFlightStateManager;
class ULandingDetectionComponent;
class UBirdControlRigComponent;

/**
 * Bird Character Class
 * 
 * Complete implementation of player-controlled bird with physics integration,
 * Enhanced Input System, flight state management, and landing detection.
 */
UCLASS()
class BIRDDIVE_API ABirdCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Constructor
	ABirdCharacter();

protected:
	// Lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Tick function
	virtual void Tick(float DeltaTime) override;

	// Enhanced Input System
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Enhanced Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FlyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	// Flight state management
	UFUNCTION(BlueprintCallable, Category = "Flight")
	EFlightState GetCurrentFlightState() const;

	UFUNCTION(BlueprintCallable, Category = "Flight")
	void SetFlightState(EFlightState NewState);

	// Landing detection
	UFUNCTION(BlueprintCallable, Category = "Flight")
	bool IsLanding() const;

	UFUNCTION(BlueprintCallable, Category = "Flight")
	FLandingResult PerformLandingCheck();

	// Speed monitoring
	UFUNCTION(BlueprintCallable, Category = "Flight")
	float GetCurrentSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Flight")
	bool IsOverSpeedLimit() const;

	// Component accessors
	UFUNCTION(BlueprintCallable, Category = "Components")
	UWindForceComponent* GetWindForceComponent() const { return WindForceComponent; }

	UFUNCTION(BlueprintCallable, Category = "Components")
	UBirdCharacterMovementComponent* GetBirdMovementComponent() const { return BirdMovementComponent; }

	UFUNCTION(BlueprintCallable, Category = "Components")
	UFlightStateManager* GetFlightStateManager() const { return FlightStateManager; }

	UFUNCTION(BlueprintCallable, Category = "Components")
	ULandingDetectionComponent* GetLandingDetectionComponent() const { return LandingDetectionComponent; }

	UFUNCTION(BlueprintCallable, Category = "Components")
	UBirdControlRigComponent* GetBirdControlRigComponent() const { return BirdControlRigComponent; }

	// Animation Integration
	UFUNCTION(BlueprintCallable, Category = "Flight")
	EFlightState GetFlightState() const;

	UFUNCTION(BlueprintCallable, Category = "Flight")
	FVector2D GetFlightInput() const { return CurrentFlightInput; }

	UFUNCTION(BlueprintCallable, Category = "Flight")
	float GetRollInput() const { return CurrentRollInput; }

	UFUNCTION(BlueprintCallable, Category = "Flight")
	void SetFlightInput(const FVector2D& Input) { CurrentFlightInput = Input; }

	// Enhanced Input System accessors for tests
	UInputMappingContext* GetDefaultMappingContext() const { return DefaultMappingContext; }
	UInputAction* GetFlyAction() const { return FlyAction; }
	UInputAction* GetLookAction() const { return LookAction; }

protected:
	// Input handling
	void HandleFlyInput(const FInputActionValue& Value);
	void HandleLookInput(const FInputActionValue& Value);

	// Physics integration
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
	UWindForceComponent* WindForceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
	UBirdCharacterMovementComponent* BirdMovementComponent;

	// Flight system components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	UFlightStateManager* FlightStateManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	ULandingDetectionComponent* LandingDetectionComponent;

	// Animation system components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UBirdControlRigComponent* BirdControlRigComponent;

	// Flight parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Parameters")
	float FlyForceMultiplier = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Parameters")
	float TurnSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Parameters")
	float MaxSafeSpeed = 2000.0f;

	// Input tracking for animation
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FVector2D CurrentFlightInput = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	float CurrentRollInput = 0.0f;

private:
	// Test helper flag
	bool bHasBegunPlay;

public:
	// Test helper functions
	bool HasBegunPlay() const { return bHasBegunPlay; }
	
	UFUNCTION(BlueprintCallable, Category = "Test", CallInEditor)
	void InitializeForTest() { BeginPlay(); }
};