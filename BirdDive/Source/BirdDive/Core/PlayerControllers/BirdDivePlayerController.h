#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BirdDivePlayerController.generated.h"

/**
 * Player controller for Bird Dive Challenge
 * Minimal implementation to pass tests
 */
UCLASS()
class BIRDDIVE_API ABirdDivePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Constructor
	ABirdDivePlayerController();

	// Input setup - minimal implementation
	virtual void SetupInputComponent() override;

	// Possession overrides (using new UE5 events)
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	// Lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};