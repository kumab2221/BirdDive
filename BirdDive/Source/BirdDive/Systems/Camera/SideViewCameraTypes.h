#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Components/SkeletalMeshComponent.h"
#include "SideViewCameraTypes.generated.h"

// Side View Camera Settings structure
USTRUCT(BlueprintType)
struct BIRDDIVE_API FSideViewCameraSettings
{
	GENERATED_BODY()

	// Following settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float FollowDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float FollowHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float MinFollowDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
	float MaxFollowDistance;

	// Look-ahead settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
	float LookAheadDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
	float LookAheadStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
	float MaxLookAheadDistance;

	// Smoothing settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
	float FollowSmoothSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
	float LookAheadSmoothSpeed;

	// Boundary settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boundary")
	FVector BoundaryMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boundary")
	FVector BoundaryMax;

	// Screen keep settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
	float ScreenMargin; // Margin from screen edge (0.0-1.0)

	// Default constructor
	FSideViewCameraSettings()
	{
		FollowDistance = 500.0f;
		FollowHeight = 0.0f;
		MinFollowDistance = 200.0f;
		MaxFollowDistance = 1000.0f;
		LookAheadDistance = 200.0f;
		LookAheadStrength = 1.0f;
		MaxLookAheadDistance = 400.0f;
		FollowSmoothSpeed = 5.0f;
		LookAheadSmoothSpeed = 3.0f;
		BoundaryMin = FVector(-5000, -5000, -1000);
		BoundaryMax = FVector(5000, 5000, 2000);
		ScreenMargin = 0.1f;
	}

	// Constructor with parameters
	FSideViewCameraSettings(float InFollowDistance, float InFollowHeight, float InLookAheadStrength)
		: FollowDistance(InFollowDistance)
		, FollowHeight(InFollowHeight)
		, MinFollowDistance(200.0f)
		, MaxFollowDistance(1000.0f)
		, LookAheadDistance(200.0f)
		, LookAheadStrength(InLookAheadStrength)
		, MaxLookAheadDistance(400.0f)
		, FollowSmoothSpeed(5.0f)
		, LookAheadSmoothSpeed(3.0f)
		, BoundaryMin(FVector(-5000, -5000, -1000))
		, BoundaryMax(FVector(5000, 5000, 2000))
		, ScreenMargin(0.1f)
	{
	}
};