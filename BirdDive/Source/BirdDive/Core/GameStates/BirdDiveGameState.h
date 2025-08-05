#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BirdDiveGameState.generated.h"

// Delegate for score changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);

/**
 * Game state for Bird Dive Challenge
 * Minimal implementation to pass tests
 */
UCLASS()
class BIRDDIVE_API ABirdDiveGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// Constructor
	ABirdDiveGameState();

	// Score management
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetCurrentScore() const { return CurrentScore; }
	
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetCurrentScore(int32 NewScore);

	// Player count management
	UFUNCTION(BlueprintCallable, Category = "Players")
	int32 GetPlayerCount() const { return PlayerCount; }
	
	UFUNCTION(BlueprintCallable, Category = "Players")
	void AddPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Players")
	void RemovePlayer();

	// Event delegate
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnScoreChanged OnScoreChanged;

private:
	// Basic game state data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
	int32 CurrentScore;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Players", meta = (AllowPrivateAccess = "true"))
	int32 PlayerCount;
};