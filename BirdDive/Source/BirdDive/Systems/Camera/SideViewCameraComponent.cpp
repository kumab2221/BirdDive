#include "SideViewCameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// Constructor
USideViewCameraComponent::USideViewCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	// Initialize default values
	Settings = FSideViewCameraSettings();
	TargetActor = nullptr;
	PreviousTargetPosition = FVector::ZeroVector;
	TargetVelocity = FVector::ZeroVector;
	CurrentCameraPosition = FVector::ZeroVector;
	TargetCameraPosition = FVector::ZeroVector;
	bIsInitialized = false;
	bHasValidTarget = false;
	LastUpdateTime = 0.0f;
	
	// Create camera component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	if (CameraComponent)
	{
		CameraComponent->SetupAttachment(this);
		// Set side view orientation (looking from Y axis towards origin)
		CameraComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

// Component lifecycle
void USideViewCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize camera system
	InitializeCamera();
}

void USideViewCameraComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up resources
	Shutdown();
	
	Super::EndPlay(EndPlayReason);
}

void USideViewCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bIsInitialized)
	{
		// Update camera position and following
		UpdateCamera(DeltaTime);
	}
}

// Core camera management
void USideViewCameraComponent::InitializeCamera()
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("SideViewCameraComponent already initialized"));
		return;
	}
	
	// Ensure camera component exists
	if (!CameraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraComponent is null during initialization"));
		return;
	}
	
	// Initialize camera state
	CurrentCameraPosition = GetComponentLocation();
	TargetCameraPosition = CurrentCameraPosition;
	LastUpdateTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	
	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("SideViewCameraComponent initialized with follow distance: %.1f"), Settings.FollowDistance);
}

void USideViewCameraComponent::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}
	
	// Reset state
	ResetCameraState();
	
	bIsInitialized = false;
	UE_LOG(LogTemp, Log, TEXT("SideViewCameraComponent shutdown"));
}

UCameraComponent* USideViewCameraComponent::GetCameraComponent() const
{
	return CameraComponent;
}

// Following system
float USideViewCameraComponent::GetFollowDistance() const
{
	return Settings.FollowDistance;
}

void USideViewCameraComponent::SetFollowDistance(float NewDistance)
{
	// Clamp to reasonable range
	float ClampedDistance = FMath::Clamp(NewDistance, 50.0f, 2000.0f);
	
	Settings.FollowDistance = ClampedDistance;
	Settings.MinFollowDistance = FMath::Min(Settings.MinFollowDistance, ClampedDistance);
	Settings.MaxFollowDistance = FMath::Max(Settings.MaxFollowDistance, ClampedDistance);
	
	UE_LOG(LogTemp, Log, TEXT("Follow distance set to: %.1f"), ClampedDistance);
}

float USideViewCameraComponent::GetFollowHeight() const
{
	return Settings.FollowHeight;
}

void USideViewCameraComponent::SetFollowHeight(float NewHeight)
{
	Settings.FollowHeight = NewHeight;
	UE_LOG(LogTemp, Log, TEXT("Follow height set to: %.1f"), NewHeight);
}

void USideViewCameraComponent::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;
	bHasValidTarget = IsValidTarget();
	
	if (bHasValidTarget)
	{
		PreviousTargetPosition = TargetActor->GetActorLocation();
		TargetVelocity = FVector::ZeroVector;
		UE_LOG(LogTemp, Log, TEXT("Target actor set: %s"), *TargetActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid target actor set"));
		ResetCameraState();
	}
}

AActor* USideViewCameraComponent::GetTargetActor() const
{
	return TargetActor;
}

void USideViewCameraComponent::UpdateCamera(float DeltaTime)
{
	if (!bIsInitialized || !CameraComponent)
	{
		return;
	}
	
	// Update target validity
	bHasValidTarget = IsValidTarget();
	
	if (bHasValidTarget)
	{
		// Update target velocity
		TargetVelocity = CalculateTargetVelocity(DeltaTime);
		
		// Update follow position
		UpdateFollowPosition(DeltaTime);
		
		// Update look-ahead
		UpdateLookAhead(DeltaTime);
		
		// Update camera transform
		UpdateCameraTransform(DeltaTime);
	}
	
	LastUpdateTime = GetWorld() ? GetWorld()->GetTimeSeconds() : LastUpdateTime + DeltaTime;
}

// Look-ahead system
float USideViewCameraComponent::GetLookAheadStrength() const
{
	return Settings.LookAheadStrength;
}

void USideViewCameraComponent::SetLookAheadStrength(float Strength)
{
	// Clamp to reasonable range
	float ClampedStrength = FMath::Clamp(Strength, 0.0f, 5.0f);
	Settings.LookAheadStrength = ClampedStrength;
	
	UE_LOG(LogTemp, Log, TEXT("Look-ahead strength set to: %.2f"), ClampedStrength);
}

FVector USideViewCameraComponent::CalculateLookAheadPosition() const
{
	if (!bHasValidTarget)
	{
		return FVector::ZeroVector;
	}
	
	FVector TargetLocation = TargetActor->GetActorLocation();
	
	// Calculate look-ahead offset based on velocity
	FVector LookAheadOffset = TargetVelocity * Settings.LookAheadStrength * Settings.LookAheadDistance / 1000.0f;
	
	// Limit maximum look-ahead distance
	if (LookAheadOffset.Size() > Settings.MaxLookAheadDistance)
	{
		LookAheadOffset = LookAheadOffset.GetSafeNormal() * Settings.MaxLookAheadDistance;
	}
	
	return TargetLocation + LookAheadOffset;
}

