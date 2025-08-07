#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../../BirdDiveTypes.h"
#include "BirdDiveGameMode.generated.h"

class UBirdDiveScoreCalculator;

// ゲームモード用状態変更デリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameModeStateChanged, EGameState, NewState);

/**
 * Base game mode for Bird Dive Challenge
 * 拡張: 難易度管理、ゲームフロー制御、スコア計算システム
 */
UCLASS()
class BIRDDIVE_API ABirdDiveGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	ABirdDiveGameMode();

	// GameModeBase overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 難易度管理
	UFUNCTION(BlueprintCallable, Category = "Difficulty")
	void SetDifficulty(EDifficultyLevel NewDifficulty);
	
	UFUNCTION(BlueprintCallable, Category = "Difficulty")
	FDifficultySettings GetCurrentDifficultySettings() const;
	
	UFUNCTION(BlueprintCallable, Category = "Difficulty")
	void ApplyDifficultySettings();

	// ゲームフロー制御
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartGame(EDifficultyLevel Difficulty);
	
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void EndGame(const FLandingResult& LandingResult);
	
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	EGameState GetCurrentGameState() const { return CurrentGameState; }

	// イベントデリゲート
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameModeStateChanged OnGameStateChanged;

protected:
	// 現在の難易度設定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Difficulty")
	EDifficultyLevel CurrentDifficulty;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Difficulty")
	FDifficultySettings DifficultySettings;

	// ゲーム状態
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	EGameState CurrentGameState;

	// スコア計算システム
	UPROPERTY()
	UBirdDiveScoreCalculator* ScoreCalculator;

private:
	// Simple flag to track BeginPlay state for tests
	bool bHasBegunPlay;

	// 内部メソッド
	void InitializeDifficultySettings();
	void SetCurrentGameState(EGameState NewState);
	FDifficultySettings CreateDifficultySettings(EDifficultyLevel Difficulty) const;

public:
	// Test helper function
	bool HasBegunPlay() const { return bHasBegunPlay; }
};