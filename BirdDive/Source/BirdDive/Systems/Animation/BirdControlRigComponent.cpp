#include "BirdControlRigComponent.h"
#include "ControlRig.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "../../Characters/Birds/BirdCharacter.h"
#include "../../Systems/Physics/WindForceComponent.h"

// Sets default values for this component's properties
UBirdControlRigComponent::UBirdControlRigComponent()
{
    // Set this component to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize default values
    bIsInitialized = false;
    CurrentFlightState = EFlightState::Idle;
    CurrentFlightSpeed = 0.0f;
    ControlRigInstance = nullptr;
    
    // Initialize default parameters
    CurrentParameters = FBirdControlRigParams();
    
    // Clear error state
    ClearError();
}

// Called when the game starts
void UBirdControlRigComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize the ControlRig system
    InitializeControlRig();
}

void UBirdControlRigComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Cleanup ControlRig resources
    CleanupControlRig();
    
    Super::EndPlay(EndPlayReason);
}

// Called every frame
void UBirdControlRigComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (bIsInitialized)
    {
        // Update flapping based on current flight state and speed
        UpdateFlappingInternal();
        
        // Apply any pending wind forces
        UpdateWindResponse(DeltaTime);
    }
}

// === 基本機能の実装 ===

bool UBirdControlRigComponent::IsInitialized() const
{
    return bIsInitialized;
}

UControlRig* UBirdControlRigComponent::GetControlRigInstance() const
{
    return ControlRigInstance;
}

bool UBirdControlRigComponent::SetControlParameters(const FBirdControlRigParams& Params)
{
    if (!bIsInitialized)
    {
        SetError(TEXT("Not initialized"));
        return false;
    }
    
    if (!ValidateParameters(Params))
    {
        SetError(TEXT("Invalid parameter"));
        return false;
    }
    
    CurrentParameters = Params;
    ClearError();
    return true;
}

FBirdControlRigParams UBirdControlRigComponent::GetControlParameters() const
{
    return CurrentParameters;
}

FString UBirdControlRigComponent::GetLastError() const
{
    return LastErrorMessage;
}

// === 羽ばたき制御の実装 ===

void UBirdControlRigComponent::SetFlightSpeed(float Speed)
{
    CurrentFlightSpeed = FMath::Max(0.0f, Speed);
    
    // Update flapping frequency based on new speed
    CurrentParameters.FlappingFrequency = CalculateFlappingFrequency(Speed);
}

void UBirdControlRigComponent::UpdateFlapping(float DeltaTime)
{
    if (bIsInitialized)
    {
        UpdateFlappingInternal();
    }
}

float UBirdControlRigComponent::CalculateFlappingFrequency(float FlightSpeed) const
{
    // Base frequency increases with flight speed
    // Formula: Base frequency (1.0) + speed factor
    // At 1000 units/sec, frequency should be around 2.0
    // At 2000 units/sec, frequency should be around 3.0
    float BaseFrequency = 1.0f;
    float SpeedFactor = FlightSpeed / 1000.0f;
    return BaseFrequency + SpeedFactor;
}

void UBirdControlRigComponent::SetFlightState(EFlightState State)
{
    CurrentFlightState = State;
    
    // Update wing angle based on new flight state
    UpdateWingAngleForFlightState();
}

void UBirdControlRigComponent::UpdateWingPosition(float DeltaTime)
{
    if (bIsInitialized)
    {
        UpdateWingAngleForFlightState();
    }
}

// === 風力応答制御の実装 ===

void UBirdControlRigComponent::ApplyWindForce(const FVector& WindForce)
{
    if (bIsInitialized)
    {
        ApplyWindInfluence(WindForce);
    }
}

void UBirdControlRigComponent::UpdateWindResponse(float DeltaTime)
{
    if (bIsInitialized)
    {
        // Apply any pending wind stabilization effects
        ApplyWindStabilization();
    }
}

void UBirdControlRigComponent::SetStabilizationStrength(float Strength)
{
    CurrentParameters.StabilizationStrength = FMath::Clamp(Strength, 0.0f, 1.0f);
}

// === プロシージャル制御の実装 ===

bool UBirdControlRigComponent::SetIndividualFeatherAngles(const TArray<float>& Angles)
{
    if (!bIsInitialized)
    {
        SetError(TEXT("Not initialized"));
        return false;
    }
    
    IndividualFeatherAngles = Angles;
    ClearError();
    return true;
}

TArray<float> UBirdControlRigComponent::GetIndividualFeatherAngles() const
{
    return IndividualFeatherAngles;
}

void UBirdControlRigComponent::SetTailFanAngle(float Angle)
{
    CurrentParameters.TailFanAngle = Angle;
}

void UBirdControlRigComponent::ApplyTailControl(const FRotator& TurnDirection)
{
    // Apply tail control for directional changes
    // Adjust tail fan angle based on turn direction
    float TurnInfluence = FMath::Abs(TurnDirection.Yaw) / 90.0f; // Normalize to 0-1
    float NewTailAngle = CurrentParameters.TailFanAngle + (TurnInfluence * 15.0f);
    CurrentParameters.TailFanAngle = FMath::Clamp(NewTailAngle, -45.0f, 45.0f);
}

// === Private ヘルパー関数の実装 ===

