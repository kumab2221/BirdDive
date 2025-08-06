#include "LandingDetectionComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "../../BirdDive.h"

ULandingDetectionComponent::ULandingDetectionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Initialize parameters
    LandingTraceDistance = 200.0f;
    LandingSpeedThreshold = 100.0f;
    PerfectLandingRadius = 50.0f;
    GoodLandingRadius = 150.0f;
    PoorLandingRadius = 400.0f;
    
    // Initialize stats
    LandingAttempts = 0;
    SuccessfulLandings = 0;
}

void ULandingDetectionComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize target center (for now, use world origin)
    // In a real game, this would be set by the game mode or level
    TargetCenter = FVector(0.0f, 0.0f, 0.0f);
}

FLandingResult ULandingDetectionComponent::CheckLanding()
{
    FLandingResult Result;
    Result.Quality = ELandingQuality::Miss;
    Result.DistanceFromCenter = 0.0f;
    Result.LandingSpeed = 0.0f;
    Result.BaseScore = 0;
    Result.BonusScore = 0;

    // Get owner character
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogBirdDive, Warning, TEXT("LandingDetectionComponent owner is not a Character"));
        return Result;
    }

    // Increment landing attempts
    LandingAttempts++;

    // Get current speed
    float CurrentSpeed = OwnerCharacter->GetVelocity().Size();
    Result.LandingSpeed = CurrentSpeed;

    // Perform ground trace
    FVector StartLocation = OwnerCharacter->GetActorLocation();
    FVector EndLocation = StartLocation - FVector(0, 0, LandingTraceDistance);
    FHitResult HitResult;

    if (!PerformGroundTrace(StartLocation, EndLocation, HitResult))
    {
        // No ground detected - miss
        UE_LOG(LogBirdDive, Log, TEXT("Landing check: No ground detected"));
        return Result;
    }

    // Calculate distance from target center
    float DistanceFromCenter = CalculateDistanceFromCenter(HitResult.Location);
    Result.DistanceFromCenter = DistanceFromCenter;

    // Calculate landing quality
    Result.Quality = CalculateLandingQuality(HitResult, CurrentSpeed);

    // Calculate accuracy factor
    float AccuracyFactor = CalculateAccuracyFactor(DistanceFromCenter);

    // Calculate base score
    Result.BaseScore = CalculateBaseScore(Result.Quality, AccuracyFactor);

    // Calculate bonus score (speed bonus for controlled landings)
    if (Result.Quality != ELandingQuality::Miss && CurrentSpeed < LandingSpeedThreshold)
    {
        float SpeedBonus = FMath::Max(0.0f, (LandingSpeedThreshold - CurrentSpeed) / LandingSpeedThreshold);
        Result.BonusScore = FMath::RoundToInt(Result.BaseScore * SpeedBonus * 0.5f);
    }

    // Update statistics
    if (Result.Quality != ELandingQuality::Miss)
    {
        SuccessfulLandings++;
    }

    // Fire event
    OnLandingDetected.Broadcast(Result);

    UE_LOG(LogBirdDive, Log, TEXT("Landing detected: Quality=%d, Distance=%.1f, Speed=%.1f, Score=%d+%d"), 
           static_cast<int32>(Result.Quality), Result.DistanceFromCenter, Result.LandingSpeed, 
           Result.BaseScore, Result.BonusScore);

    return Result;
}

bool ULandingDetectionComponent::IsNearGround(float& OutDistance) const
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        OutDistance = -1.0f;
        return false;
    }

    // Perform ground trace
    FVector StartLocation = OwnerCharacter->GetActorLocation();
    FVector EndLocation = StartLocation - FVector(0, 0, LandingTraceDistance);
    FHitResult HitResult;

    if (PerformGroundTrace(StartLocation, EndLocation, HitResult))
    {
        OutDistance = HitResult.Distance;
        return true;
    }

    OutDistance = LandingTraceDistance; // Max distance if no ground found
    return false;
}

