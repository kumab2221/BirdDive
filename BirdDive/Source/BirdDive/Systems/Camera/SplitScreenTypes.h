#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Camera/CameraActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SplitScreenTypes.generated.h"

// Split screen orientation type
UENUM(BlueprintType)
enum class EBirdDiveSplitScreenType : uint8
{
	Horizontal  UMETA(DisplayName = "Horizontal (Top/Bottom)"),
	Vertical    UMETA(DisplayName = "Vertical (Left/Right)")
};

// Viewport information structure
USTRUCT(BlueprintType)
struct BIRDDIVE_API FBirdDiveViewportInfo
{
	GENERATED_BODY()

	// Viewport rectangle on screen
	UPROPERTY(BlueprintReadOnly, Category = "Viewport")
	FIntRect ViewportRect;

	// Aspect ratio for this viewport
	UPROPERTY(BlueprintReadOnly, Category = "Viewport")
	float AspectRatio;

	// Render target for this viewport
	UPROPERTY(BlueprintReadOnly, Category = "Viewport")
	class UTextureRenderTarget2D* RenderTarget;

	// Camera assigned to this viewport
	UPROPERTY(BlueprintReadOnly, Category = "Viewport")
	class ACameraActor* AssignedCamera;

	// Default constructor
	FBirdDiveViewportInfo()
	{
		ViewportRect = FIntRect(0, 0, 0, 0);
		AspectRatio = 16.0f / 9.0f;
		RenderTarget = nullptr;
		AssignedCamera = nullptr;
	}

	// Constructor with parameters
	FBirdDiveViewportInfo(const FIntRect& InRect, float InAspectRatio)
		: ViewportRect(InRect)
		, AspectRatio(InAspectRatio)
		, RenderTarget(nullptr)
		, AssignedCamera(nullptr)
	{
	}
};