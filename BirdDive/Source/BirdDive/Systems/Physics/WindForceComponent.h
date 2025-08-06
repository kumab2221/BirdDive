#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Engine.h"
#include "WindForceComponent.generated.h"

/**
 * Wind Force Component
 * 
 * Provides Perlin noise-based wind force calculations for realistic bird flight physics.
 * Supports difficulty-based wind strength adjustment and efficient performance.
 */
UCLASS(ClassGroup=(Physics), meta=(BlueprintSpawnableComponent))
class BIRDDIVE_API UWindForceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWindForceComponent();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Calculate wind force at a specific position and height
     * @param Position - World position to calculate wind force
     * @param Height - Height above ground level
     * @return Wind force vector
     */
    UFUNCTION(BlueprintCallable, Category = "Wind System")
    FVector CalculateWindForce(const FVector& Position, float Height) const;

    /**
     * Get Perlin noise value for wind calculation
     * @param Position - World position
     * @param Time - Current time for animation
     * @return Perlin noise value between -1.0 and 1.0
     */
    UFUNCTION(BlueprintCallable, Category = "Wind System")
    float GetPerlinNoiseValue(const FVector& Position, float Time) const;

    // Wind strength multiplier (difficulty-based)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Parameters", meta = (ClampMin = "0.0", ClampMax = "5.0"))
    float WindStrength = 1.0f;

    // Perlin noise scale factor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Parameters", meta = (ClampMin = "0.001", ClampMax = "1.0"))
    float NoiseScale = 0.1f;

    // Time scale for wind animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Parameters", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float TimeScale = 1.0f;

    // Base wind direction (normalized)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Parameters")
    FVector BaseWindDirection = FVector(1.0f, 0.0f, 0.0f);

    // Height influence factor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Parameters", meta = (ClampMin = "0.0", ClampMax = "2.0", ToolTip = "How much height affects wind strength. Higher values increase wind at altitude."))
    float HeightInfluence = 1.0f;

    // Maximum wind force magnitude
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind Parameters", meta = (ClampMin = "1.0", ClampMax = "10000.0", ToolTip = "Maximum wind force in Unreal units. Prevents extreme wind values."))
    float MaxWindForce = 1000.0f;

    // Blueprint helper functions
    UFUNCTION(BlueprintCallable, Category = "Wind System", meta = (ToolTip = "Set wind parameters for specific difficulty level"))
    void SetDifficultyParameters(EDifficultyLevel Difficulty);

    UFUNCTION(BlueprintCallable, Category = "Wind System", meta = (ToolTip = "Get current wind intensity at location (0-1 range)"))
    float GetWindIntensityAtLocation(const FVector& Location, float Height) const;

private:
    // Physics constants
    static constexpr float MIN_HEIGHT_FACTOR = 0.1f;
    static constexpr float HEIGHT_NORMALIZATION = 1000.0f;
    static constexpr float VERTICAL_WIND_SCALE = 0.2f;
    static constexpr float HORIZONTAL_WIND_SCALE = 0.5f;

    // Simple Perlin noise implementation for wind calculations
    float SimplePerlinNoise(float x, float y, float z) const;
    
    // Fade function for smooth interpolation
    float Fade(float t) const;
    
    // Linear interpolation
    float Lerp(float t, float a, float b) const;
    
    // Gradient function
    float Grad(int hash, float x, float y, float z) const;

    // Input validation
    bool IsValidInput(const FVector& Position, float Height) const;

    // Permutation table for Perlin noise
    static const int32 PermutationTable[256];
};