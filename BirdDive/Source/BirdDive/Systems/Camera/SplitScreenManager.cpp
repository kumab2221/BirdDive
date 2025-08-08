#include "SplitScreenManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TextureRenderTarget2D.h"

// Constructor
USplitScreenManager::USplitScreenManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	// Initialize default values
	SplitType = EBirdDiveSplitScreenType::Horizontal;
	SplitRatio = 0.5f;
	bIsInitialized = false;
	ScreenSize = FIntPoint(1920, 1080);
}

// Component lifecycle
void USplitScreenManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize split screen system
	InitializeSplitScreen();
}

void USplitScreenManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up resources
	Shutdown();
	
	Super::EndPlay(EndPlayReason);
}

void USplitScreenManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Currently no tick logic needed - split screen is mostly event-driven
}

// Core viewport management
int32 USplitScreenManager::GetViewportCount() const
{
	// Always use 2 viewports for split screen
	return 2;
}

float USplitScreenManager::GetSplitRatio() const
{
	return SplitRatio;
}

bool USplitScreenManager::SetSplitRatio(float NewRatio)
{
	// Clamp ratio to valid range
	float ClampedRatio = FMath::Clamp(NewRatio, 0.2f, 0.8f);
	
	if (FMath::IsNearlyEqual(SplitRatio, ClampedRatio))
	{
		return false; // No change needed
	}
	
	SplitRatio = ClampedRatio;
	
	// Update viewport rectangles if initialized
	if (bIsInitialized)
	{
		UpdateViewportRectangles();
	}
	
	return true;
}

EBirdDiveSplitScreenType USplitScreenManager::GetSplitType() const
{
	return SplitType;
}

void USplitScreenManager::SetSplitType(EBirdDiveSplitScreenType NewType)
{
	if (SplitType == NewType)
	{
		return; // No change needed
	}
	
	SplitType = NewType;
	
	// Update viewport rectangles if initialized
	if (bIsInitialized)
	{
		UpdateViewportRectangles();
	}
	
	UE_LOG(LogTemp, Log, TEXT("Split type changed to %s"), 
		   SplitType == EBirdDiveSplitScreenType::Horizontal ? TEXT("Horizontal") : TEXT("Vertical"));
}

// Camera management
bool USplitScreenManager::AssignCamera(int32 ViewportIndex, ACameraActor* Camera)
{
	if (!IsValidViewportIndex(ViewportIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid viewport index: %d"), ViewportIndex);
		return false;
	}
	
	// Ensure camera array has enough elements
	while (AssignedCameras.Num() <= ViewportIndex)
	{
		AssignedCameras.Add(nullptr);
	}
	
	// Assign the camera
	AssignedCameras[ViewportIndex] = Camera;
	
	// Update viewport info if initialized
	if (bIsInitialized && Viewports.IsValidIndex(ViewportIndex))
	{
		Viewports[ViewportIndex].AssignedCamera = Camera;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Camera assigned to viewport %d"), ViewportIndex);
	return true;
}

ACameraActor* USplitScreenManager::GetAssignedCamera(int32 ViewportIndex) const
{
	if (!IsValidViewportIndex(ViewportIndex) || !AssignedCameras.IsValidIndex(ViewportIndex))
	{
		return nullptr;
	}
	
	return AssignedCameras[ViewportIndex];
}

// Viewport information access
FBirdDiveViewportInfo USplitScreenManager::GetViewportInfo(int32 Index) const
{
	if (!IsValidViewportIndex(Index) || !Viewports.IsValidIndex(Index))
	{
		// Return empty viewport info for invalid indices
		return FBirdDiveViewportInfo();
	}
	
	return Viewports[Index];
}

// System management
void USplitScreenManager::InitializeSplitScreen()
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("SplitScreenManager already initialized"));
		return;
	}
	
	// Initialize viewport array
	const int32 ViewportCount = GetViewportCount();
	Viewports.SetNum(ViewportCount);
	AssignedCameras.SetNum(ViewportCount);
	
	// Initialize each viewport
	for (int32 i = 0; i < ViewportCount; ++i)
	{
		AssignedCameras[i] = nullptr;
	}
	
	// Calculate initial viewport rectangles
	UpdateViewportRectangles();
	
	// Create render targets
	CreateRenderTargets();
	
	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("SplitScreenManager initialized with %d viewports"), ViewportCount);
}

void USplitScreenManager::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}
	
	// Clean up render targets
	CleanupRenderTargets();
	
	// Clear viewport data
	Viewports.Empty();
	AssignedCameras.Empty();
	
	bIsInitialized = false;
	UE_LOG(LogTemp, Log, TEXT("SplitScreenManager shutdown"));
}

