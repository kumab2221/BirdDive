#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Engine.h"
#include "Engine/HitResult.h"
#include "../../BirdDiveTypes.h"
#include "LandingDetectionComponent.generated.h"

// Delegate for landing detection
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLandingDetected, const FLandingResult&, LandingResult);

/**
 * Landing Detection Component
 * 
 * Detects landing conditions, calculates landing quality, and computes scores.
 * Provides comprehensive landing evaluation for the bird dive gameplay.
 */
UCLASS(ClassGroup=(Flight), meta=(BlueprintSpawnableComponent))
class BIRDDIVE_API ULandingDetectionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULandingDetectionComponent();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Perform comprehensive landing check
     * @return Landing result with quality and score
     */
    UFUNCTION(BlueprintCallable, Category = "Landing Detection")
    FLandingResult CheckLanding();

    /**
     * Check if character is near ground
     * @param OutDistance - Distance to ground (output)
     * @return True if near ground
     */
    UFUNCTION(BlueprintCallable, Category = "Landing Detection")
    bool IsNearGround(float& OutDistance) const;

    /**
     * Get current landing speed
     * @return Current speed of the character
     */
    UFUNCTION(BlueprintCallable, Category = "Landing Detection")
    float GetCurrentLandingSpeed() const;

    /**
     * Check if landing speed is safe
     * @return True if within safe landing speed
     */
    UFUNCTION(BlueprintCallable, Category = "Landing Detection")
    bool IsSafeLandingSpeed() const;

    // Event fired when landing is detected
    UPROPERTY(BlueprintAssignable, Category = "Landing Detection")
    FOnLandingDetected OnLandingDetected;

    // Landing detection parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing Detection", meta = (ClampMin = "10.0", ClampMax = "1000.0"))
    float LandingTraceDistance = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing Detection", meta = (ClampMin = "10.0", ClampMax = "500.0"))
    float LandingSpeedThreshold = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing Detection", meta = (ClampMin = "1.0", ClampMax = "100.0"))
    float PerfectLandingRadius = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing Detection", meta = (ClampMin = "10.0", ClampMax = "500.0"))
    float GoodLandingRadius = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing Detection", meta = (ClampMin = "50.0", ClampMax = "1000.0"))
    float PoorLandingRadius = 400.0f;

public:
    /**
     * Calculate landing quality based on hit result and speed
     * @param HitResult - Ground hit information
     * @param LandingSpeed - Speed at landing
     * @return Landing quality assessment
     */
    ELandingQuality CalculateLandingQuality(const FHitResult& HitResult, float LandingSpeed) const;

    /**
     * Calculate base score for landing
     * @param Quality - Landing quality
     * @param Accuracy - Accuracy factor (0-1)
     * @return Base score points
     */
    int32 CalculateBaseScore(ELandingQuality Quality, float Accuracy) const;

protected:
    /**
     * Perform ground trace
     * @param Start - Trace start location
     * @param End - Trace end location
     * @param OutHit - Hit result (output)
     * @return True if ground was hit
     */
    bool PerformGroundTrace(const FVector& Start, const FVector& End, FHitResult& OutHit) const;

    /**
     * Calculate distance from target center
     * @param HitLocation - Landing location
     * @return Distance from ideal landing spot
     */
    float CalculateDistanceFromCenter(const FVector& HitLocation) const;

    /**
     * Calculate accuracy factor based on distance
     * @param Distance - Distance from center
     * @return Accuracy factor (0-1)
     */
    float CalculateAccuracyFactor(float Distance) const;

private:
    // Target center location (will be set by game mode)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Landing Detection", meta = (AllowPrivateAccess = "true"))
    FVector TargetCenter = FVector::ZeroVector;

    // Landing statistics for debugging
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    int32 LandingAttempts = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    int32 SuccessfulLandings = 0;
};