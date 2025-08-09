#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "SideViewCameraTypes.h"
#include "SideViewCameraComponent.generated.h"

/**
 * Component for managing side view camera functionality
 * Handles target following, look-ahead prediction, and boundary management
 */
UCLASS(BlueprintType, Blueprintable, Category = "Camera", 
	   meta = (BlueprintSpawnableComponent))
class BIRDDIVE_API USideViewCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Constructor
	USideViewCameraComponent();

	// Component lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Core camera management
	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void InitializeCamera();

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void Shutdown();

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	UCameraComponent* GetCameraComponent() const;

	// Following system
	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	float GetFollowDistance() const;

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void SetFollowDistance(float NewDistance);

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	float GetFollowHeight() const;

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void SetFollowHeight(float NewHeight);

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	AActor* GetTargetActor() const;

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void UpdateCamera(float DeltaTime);

	// Look-ahead system
	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	float GetLookAheadStrength() const;

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void SetLookAheadStrength(float Strength);

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	FVector CalculateLookAheadPosition() const;

	void UpdateLookAhead(float DeltaTime);

	// Boundary system
	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void SetBoundaryLimits(const FVector& MinBounds, const FVector& MaxBounds);

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	bool IsPositionInBounds(const FVector& Position) const;

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	FVector ClampToBoundary(const FVector& Position) const;

	// Settings access
	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	void ApplySettings(const FSideViewCameraSettings& NewSettings);

	UFUNCTION(BlueprintCallable, Category = "Side View Camera")
	const FSideViewCameraSettings& GetSettings() const;

	// Blueprint events
	UFUNCTION(BlueprintImplementableEvent, Category = "Side View Camera")
	void OnTargetOutOfBounds(const FVector& TargetLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Side View Camera")
	void OnCameraReachedBoundary(const FVector& BoundaryLocation);

protected:
	// Core components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FSideViewCameraSettings Settings;

	// Target and tracking
	UPROPERTY()
	AActor* TargetActor;

	FVector PreviousTargetPosition;
	FVector TargetVelocity;
	FVector CurrentCameraPosition;
	FVector TargetCameraPosition;

	// State
	bool bIsInitialized;
	bool bHasValidTarget;
	float LastUpdateTime;

private:
	// Internal helper methods
	void UpdateFollowPosition(float DeltaTime);
	void UpdateCameraTransform(float DeltaTime);
	FVector CalculateBaseFollowPosition() const;
	FVector CalculateTargetVelocity(float DeltaTime);
	bool IsValidTarget() const;
	void ResetCameraState();
};