#include "WindForceComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../BirdDiveTypes.h"

// Permutation table for Perlin noise (standard implementation)
const int32 UWindForceComponent::PermutationTable[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
    18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
    250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
    189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
    228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
    107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

UWindForceComponent::UWindForceComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Initialize default values
    WindStrength = 1.0f;
    NoiseScale = 0.1f;
    TimeScale = 1.0f;
    BaseWindDirection = FVector(1.0f, 0.0f, 0.0f);
    HeightInfluence = 1.0f;
    MaxWindForce = 1000.0f;
}

void UWindForceComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Normalize base wind direction
    BaseWindDirection.Normalize();
}

FVector UWindForceComponent::CalculateWindForce(const FVector& Position, float Height) const
{
    // Enhanced input validation
    if (!IsValidInput(Position, Height))
    {
        return FVector::ZeroVector;
    }
    
    // Get current time for animation
    float CurrentTime = 0.0f;
    if (UWorld* World = GetWorld())
    {
        CurrentTime = World->GetTimeSeconds();
    }
    
    // Calculate Perlin noise at position with time animation
    FVector NoisePosition = Position * NoiseScale;
    float NoiseX = GetPerlinNoiseValue(NoisePosition + FVector(CurrentTime * TimeScale, 0.0f, 0.0f), CurrentTime);
    float NoiseY = GetPerlinNoiseValue(NoisePosition + FVector(0.0f, CurrentTime * TimeScale, 0.0f), CurrentTime);
    float NoiseZ = GetPerlinNoiseValue(NoisePosition + FVector(0.0f, 0.0f, CurrentTime * TimeScale * 0.5f), CurrentTime);
    
    // Create wind direction with noise variation
    FVector WindDirection = BaseWindDirection;
    WindDirection.X += NoiseX * HORIZONTAL_WIND_SCALE;
    WindDirection.Y += NoiseY * HORIZONTAL_WIND_SCALE;
    WindDirection.Z += NoiseZ * VERTICAL_WIND_SCALE; // Less vertical variation
    
    // Normalize to prevent extreme values
    if (!WindDirection.IsNearlyZero())
    {
        WindDirection.Normalize();
    }
    else
    {
        WindDirection = BaseWindDirection;
    }
    
    // Calculate height influence (higher = stronger wind)
    float HeightFactor = FMath::Max(MIN_HEIGHT_FACTOR, Height / HEIGHT_NORMALIZATION);
    HeightFactor = FMath::Pow(HeightFactor, HeightInfluence);
    
    // Calculate final wind force
    float ForceMagnitude = WindStrength * HeightFactor * FMath::Abs(NoiseX + NoiseY) * 0.5f;
    ForceMagnitude = FMath::Clamp(ForceMagnitude, 0.0f, MaxWindForce);
    
    FVector WindForce = WindDirection * ForceMagnitude;
    
    // Final safety check
    if (!FMath::IsFinite(WindForce.Size()))
    {
        return FVector::ZeroVector;
    }
    
    return WindForce;
}

float UWindForceComponent::GetPerlinNoiseValue(const FVector& Position, float Time) const
{
    // Safety check
    if (!FMath::IsFinite(Position.X) || !FMath::IsFinite(Position.Y) || !FMath::IsFinite(Position.Z))
    {
        return 0.0f;
    }
    
    // Use time for 4D noise effect
    float Result = SimplePerlinNoise(Position.X, Position.Y, Position.Z + Time * 0.1f);
    
    // Clamp to valid range
    return FMath::Clamp(Result, -1.0f, 1.0f);
}

float UWindForceComponent::SimplePerlinNoise(float x, float y, float z) const
{
    // Find unit cube that contains point
    int32 X = static_cast<int32>(FMath::Floor(x)) & 255;
    int32 Y = static_cast<int32>(FMath::Floor(y)) & 255;
    int32 Z = static_cast<int32>(FMath::Floor(z)) & 255;
    
    // Find relative x,y,z of point in cube
    x -= FMath::Floor(x);
    y -= FMath::Floor(y);
    z -= FMath::Floor(z);
    
    // Compute fade curves for each of x,y,z
    float u = Fade(x);
    float v = Fade(y);
    float w = Fade(z);
    
    // Hash coordinates of the 8 cube corners
    int32 A = PermutationTable[X] + Y;
    int32 AA = PermutationTable[A] + Z;
    int32 AB = PermutationTable[A + 1] + Z;
    int32 B = PermutationTable[X + 1] + Y;
    int32 BA = PermutationTable[B] + Z;
    int32 BB = PermutationTable[B + 1] + Z;
    
    // Add blended results from 8 corners of cube
    return Lerp(w, Lerp(v, Lerp(u, Grad(PermutationTable[AA], x, y, z),
                                    Grad(PermutationTable[BA], x - 1, y, z)),
                           Lerp(u, Grad(PermutationTable[AB], x, y - 1, z),
                                    Grad(PermutationTable[BB], x - 1, y - 1, z))),
                   Lerp(v, Lerp(u, Grad(PermutationTable[AA + 1], x, y, z - 1),
                                    Grad(PermutationTable[BA + 1], x - 1, y, z - 1)),
                           Lerp(u, Grad(PermutationTable[AB + 1], x, y - 1, z - 1),
                                    Grad(PermutationTable[BB + 1], x - 1, y - 1, z - 1))));
}

float UWindForceComponent::Fade(float t) const
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float UWindForceComponent::Lerp(float t, float a, float b) const
{
    return a + t * (b - a);
}

float UWindForceComponent::Grad(int hash, float x, float y, float z) const
{
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

bool UWindForceComponent::IsValidInput(const FVector& Position, float Height) const
{
    // Check for NaN or infinite values
    if (!FMath::IsFinite(Position.X) || !FMath::IsFinite(Position.Y) || !FMath::IsFinite(Position.Z))
    {
        return false;
    }
    
    if (!FMath::IsFinite(Height) || Height < 0.0f)
    {
        return false;
    }
    
    return true;
}

void UWindForceComponent::SetDifficultyParameters(EDifficultyLevel Difficulty)
{
    switch (Difficulty)
    {
        case EDifficultyLevel::Easy:
            WindStrength = 0.5f;
            NoiseScale = 0.05f;
            TimeScale = 0.8f;
            break;
            
        case EDifficultyLevel::Normal:
            WindStrength = 1.0f;
            NoiseScale = 0.1f;
            TimeScale = 1.0f;
            break;
            
        case EDifficultyLevel::Hard:
            WindStrength = 2.0f;
            NoiseScale = 0.15f;
            TimeScale = 1.5f;
            break;
    }
}

float UWindForceComponent::GetWindIntensityAtLocation(const FVector& Location, float Height) const
{
    FVector WindForce = CalculateWindForce(Location, Height);
    float Intensity = WindForce.Size() / MaxWindForce;
    return FMath::Clamp(Intensity, 0.0f, 1.0f);
}