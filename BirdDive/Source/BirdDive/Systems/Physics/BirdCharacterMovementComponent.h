#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "BirdCharacterMovementComponent.generated.h"

/**
 * Bird Character Movement Component
 * 
 * Extended CharacterMovementComponent for bird flight physics.
 * Integrates wind forces, air resistance, and custom gravity for realistic flight simulation.
 */
UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class BIRDDIVE_API UBirdCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UBirdCharacterMovementComponent();

protected:
    virtual void BeginPlay() override;

public:
    // Override physics calculations
    virtual void PhysicsRotation(float DeltaTime) override;
    virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

    /**
     * Apply wind force to the character
     * @param WindForce - Wind force vector to apply
     * @param DeltaTime - Time since last update
     */
    UFUNCTION(BlueprintCallable, Category = "Bird Physics")
    void ApplyWindForce(const FVector& WindForce, float DeltaTime);

    /**
     * Apply air resistance to current velocity
     * @param DeltaTime - Time since last update
     */
    UFUNCTION(BlueprintCallable, Category = "Bird Physics")
    void ApplyAirResistance(float DeltaTime);

    /**
     * Enforce maximum speed limits
     */
    UFUNCTION(BlueprintCallable, Category = "Bird Physics")
    void EnforceSpeedLimits();

    // Air resistance coefficient (0.0 = no resistance, 1.0 = maximum resistance)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird Physics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AirResistance = 0.02f;

    // Custom gravity multiplier (1.0 = normal gravity)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird Physics", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float GravityMultiplier = 1.0f;

    // Maximum diving speed (cm/s)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird Physics", meta = (ClampMin = "100.0", ClampMax = "10000.0"))
    float MaxDiveSpeed = 2000.0f;

    // Maximum horizontal speed (cm/s)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird Physics", meta = (ClampMin = "100.0", ClampMax = "5000.0"))
    float MaxHorizontalSpeed = 1500.0f;

    // Wind force influence factor (0.0 = no wind effect, 1.0 = full wind effect)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird Physics", meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float WindInfluence = 1.0f;

    // Enable/disable physics debug information
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowDebugInfo = false;

protected:
    /**
     * Validate and sanitize velocity values
     * @param InVelocity - Velocity to validate
     * @return Sanitized velocity
     */
    FVector ValidateVelocity(const FVector& InVelocity) const;

    /**
     * Calculate custom gravity force
     * @param DeltaTime - Time since last update
     * @return Gravity force vector
     */
    FVector CalculateGravityForce(float DeltaTime) const;

    /**
     * Draw debug information
     */
    void DrawDebugInfo() const;

private:
    // Accumulated wind force for this frame
    FVector AccumulatedWindForce = FVector::ZeroVector;

    // Last frame's velocity for debugging
    FVector LastFrameVelocity = FVector::ZeroVector;

    // Physics statistics for performance monitoring
    float LastPhysicsUpdateTime = 0.0f;
    int32 PhysicsUpdateCount = 0;
};