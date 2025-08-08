#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SplitScreenTypes.h"
#include "SplitScreenManager.generated.h"

/**
 * Component for managing split screen functionality
 * Handles viewport management, camera assignment, and split ratio control
 */
UCLASS(BlueprintType, Blueprintable, Category = "Camera", 
	   meta = (BlueprintSpawnableComponent))
class BIRDDIVE_API USplitScreenManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Constructor
	USplitScreenManager();

	// Component lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Core viewport management
	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	int32 GetViewportCount() const;

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	float GetSplitRatio() const;

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	bool SetSplitRatio(float NewRatio);

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	EBirdDiveSplitScreenType GetSplitType() const;

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	void SetSplitType(EBirdDiveSplitScreenType NewType);

	// Camera management
	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	bool AssignCamera(int32 ViewportIndex, ACameraActor* Camera);

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	ACameraActor* GetAssignedCamera(int32 ViewportIndex) const;

	// Viewport information access
	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	FBirdDiveViewportInfo GetViewportInfo(int32 Index) const;

	// System management
	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	void InitializeSplitScreen();

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	void Shutdown();

protected:
	// Configuration properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split Screen")
	EBirdDiveSplitScreenType SplitType = EBirdDiveSplitScreenType::Horizontal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split Screen", 
			  meta = (ClampMin = "0.2", ClampMax = "0.8"))
	float SplitRatio = 0.5f;

	// Viewport data storage
	UPROPERTY()
	TArray<FBirdDiveViewportInfo> Viewports;

	// Camera assignments
	UPROPERTY()
	TArray<ACameraActor*> AssignedCameras;

	// Internal state
	bool bIsInitialized = false;
	FIntPoint ScreenSize = FIntPoint(1920, 1080);
	
	// Render targets for each viewport
	UPROPERTY()
	TArray<UTextureRenderTarget2D*> RenderTargets;

private:
	// Internal helper methods
	void UpdateViewportRectangles();
	void CalculateViewportRect(int32 ViewportIndex, FIntRect& OutRect) const;
	float CalculateAspectRatio(const FIntRect& ViewportRect) const;
	bool IsValidViewportIndex(int32 Index) const;
	void CreateRenderTargets();
	void CleanupRenderTargets();
};