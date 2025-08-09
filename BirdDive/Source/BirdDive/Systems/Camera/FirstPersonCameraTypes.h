#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Components/SkeletalMeshComponent.h"
#include "FirstPersonCameraTypes.generated.h"

// First Person Camera Settings structure
USTRUCT(BlueprintType)
struct BIRDDIVE_API FFirstPersonCameraSettings
{
	GENERATED_BODY()

	// FOV settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float BaseFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float MinFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
	float MaxFOV;

	// Speed mapping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float MinSpeedForFOVChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float MaxSpeedForFOVChange;

	// Smoothing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
	float FOVInterpolationSpeed;

	// Post process
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float MaxMotionBlurAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float SpeedThresholdForEffects;

	// Default constructor
	FFirstPersonCameraSettings()
	{
		BaseFOV = 90.0f;
		MinFOV = 60.0f;
		MaxFOV = 120.0f;
		MinSpeedForFOVChange = 100.0f;
		MaxSpeedForFOVChange = 2000.0f;
		FOVInterpolationSpeed = 5.0f;
		MaxMotionBlurAmount = 0.5f;
		SpeedThresholdForEffects = 500.0f;
	}

	// Constructor with parameters
	FFirstPersonCameraSettings(float InBaseFOV, float InMinFOV, float InMaxFOV)
		: BaseFOV(InBaseFOV)
		, MinFOV(InMinFOV)
		, MaxFOV(InMaxFOV)
		, MinSpeedForFOVChange(100.0f)
		, MaxSpeedForFOVChange(2000.0f)
		, FOVInterpolationSpeed(5.0f)
		, MaxMotionBlurAmount(0.5f)
		, SpeedThresholdForEffects(500.0f)
	{
	}
};