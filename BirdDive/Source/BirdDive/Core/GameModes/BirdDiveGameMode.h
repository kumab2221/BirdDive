#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BirdDiveGameMode.generated.h"

/**
 * Base game mode for Bird Dive Challenge
 * Minimal implementation to pass tests
 */
UCLASS()
class BIRDDIVE_API ABirdDiveGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	ABirdDiveGameMode();

	// Minimal required overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// Simple flag to track BeginPlay state for tests
	bool bHasBegunPlay;

public:
	// Test helper function
	bool HasBegunPlay() const { return bHasBegunPlay; }
};