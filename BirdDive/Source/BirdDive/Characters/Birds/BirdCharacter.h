#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BirdCharacter.generated.h"

/**
 * Base character class for birds in Bird Dive Challenge
 * Minimal implementation to pass tests
 */
UCLASS()
class BIRDDIVE_API ABirdCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Constructor
	ABirdCharacter();

protected:
	// Lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Tick function
	virtual void Tick(float DeltaTime) override;

	// Input binding
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Simple flag to track BeginPlay state for tests
	bool bHasBegunPlay;

public:
	// Test helper function
	bool HasBegunPlay() const { return bHasBegunPlay; }
};