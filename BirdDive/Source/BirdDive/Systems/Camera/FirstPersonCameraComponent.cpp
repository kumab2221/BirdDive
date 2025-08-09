#include "FirstPersonCameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// Constructor
UFirstPersonCameraComponent::UFirstPersonCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	// Initialize default values
	Settings = FFirstPersonCameraSettings();
	CurrentFOV = Settings.BaseFOV;
	TargetFOV = Settings.BaseFOV;
	bSpeedBasedFOVEnabled = true;
	bPostProcessEffectsEnabled = true;
	bIsInitialized = false;
	
	// Attachment info
	AttachedMesh = nullptr;
	AttachSocketName = NAME_None;
	bIsAttached = false;
	
	// Create camera component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	if (CameraComponent)
	{
		CameraComponent->SetupAttachment(this);
		CameraComponent->SetFieldOfView(Settings.BaseFOV);
	}
	
	// Create post process component
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	if (PostProcessComponent)
	{
		PostProcessComponent->SetupAttachment(this);
	}
}

// Component lifecycle
void UFirstPersonCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize camera system
	InitializeCamera();
}

void UFirstPersonCameraComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up resources
	Shutdown();
	
	Super::EndPlay(EndPlayReason);
}

void UFirstPersonCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bIsInitialized)
	{
		// Update FOV smoothly
		UpdateCameraFOV(DeltaTime);
	}
}

// Core camera management
void UFirstPersonCameraComponent::InitializeCamera()
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("FirstPersonCameraComponent already initialized"));
		return;
	}
	
	// Ensure camera component exists
	if (!CameraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraComponent is null during initialization"));
		return;
	}
	
	// Set initial FOV
	CurrentFOV = Settings.BaseFOV;
	TargetFOV = Settings.BaseFOV;
	CameraComponent->SetFieldOfView(CurrentFOV);
	
	// Initialize post-process effects
	if (PostProcessComponent && bPostProcessEffectsEnabled)
	{
		// Set default post-process settings
		PostProcessComponent->Settings.bOverride_MotionBlurAmount = true;
		PostProcessComponent->Settings.MotionBlurAmount = 0.0f;
	}
	
	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("FirstPersonCameraComponent initialized with FOV: %.1f"), CurrentFOV);
}

void UFirstPersonCameraComponent::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}
	
	// Detach from parent if attached
	if (bIsAttached)
	{
		DetachFromAttachedMesh();
	}
	
	bIsInitialized = false;
	UE_LOG(LogTemp, Log, TEXT("FirstPersonCameraComponent shutdown"));
}

UCameraComponent* UFirstPersonCameraComponent::GetCameraComponent() const
{
	return CameraComponent;
}

// FOV management
float UFirstPersonCameraComponent::GetBaseFOV() const
{
	return Settings.BaseFOV;
}