void UBirdControlRigComponent::InitializeControlRig()
{
    // For minimum viable implementation, we'll create a basic ControlRig setup
    // In a full implementation, this would load the actual ControlRig asset
    
    // TODO: Load actual ControlRig asset from /Game/BirdDive/Characters/Birds/CR_Bird
    // For now, we'll just mark as initialized
    
    bIsInitialized = true;
    ControlRigInstance = nullptr; // Would be assigned actual ControlRig instance
    ClearError();
}

void UBirdControlRigComponent::CleanupControlRig()
{
    if (ControlRigInstance)
    {
        // Cleanup ControlRig resources
        ControlRigInstance = nullptr;
    }
    
    bIsInitialized = false;
}

bool UBirdControlRigComponent::ValidateParameters(const FBirdControlRigParams& Params)
{
    // Validate flapping frequency (must be positive)
    if (Params.FlappingFrequency < 0.0f)
    {
        return false;
    }
    
    // Validate wing angle (reasonable range)
    if (FMath::Abs(Params.WingAngle) > 90.0f)
    {
        return false;
    }
    
    // Validate wing spread (0-1 range)
    if (Params.WingSpread < 0.0f)
    {
        return false;
    }
    
    // Validate stabilization strength (0-1 range)
    if (Params.StabilizationStrength < 0.0f || Params.StabilizationStrength > 1.0f)
    {
        return false;
    }
    
    return true;
}

void UBirdControlRigComponent::SetError(const FString& ErrorMessage)
{
    LastErrorMessage = ErrorMessage;
}

void UBirdControlRigComponent::ClearError()
{
    LastErrorMessage = TEXT("");
}

void UBirdControlRigComponent::UpdateFlappingInternal()
{
    // Update flapping frequency based on current flight speed
    CurrentParameters.FlappingFrequency = CalculateFlappingFrequency(CurrentFlightSpeed);
    
    // Update wing angle based on current flight state
    UpdateWingAngleForFlightState();
}

void UBirdControlRigComponent::UpdateWingAngleForFlightState()
{
    CurrentParameters.WingAngle = CalculateWingAngleForState(CurrentFlightState);
}

void UBirdControlRigComponent::ApplyWindInfluence(const FVector& WindForce)
{
    // Store wind influence in parameters
    CurrentParameters.WindInfluence = WindForce;
    
    // Calculate body rotation based on wind force
    CurrentParameters.BodyRotation = CalculateBodyRotationForWind(WindForce);
    
    // Calculate stabilization strength based on wind intensity
    float WindIntensity = WindForce.Size();
    CurrentParameters.StabilizationStrength = CalculateStabilizationStrength(WindForce);
}

void UBirdControlRigComponent::ApplyWindStabilization()
{
    // Apply stabilization effects based on current wind influence
    float WindIntensity = CurrentParameters.WindInfluence.Size();
    
    if (WindIntensity > 500.0f) // Strong wind threshold
    {
        // Increase wing spread for stability
        CurrentParameters.WingSpread = FMath::Min(1.0f, CurrentParameters.WingSpread + 0.1f);
        
        // Increase stabilization strength
        CurrentParameters.StabilizationStrength = FMath::Min(1.0f, CurrentParameters.StabilizationStrength + 0.1f);
    }
}

float UBirdControlRigComponent::CalculateWingAngleForState(EFlightState State) const
{
    switch (State)
    {
    case EFlightState::Flying:
        return 0.0f; // Neutral angle for level flight
        
    case EFlightState::Gliding:
        return -5.0f; // Slight negative angle for gliding
        
    case EFlightState::Turning:
        return 10.0f; // Positive angle for maneuvering
        
    case EFlightState::Landing:
        return 20.0f; // High angle for landing approach
        
    case EFlightState::Landed:
        return 0.0f; // Neutral position on ground
        
    case EFlightState::Idle:
    default:
        return 0.0f; // Neutral position
    }
}

FRotator UBirdControlRigComponent::CalculateBodyRotationForWind(const FVector& WindForce) const
{
    FRotator BodyRotation = FRotator::ZeroRotator;
    
    // Calculate roll based on crosswind (Y component)
    if (FMath::Abs(WindForce.Y) > 0.1f)
    {
        // Lean into the wind to maintain stability
        float RollAngle = -WindForce.Y / 100.0f; // Scale factor
        BodyRotation.Roll = FMath::Clamp(RollAngle, -30.0f, 30.0f);
    }
    
    // Calculate pitch based on headwind/tailwind (X component)
    if (FMath::Abs(WindForce.X) > 0.1f)
    {
        // Adjust pitch slightly for headwind/tailwind
        float PitchAngle = WindForce.X / 200.0f; // Scale factor
        BodyRotation.Pitch = FMath::Clamp(PitchAngle, -15.0f, 15.0f);
    }
    
    return BodyRotation;
}

float UBirdControlRigComponent::CalculateStabilizationStrength(const FVector& WindForce) const
{
    // Calculate stabilization strength based on wind intensity
    float WindIntensity = WindForce.Size();
    
    // Normalize wind intensity to 0-1 range
    // Assume max wind force of 2000 units
    float NormalizedIntensity = FMath::Clamp(WindIntensity / 2000.0f, 0.0f, 1.0f);
    
    // Apply curve to make stabilization more aggressive in strong winds
    return FMath::Pow(NormalizedIntensity, 0.7f);
}