// Private helper methods
void USplitScreenManager::UpdateViewportRectangles()
{
	const int32 ViewportCount = GetViewportCount();
	
	for (int32 i = 0; i < ViewportCount && i < Viewports.Num(); ++i)
	{
		FIntRect ViewportRect;
		CalculateViewportRect(i, ViewportRect);
		
		Viewports[i].ViewportRect = ViewportRect;
		Viewports[i].AspectRatio = CalculateAspectRatio(ViewportRect);
		
		// Preserve existing camera assignment and render target
		if (AssignedCameras.IsValidIndex(i))
		{
			Viewports[i].AssignedCamera = AssignedCameras[i];
		}
		if (RenderTargets.IsValidIndex(i))
		{
			Viewports[i].RenderTarget = RenderTargets[i];
		}
	}
}

void USplitScreenManager::CalculateViewportRect(int32 ViewportIndex, FIntRect& OutRect) const
{
	if (!IsValidViewportIndex(ViewportIndex))
	{
		OutRect = FIntRect(0, 0, 0, 0);
		return;
	}
	
	switch (SplitType)
	{
		case EBirdDiveSplitScreenType::Horizontal:
			{
				// Top/Bottom split
				const int32 TopHeight = FMath::RoundToInt(ScreenSize.Y * SplitRatio);
				const int32 BottomHeight = ScreenSize.Y - TopHeight;
				
				if (ViewportIndex == 0)
				{
					// Top viewport
					OutRect = FIntRect(0, 0, ScreenSize.X, TopHeight);
				}
				else
				{
					// Bottom viewport
					OutRect = FIntRect(0, TopHeight, ScreenSize.X, ScreenSize.Y);
				}
			}
			break;
			
		case EBirdDiveSplitScreenType::Vertical:
			{
				// Left/Right split
				const int32 LeftWidth = FMath::RoundToInt(ScreenSize.X * SplitRatio);
				const int32 RightWidth = ScreenSize.X - LeftWidth;
				
				if (ViewportIndex == 0)
				{
					// Left viewport
					OutRect = FIntRect(0, 0, LeftWidth, ScreenSize.Y);
				}
				else
				{
					// Right viewport
					OutRect = FIntRect(LeftWidth, 0, ScreenSize.X, ScreenSize.Y);
				}
			}
			break;
			
		default:
			OutRect = FIntRect(0, 0, ScreenSize.X, ScreenSize.Y);
			break;
	}
}

float USplitScreenManager::CalculateAspectRatio(const FIntRect& ViewportRect) const
{
	if (ViewportRect.Height() == 0)
	{
		return 16.0f / 9.0f; // Default aspect ratio
	}
	
	return static_cast<float>(ViewportRect.Width()) / static_cast<float>(ViewportRect.Height());
}

bool USplitScreenManager::IsValidViewportIndex(int32 Index) const
{
	return Index >= 0 && Index < GetViewportCount();
}

void USplitScreenManager::CreateRenderTargets()
{
	const int32 ViewportCount = GetViewportCount();
	RenderTargets.SetNum(ViewportCount);
	
	for (int32 i = 0; i < ViewportCount; ++i)
	{
		// Calculate render target size based on viewport
		FIntRect ViewportRect;
		CalculateViewportRect(i, ViewportRect);
		
		int32 RenderTargetSizeX = FMath::Max(ViewportRect.Width(), 256);
		int32 RenderTargetSizeY = FMath::Max(ViewportRect.Height(), 256);
		
		// Create render target
		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
		if (RenderTarget)
		{
			RenderTarget->InitAutoFormat(RenderTargetSizeX, RenderTargetSizeY);
			RenderTarget->UpdateResourceImmediate(true);
			
			RenderTargets[i] = RenderTarget;
			
			// Update viewport info
			if (Viewports.IsValidIndex(i))
			{
				Viewports[i].RenderTarget = RenderTarget;
			}
			
			UE_LOG(LogTemp, Log, TEXT("Created render target %dx%d for viewport %d"), 
				   RenderTargetSizeX, RenderTargetSizeY, i);
		}
	}
}

void USplitScreenManager::CleanupRenderTargets()
{
	for (UTextureRenderTarget2D* RenderTarget : RenderTargets)
	{
		if (RenderTarget)
		{
			RenderTarget->ConditionalBeginDestroy();
		}
	}
	
	RenderTargets.Empty();
	
	// Clear render targets from viewport info
	for (FBirdDiveViewportInfo& ViewportInfo : Viewports)
	{
		ViewportInfo.RenderTarget = nullptr;
	}
}