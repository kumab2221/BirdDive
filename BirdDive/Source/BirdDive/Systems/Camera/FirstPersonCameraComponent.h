#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FirstPersonCameraTypes.h"
#include "FirstPersonCameraComponent.generated.h"

/**
 * Component for managing first person camera functionality
 * Handles head tracking, speed-based FOV adjustment, and post-process effects
 */
UCLASS(BlueprintType, Blueprintable, Category = "Camera", 
	   meta = (BlueprintSpawnableComponent))
class BIRDDIVE_API UFirstPersonCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Constructor
	UFirstPersonCameraComponent();

	// Component lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Core camera management
	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void InitializeCamera();

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void Shutdown();

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	UCameraComponent* GetCameraComponent() const;

	// FOV management
	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	float GetBaseFOV() const;

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void SetBaseFOV(float NewFOV);

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	float GetMinFOV() const;

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	float GetMaxFOV() const;

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void SetFOVRange(float NewMinFOV, float NewMaxFOV);

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	float GetCurrentFOV() const;

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void UpdateFOVBasedOnSpeed(float CurrentSpeed);

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void EnableSpeedBasedFOV(bool bEnable);

	// Attachment functionality
	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	bool AttachToSocketOrBone(USkeletalMeshComponent* Mesh, const FName& SocketName);

	void DetachFromAttachedMesh();

	// Post-process effects
	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void SetPostProcessEffects(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	bool ArePostProcessEffectsEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void UpdatePostProcessEffects(float Speed, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void SetMotionBlurIntensity(float Intensity);

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	float GetMotionBlurIntensity() const;

	// Settings access
	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	void ApplySettings(const FFirstPersonCameraSettings& NewSettings);

	UFUNCTION(BlueprintCallable, Category = "First Person Camera")
	const FFirstPersonCameraSettings& GetSettings() const;

protected:
	// Core components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	UPostProcessComponent* PostProcessComponent;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FFirstPersonCameraSettings Settings;

	// Current state
	float CurrentFOV;
	float TargetFOV;
	bool bSpeedBasedFOVEnabled;
	bool bPostProcessEffectsEnabled;
	bool bIsInitialized;

	// Attachment info
	UPROPERTY()
	USkeletalMeshComponent* AttachedMesh;

	FName AttachSocketName;
	bool bIsAttached;

private:
	// Internal helper methods
	void UpdateCameraFOV(float DeltaTime);
	float CalculateFOVFromSpeed(float Speed) const;
	void UpdateMotionBlur(float Speed);
	bool IsValidFOV(float FOV) const;
};