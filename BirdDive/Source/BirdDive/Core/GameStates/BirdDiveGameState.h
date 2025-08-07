#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../../BirdDiveTypes.h"
#include "../../BirdDiveDelegates.h"
#include "BirdDiveGameState.generated.h"

// Delegate for score changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);

/**
 * Game state for Bird Dive Challenge
 * 拡張: タイマー管理、最終スコア管理、制限時間処理
 */
UCLASS()
class BIRDDIVE_API ABirdDiveGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// Constructor
	ABirdDiveGameState();

	// GameStateBase overrides
	virtual void Tick(float DeltaTime) override;

	// Score management
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetCurrentScore() const { return CurrentScore; }
	
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetCurrentScore(int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetFinalScore() const { return FinalScore; }
	
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetFinalScore(int32 NewFinalScore);

	// Timer management
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StartGameTimer();
	
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StopGameTimer();
	
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void ResetGameTimer();
	
	UFUNCTION(BlueprintCallable, Category = "Timer")
	float GetElapsedTime() const { return ElapsedTime; }

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void SetTimeLimit(float NewTimeLimit);
	
	UFUNCTION(BlueprintCallable, Category = "Timer")
	float GetTimeLimit() const { return TimeLimit; }

	// Game state management
	UFUNCTION(BlueprintCallable, Category = "Game State")
	EGameState GetCurrentGameState() const { return CurrentGameState; }
	
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetCurrentGameState(EGameState NewState);

	// Player count management
	UFUNCTION(BlueprintCallable, Category = "Players")
	int32 GetPlayerCount() const { return PlayerCount; }
	
	UFUNCTION(BlueprintCallable, Category = "Players")
	void AddPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Players")
	void RemovePlayer();

	// Event delegates
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnScoreChanged OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameStateChanged OnGameStateChanged;

private:
	// Score data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
	int32 CurrentScore;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
	int32 FinalScore;

	// Timer data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	float ElapsedTime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	float TimeLimit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	bool bTimerActive;

	// Game state data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State", meta = (AllowPrivateAccess = "true"))
	EGameState CurrentGameState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Players", meta = (AllowPrivateAccess = "true"))
	int32 PlayerCount;

	// Internal methods
	void CheckTimeLimit();
};