float ULandingDetectionComponent::GetCurrentLandingSpeed() const
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return 0.0f;
    }

    return OwnerCharacter->GetVelocity().Size();
}

bool ULandingDetectionComponent::IsSafeLandingSpeed() const
{
    return GetCurrentLandingSpeed() <= LandingSpeedThreshold;
}

ELandingQuality ULandingDetectionComponent::CalculateLandingQuality(const FHitResult& HitResult, float LandingSpeed) const
{
    // Calculate distance from target center
    float Distance = CalculateDistanceFromCenter(HitResult.Location);

    // Speed factor (penalty for high-speed landings)
    bool bSafeSpeed = LandingSpeed <= LandingSpeedThreshold;

    // Determine quality based on distance and speed
    if (Distance <= PerfectLandingRadius && bSafeSpeed)
    {
        return ELandingQuality::Perfect;
    }
    else if (Distance <= GoodLandingRadius && bSafeSpeed)
    {
        return ELandingQuality::Good;
    }
    else if (Distance <= PoorLandingRadius)
    {
        return ELandingQuality::Poor;
    }
    else
    {
        return ELandingQuality::Miss;
    }
}

int32 ULandingDetectionComponent::CalculateBaseScore(ELandingQuality Quality, float Accuracy) const
{
    int32 BaseScore = 0;

    // Base score by quality
    switch (Quality)
    {
        case ELandingQuality::Perfect:
            BaseScore = 1000;
            break;
        case ELandingQuality::Good:
            BaseScore = 600;
            break;
        case ELandingQuality::Poor:
            BaseScore = 200;
            break;
        case ELandingQuality::Miss:
        default:
            BaseScore = 0;
            break;
    }

    // Apply accuracy multiplier
    BaseScore = FMath::RoundToInt(BaseScore * FMath::Clamp(Accuracy, 0.0f, 1.0f));

    return FMath::Max(0, BaseScore);
}

bool ULandingDetectionComponent::PerformGroundTrace(const FVector& Start, const FVector& End, FHitResult& OutHit) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    // Set up trace parameters
    FCollisionQueryParams TraceParams;
    TraceParams.bTraceComplex = false;
    TraceParams.bReturnPhysicalMaterial = true;
    TraceParams.AddIgnoredActor(GetOwner());

    // Perform line trace
    bool bHit = World->LineTraceSingleByChannel(
        OutHit,
        Start,
        End,
        ECollisionChannel::ECC_WorldStatic,
        TraceParams
    );

    // Optional: Draw debug line in development builds
    #if !UE_BUILD_SHIPPING
    if (GetOwner() && GetOwner()->GetClass()->HasAnyClassFlags(CLASS_Config))
    {
        FColor DebugColor = bHit ? FColor::Green : FColor::Red;
        DrawDebugLine(World, Start, End, DebugColor, false, 1.0f, 0, 2.0f);
        
        if (bHit)
        {
            DrawDebugSphere(World, OutHit.Location, 25.0f, 8, FColor::Yellow, false, 1.0f);
        }
    }
    #endif

    return bHit;
}

float ULandingDetectionComponent::CalculateDistanceFromCenter(const FVector& HitLocation) const
{
    // Calculate 2D distance (ignore Z coordinate)
    FVector2D HitLocation2D(HitLocation.X, HitLocation.Y);
    FVector2D TargetCenter2D(TargetCenter.X, TargetCenter.Y);
    
    return FVector2D::Distance(HitLocation2D, TargetCenter2D);
}

float ULandingDetectionComponent::CalculateAccuracyFactor(float Distance) const
{
    // Perfect accuracy at center, decreasing linearly to poor landing radius
    if (Distance <= PerfectLandingRadius)
    {
        return 1.0f;
    }
    else if (Distance <= PoorLandingRadius)
    {
        return FMath::Max(0.0f, 1.0f - (Distance - PerfectLandingRadius) / (PoorLandingRadius - PerfectLandingRadius));
    }
    else
    {
        return 0.0f;
    }
}