void UFirstPersonCameraComponent::SetBaseFOV(float NewFOV)
{
	// Clamp to reasonable range
	float ClampedFOV = FMath::Clamp(NewFOV, 30.0f, 160.0f);
	
	Settings.BaseFOV = ClampedFOV;
	
	// Update current FOV if not using speed-based adjustment
	if (!bSpeedBasedFOVEnabled)
	{
		TargetFOV = ClampedFOV;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Base FOV set to: %.1f"), ClampedFOV);
}

float UFirstPersonCameraComponent::GetMinFOV() const
{
	return Settings.MinFOV;
}

float UFirstPersonCameraComponent::GetMaxFOV() const
{
	return Settings.MaxFOV;
}

void UFirstPersonCameraComponent::SetFOVRange(float NewMinFOV, float NewMaxFOV)
{
	// Ensure min is less than max
	if (NewMinFOV > NewMaxFOV)
	{
		Swap(NewMinFOV, NewMaxFOV);
	}
	
	// Clamp to reasonable range
	Settings.MinFOV = FMath::Clamp(NewMinFOV, 30.0f, 160.0f);
	Settings.MaxFOV = FMath::Clamp(NewMaxFOV, 30.0f, 160.0f);
	
	UE_LOG(LogTemp, Log, TEXT("FOV range set to: %.1f - %.1f"), Settings.MinFOV, Settings.MaxFOV);
}

float UFirstPersonCameraComponent::GetCurrentFOV() const
{
	return CurrentFOV;
}

void UFirstPersonCameraComponent::UpdateFOVBasedOnSpeed(float CurrentSpeed)
{
	if (!bSpeedBasedFOVEnabled)
	{
		return;
	}
	
	// Calculate target FOV based on speed
	TargetFOV = CalculateFOVFromSpeed(CurrentSpeed);
}

void UFirstPersonCameraComponent::EnableSpeedBasedFOV(bool bEnable)
{
	bSpeedBasedFOVEnabled = bEnable;
	
	if (!bEnable)
	{
		// Revert to base FOV
		TargetFOV = Settings.BaseFOV;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Speed-based FOV %s"), bEnable ? TEXT("enabled") : TEXT("disabled"));
}

// Attachment functionality
bool UFirstPersonCameraComponent::AttachToSocketOrBone(USkeletalMeshComponent* Mesh, const FName& SocketName)
{
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot attach to null skeletal mesh"));
		return false;
	}
	
	// Check if socket exists
	if (!Mesh->DoesSocketExist(SocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket '%s' does not exist on skeletal mesh"), *SocketName.ToString());
		return false;
	}
	
	// Detach from previous parent if attached
	if (bIsAttached)
	{
		DetachFromAttachedMesh();
	}
	
	// Attach to socket
	AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	
	// Store attachment info
	AttachedMesh = Mesh;
	AttachSocketName = SocketName;
	bIsAttached = true;
	
	UE_LOG(LogTemp, Log, TEXT("FirstPersonCamera attached to socket '%s'"), *SocketName.ToString());
	return true;
}

void UFirstPersonCameraComponent::DetachFromAttachedMesh()
{
	if (!bIsAttached)
	{
		return;
	}
	
	// Detach from parent
	DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	// Clear attachment info
	AttachedMesh = nullptr;
	AttachSocketName = NAME_None;
	bIsAttached = false;
	
	UE_LOG(LogTemp, Log, TEXT("FirstPersonCamera detached from parent"));
}

// Post-process effects
void UFirstPersonCameraComponent::SetPostProcessEffects(bool bEnable)
{
	bPostProcessEffectsEnabled = bEnable;
	
	if (PostProcessComponent)
	{
		PostProcessComponent->SetVisibility(bEnable);
		
		if (!bEnable)
		{
			// Reset motion blur when disabled
			PostProcessComponent->Settings.MotionBlurAmount = 0.0f;
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Post-process effects %s"), bEnable ? TEXT("enabled") : TEXT("disabled"));
}

bool UFirstPersonCameraComponent::ArePostProcessEffectsEnabled() const
{
	return bPostProcessEffectsEnabled;
}

void UFirstPersonCameraComponent::UpdatePostProcessEffects(float Speed, float DeltaTime)
{
	if (!bPostProcessEffectsEnabled || !PostProcessComponent)
	{
		return;
	}
	
	// Update motion blur based on speed
	UpdateMotionBlur(Speed);
}

void UFirstPersonCameraComponent::SetMotionBlurIntensity(float Intensity)
{
	if (!PostProcessComponent)
	{
		return;
	}
	
	float ClampedIntensity = FMath::Clamp(Intensity, 0.0f, Settings.MaxMotionBlurAmount);
	PostProcessComponent->Settings.MotionBlurAmount = ClampedIntensity;
	
	UE_LOG(LogTemp, VeryVerbose, TEXT("Motion blur intensity set to: %.2f"), ClampedIntensity);
}

float UFirstPersonCameraComponent::GetMotionBlurIntensity() const
{
	if (!PostProcessComponent)
	{
		return 0.0f;
	}
	
	return PostProcessComponent->Settings.MotionBlurAmount;
}

// Settings access
void UFirstPersonCameraComponent::ApplySettings(const FFirstPersonCameraSettings& NewSettings)
{
	Settings = NewSettings;
	
	// Apply new settings immediately
	if (bIsInitialized)
	{
		SetBaseFOV(Settings.BaseFOV);
		SetFOVRange(Settings.MinFOV, Settings.MaxFOV);
	}
	
	UE_LOG(LogTemp, Log, TEXT("FirstPersonCamera settings applied"));
}

const FFirstPersonCameraSettings& UFirstPersonCameraComponent::GetSettings() const
{
	return Settings;
}

// Private helper methods
void UFirstPersonCameraComponent::UpdateCameraFOV(float DeltaTime)
{
	if (!CameraComponent)
	{
		return;
	}
	
	// Smoothly interpolate to target FOV
	if (!FMath::IsNearlyEqual(CurrentFOV, TargetFOV, 0.1f))
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, Settings.FOVInterpolationSpeed);
		CameraComponent->SetFieldOfView(CurrentFOV);
	}
}

float UFirstPersonCameraComponent::CalculateFOVFromSpeed(float Speed) const
{
	if (Speed < Settings.MinSpeedForFOVChange)
	{
		return Settings.BaseFOV;
	}
	
	if (Speed > Settings.MaxSpeedForFOVChange)
	{
		return Settings.MaxFOV;
	}
	
	// Linear interpolation between min and max speed
	float SpeedAlpha = (Speed - Settings.MinSpeedForFOVChange) / (Settings.MaxSpeedForFOVChange - Settings.MinSpeedForFOVChange);
	return FMath::Lerp(Settings.BaseFOV, Settings.MaxFOV, SpeedAlpha);
}

void UFirstPersonCameraComponent::UpdateMotionBlur(float Speed)
{
	if (!PostProcessComponent)
	{
		return;
	}
	
	float BlurIntensity = 0.0f;
	
	if (Speed > Settings.SpeedThresholdForEffects)
	{
		// Calculate blur intensity based on speed
		float SpeedAlpha = (Speed - Settings.SpeedThresholdForEffects) / (Settings.MaxSpeedForFOVChange - Settings.SpeedThresholdForEffects);
		SpeedAlpha = FMath::Clamp(SpeedAlpha, 0.0f, 1.0f);
		BlurIntensity = SpeedAlpha * Settings.MaxMotionBlurAmount;
	}
	
	SetMotionBlurIntensity(BlurIntensity);
}

bool UFirstPersonCameraComponent::IsValidFOV(float FOV) const
{
	return FOV >= 30.0f && FOV <= 160.0f;
}