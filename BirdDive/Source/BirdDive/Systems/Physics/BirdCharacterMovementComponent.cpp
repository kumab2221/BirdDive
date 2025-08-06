#include "BirdCharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"

UBirdCharacterMovementComponent::UBirdCharacterMovementComponent()
{
    // Initialize default values
    AirResistance = 0.02f;
    GravityMultiplier = 1.0f;
    MaxDiveSpeed = 2000.0f;
    MaxHorizontalSpeed = 1500.0f;
    WindInfluence = 1.0f;
    bShowDebugInfo = false;

    // Set default movement mode to falling (for bird physics)
    DefaultLandMovementMode = MOVE_Falling;
    
    // Disable ground friction for air movement
    GroundFriction = 0.0f;
    BrakingFrictionFactor = 0.0f;
}

void UBirdCharacterMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize physics tracking
    LastPhysicsUpdateTime = 0.0f;
    PhysicsUpdateCount = 0;
    AccumulatedWindForce = FVector::ZeroVector;
}

void UBirdCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
    // Call parent implementation first
    Super::PhysicsRotation(DeltaTime);

    // Update physics statistics
    PhysicsUpdateCount++;
    LastPhysicsUpdateTime = DeltaTime;
}

void UBirdCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
    // Apply custom gravity first
    FVector GravityForce = CalculateGravityForce(DeltaTime);
    Velocity += GravityForce;

    // Apply accumulated wind force
    if (!AccumulatedWindForce.IsNearlyZero())
    {
        Velocity += AccumulatedWindForce * WindInfluence;
        AccumulatedWindForce = FVector::ZeroVector; // Reset for next frame
    }

    // Apply air resistance
    ApplyAirResistance(DeltaTime);

    // Enforce speed limits
    EnforceSpeedLimits();

    // Validate and sanitize velocity
    Velocity = ValidateVelocity(Velocity);

    // Store for debugging
    LastFrameVelocity = Velocity;

    // Draw debug information if enabled
    if (bShowDebugInfo)
    {
        DrawDebugInfo();
    }

    // Call parent with modified parameters (no friction in air)
    Super::CalcVelocity(DeltaTime, 0.0f, bFluid, 0.0f);
}

void UBirdCharacterMovementComponent::ApplyWindForce(const FVector& WindForce, float DeltaTime)
{
    // Validate input
    if (!FMath::IsFinite(WindForce.Size()) || DeltaTime <= 0.0f)
    {
        return;
    }

    // Accumulate wind force (will be applied in CalcVelocity)
    FVector ScaledWindForce = WindForce * DeltaTime;
    AccumulatedWindForce += ValidateVelocity(ScaledWindForce);
}

void UBirdCharacterMovementComponent::ApplyAirResistance(float DeltaTime)
{
    if (AirResistance <= 0.0f || Velocity.IsNearlyZero())
    {
        return;
    }

    // Calculate air resistance force (opposite to velocity direction)
    float CurrentSpeed = Velocity.Size();
    if (CurrentSpeed > 0.0f)
    {
        FVector VelocityDirection = Velocity / CurrentSpeed;
        
        // Air resistance increases with speed squared (simplified aerodynamics)
        float ResistanceMagnitude = AirResistance * CurrentSpeed * CurrentSpeed * DeltaTime;
        
        // Apply resistance opposite to velocity direction
        FVector ResistanceForce = -VelocityDirection * ResistanceMagnitude;
        
        // Don't let air resistance reverse the velocity direction
        if (ResistanceForce.Size() > CurrentSpeed)
        {
            ResistanceForce = -Velocity * 0.95f; // Slow down to 5% of current speed
        }
        
        Velocity += ResistanceForce;
    }
}

void UBirdCharacterMovementComponent::EnforceSpeedLimits()
{
    if (Velocity.IsNearlyZero())
    {
        return;
    }

    // Separate horizontal and vertical components
    FVector HorizontalVelocity = FVector(Velocity.X, Velocity.Y, 0.0f);
    float VerticalVelocity = Velocity.Z;

    // Limit horizontal speed
    float HorizontalSpeed = HorizontalVelocity.Size();
    if (HorizontalSpeed > MaxHorizontalSpeed)
    {
        HorizontalVelocity = HorizontalVelocity.GetSafeNormal() * MaxHorizontalSpeed;
    }

    // Limit diving speed (negative Z is downward)
    if (VerticalVelocity < -MaxDiveSpeed)
    {
        VerticalVelocity = -MaxDiveSpeed;
    }

    // Reconstruct velocity
    Velocity = FVector(HorizontalVelocity.X, HorizontalVelocity.Y, VerticalVelocity);
}

FVector UBirdCharacterMovementComponent::ValidateVelocity(const FVector& InVelocity) const
{
    FVector ValidatedVelocity = InVelocity;

    // Check for NaN or infinite values
    if (!FMath::IsFinite(ValidatedVelocity.X))
    {
        ValidatedVelocity.X = 0.0f;
    }
    if (!FMath::IsFinite(ValidatedVelocity.Y))
    {
        ValidatedVelocity.Y = 0.0f;
    }
    if (!FMath::IsFinite(ValidatedVelocity.Z))
    {
        ValidatedVelocity.Z = 0.0f;
    }

    // Clamp to reasonable maximum values to prevent physics explosions
    const float MaxSafeSpeed = 50000.0f; // 500 m/s
    if (ValidatedVelocity.Size() > MaxSafeSpeed)
    {
        ValidatedVelocity = ValidatedVelocity.GetSafeNormal() * MaxSafeSpeed;
    }

    return ValidatedVelocity;
}

FVector UBirdCharacterMovementComponent::CalculateGravityForce(float DeltaTime) const
{
    // Get world gravity
    float WorldGravity = GetWorld() ? GetWorld()->GetGravityZ() : -980.0f;
    
    // Apply custom gravity multiplier
    float CustomGravity = WorldGravity * GravityMultiplier;
    
    return FVector(0.0f, 0.0f, CustomGravity * DeltaTime);
}

void UBirdCharacterMovementComponent::DrawDebugInfo() const
{
    if (!GetWorld() || !GetOwner())
    {
        return;
    }

    FVector OwnerLocation = GetOwner()->GetActorLocation();
    
    // Draw velocity vector
    FVector VelocityEnd = OwnerLocation + Velocity * 0.01f; // Scale for visibility
    DrawDebugLine(GetWorld(), OwnerLocation, VelocityEnd, FColor::Green, false, 0.0f, 0, 2.0f);
    
    // Draw wind force vector
    if (!AccumulatedWindForce.IsNearlyZero())
    {
        FVector WindEnd = OwnerLocation + AccumulatedWindForce * 0.1f; // Scale for visibility
        DrawDebugLine(GetWorld(), OwnerLocation, WindEnd, FColor::Blue, false, 0.0f, 0, 2.0f);
    }
    
    // Display text information
    FString DebugText = FString::Printf(TEXT("Speed: %.1f cm/s\nWind: %.1f\nGravity: %.1fx"), 
                                       Velocity.Size(), 
                                       AccumulatedWindForce.Size(),
                                       GravityMultiplier);
    
    DrawDebugString(GetWorld(), OwnerLocation + FVector(0, 0, 100), DebugText, nullptr, FColor::White, 0.0f);
}