void USideViewCameraComponent::UpdateLookAhead(float DeltaTime)
{
	if (!bHasValidTarget)
	{
		return;
	}
	
	// Calculate look-ahead position
	FVector LookAheadPosition = CalculateLookAheadPosition();
	
	// Use look-ahead position as target for camera calculation
	TargetCameraPosition = CalculateBaseFollowPosition() + (LookAheadPosition - TargetActor->GetActorLocation());
}

// Boundary system
void USideViewCameraComponent::SetBoundaryLimits(const FVector& MinBounds, const FVector& MaxBounds)
{
	Settings.BoundaryMin = MinBounds;
	Settings.BoundaryMax = MaxBounds;
	
	UE_LOG(LogTemp, Log, TEXT("Boundary limits set: Min(%.1f,%.1f,%.1f) Max(%.1f,%.1f,%.1f)"), 
		MinBounds.X, MinBounds.Y, MinBounds.Z, MaxBounds.X, MaxBounds.Y, MaxBounds.Z);
}

bool USideViewCameraComponent::IsPositionInBounds(const FVector& Position) const
{
	return Position.X >= Settings.BoundaryMin.X && Position.X <= Settings.BoundaryMax.X &&
		   Position.Y >= Settings.BoundaryMin.Y && Position.Y <= Settings.BoundaryMax.Y &&
		   Position.Z >= Settings.BoundaryMin.Z && Position.Z <= Settings.BoundaryMax.Z;
}

FVector USideViewCameraComponent::ClampToBoundary(const FVector& Position) const
{
	FVector ClampedPosition;
	ClampedPosition.X = FMath::Clamp(Position.X, Settings.BoundaryMin.X, Settings.BoundaryMax.X);
	ClampedPosition.Y = FMath::Clamp(Position.Y, Settings.BoundaryMin.Y, Settings.BoundaryMax.Y);
	ClampedPosition.Z = FMath::Clamp(Position.Z, Settings.BoundaryMin.Z, Settings.BoundaryMax.Z);
	
	return ClampedPosition;
}

// Settings access
void USideViewCameraComponent::ApplySettings(const FSideViewCameraSettings& NewSettings)
{
	Settings = NewSettings;
	
	// Apply settings immediately if initialized
	if (bIsInitialized)
	{
		SetFollowDistance(Settings.FollowDistance);
		SetFollowHeight(Settings.FollowHeight);
		SetLookAheadStrength(Settings.LookAheadStrength);
	}
	
	UE_LOG(LogTemp, Log, TEXT("SideViewCamera settings applied"));
}

const FSideViewCameraSettings& USideViewCameraComponent::GetSettings() const
{
	return Settings;
}

// Private helper methods
void USideViewCameraComponent::UpdateFollowPosition(float DeltaTime)
{
	if (!bHasValidTarget)
	{
		return;
	}
	
	// Calculate base follow position (side view)
	FVector BasePosition = CalculateBaseFollowPosition();
	
	// Set target camera position
	TargetCameraPosition = BasePosition;
	
	// Apply boundary clamping
	TargetCameraPosition = ClampToBoundary(TargetCameraPosition);
}

void USideViewCameraComponent::UpdateCameraTransform(float DeltaTime)
{
	if (!CameraComponent)
	{
		return;
	}
	
	// Smoothly interpolate to target position
	CurrentCameraPosition = FMath::VInterpTo(CurrentCameraPosition, TargetCameraPosition, 
											 DeltaTime, Settings.FollowSmoothSpeed);
	
	// Update component location
	SetWorldLocation(CurrentCameraPosition);
	
	// Ensure side view orientation is maintained
	if (bHasValidTarget)
	{
		FVector TargetLocation = TargetActor->GetActorLocation();
		FVector LookDirection = (TargetLocation - CurrentCameraPosition).GetSafeNormal();
		
		// For side view, we want to look from the side (Y axis)
		FRotator LookRotation = FRotator(0.0f, -90.0f, 0.0f);
		CameraComponent->SetWorldRotation(LookRotation);
	}
}

FVector USideViewCameraComponent::CalculateBaseFollowPosition() const
{
	if (!bHasValidTarget)
	{
		return CurrentCameraPosition;
	}
	
	FVector TargetLocation = TargetActor->GetActorLocation();
	
	// Side view position: offset on Y axis (side) from target
	FVector FollowPosition = TargetLocation;
	FollowPosition.Y -= Settings.FollowDistance; // Move to the side (negative Y)
	FollowPosition.Z += Settings.FollowHeight;   // Apply height offset
	
	return FollowPosition;
}

FVector USideViewCameraComponent::CalculateTargetVelocity(float DeltaTime)
{
	if (!bHasValidTarget || DeltaTime <= 0.0f)
	{
		return TargetVelocity; // Return previous velocity
	}
	
	FVector CurrentTargetPosition = TargetActor->GetActorLocation();
	FVector NewVelocity = (CurrentTargetPosition - PreviousTargetPosition) / DeltaTime;
	
	// Smooth velocity to avoid jittering
	TargetVelocity = FMath::VInterpTo(TargetVelocity, NewVelocity, DeltaTime, 5.0f);
	
	// Update previous position for next frame
	PreviousTargetPosition = CurrentTargetPosition;
	
	return TargetVelocity;
}

bool USideViewCameraComponent::IsValidTarget() const
{
	return TargetActor != nullptr && IsValid(TargetActor);
}

void USideViewCameraComponent::ResetCameraState()
{
	TargetActor = nullptr;
	PreviousTargetPosition = FVector::ZeroVector;
	TargetVelocity = FVector::ZeroVector;
	bHasValidTarget = false;
}