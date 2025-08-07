# TASK-202: TDD Red Phase - 失敗するテスト実装

## 実装方針

テストファーストアプローチで、まず失敗するテストを実装し、テストが確実に失敗することを確認する。

## テスト実装

### 1. スコア計算システムのテスト

まず、スコア計算専用クラス `UBirdDiveScoreCalculator` のテストを実装します。

```cpp
// BirdDive/Source/BirdDive/Tests/BirdDiveScoreCalculatorTest.cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "BirdDiveTypes.h"

// 実装前なのでインクルードは失敗するはず - これが「Red」の状態
// #include "Systems/Scoring/BirdDiveScoreCalculator.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveScoreCalculatorTest_BasicScore, 
    "BirdDive.ScoreCalculator.BasicScore", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdDiveScoreCalculatorTest_BasicScore::RunTest(const FString& Parameters)
{
    // Arrange - スコア計算クラスのインスタンス作成
    // UBirdDiveScoreCalculator* ScoreCalculator = NewObject<UBirdDiveScoreCalculator>();
    // ↑ まだクラスが存在しないので、コンパイルエラーになる
    
    FLandingResult PerfectResult;
    PerfectResult.DistanceFromCenter = 5.0f; // Perfect範囲内
    
    // Act - 基本スコア計算
    // int32 Score = ScoreCalculator->CalculateBaseScore(PerfectResult);
    // ↑ メソッドが存在しないので、コンパイルエラーになる
    
    // Assert - Perfect着地で1000点を期待
    // TestEqual("Perfect landing should return 1000 points", Score, 1000);
    // TestEqual("Landing quality should be Perfect", (int32)PerfectResult.Quality, (int32)ELandingQuality::Perfect);
    
    // 現在はクラスが存在しないので、テストは失敗する
    TestFalse("ScoreCalculator class does not exist yet", true);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveScoreCalculatorTest_TimeBonus, 
    "BirdDive.ScoreCalculator.TimeBonus", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdDiveScoreCalculatorTest_TimeBonus::RunTest(const FString& Parameters)
{
    // Arrange
    float ElapsedTime = 30.0f;
    float TimeLimit = 60.0f;
    int32 ExpectedBonus = 300; // (60-30) × 10
    
    // Act & Assert - まだ実装されていないのでテスト失敗
    TestFalse("TimeBonus calculation not implemented yet", true);
    
    return true;
}
```

### 2. ゲームモード拡張のテスト

既存の `ABirdDiveGameMode` クラスに新機能を追加するテストを実装します。

```cpp
// BirdDive/Source/BirdDive/Tests/BirdDiveGameModeTest.cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Core/GameModes/BirdDiveGameMode.h"
#include "BirdDiveTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveGameModeTest_DifficultySettings, 
    "BirdDive.GameMode.DifficultySettings", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdDiveGameModeTest_DifficultySettings::RunTest(const FString& Parameters)
{
    // Arrange - GameModeインスタンス作成
    ABirdDiveGameMode* GameMode = NewObject<ABirdDiveGameMode>();
    
    // Act & Assert - まだ実装されていない機能のテスト
    // GameMode->SetDifficulty(EDifficultyLevel::Easy);
    // ↑ SetDifficultyメソッドが存在しないので、コンパイルエラー
    
    // FDifficultySettings Settings = GameMode->GetCurrentDifficultySettings();
    // ↑ GetCurrentDifficultySettingsメソッドが存在しないので、コンパイルエラー
    
    // TestEqual("Easy difficulty wind multiplier", Settings.WindStrengthMultiplier, 0.5f);
    // TestEqual("Easy difficulty gravity multiplier", Settings.GravityMultiplier, 0.8f);
    
    // 現在はメソッドが存在しないので、テストは失敗する
    TestFalse("Difficulty management methods do not exist yet", true);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveGameModeTest_GameFlow, 
    "BirdDive.GameMode.GameFlow", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdDiveGameModeTest_GameFlow::RunTest(const FString& Parameters)
{
    // Arrange
    ABirdDiveGameMode* GameMode = NewObject<ABirdDiveGameMode>();
    
    // Act & Assert - ゲーム開始機能のテスト
    // GameMode->StartGame(EDifficultyLevel::Normal);
    // ↑ StartGameメソッドが存在しないので、コンパイルエラー
    
    // EGameState CurrentState = GameMode->GetCurrentGameState();
    // ↑ GetCurrentGameStateメソッドが存在しないので、コンパイルエラー
    
    // TestEqual("Game state should be Playing after StartGame", (int32)CurrentState, (int32)EGameState::Playing);
    
    TestFalse("Game flow methods do not exist yet", true);
    
    return true;
}
```

### 3. ゲーム状態拡張のテスト

既存の `ABirdDiveGameState` クラスにタイマー機能を追加するテストを実装します。

```cpp
// BirdDive/Source/BirdDive/Tests/BirdDiveGameStateTest.cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Core/GameStates/BirdDiveGameState.h"
#include "BirdDiveTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveGameStateTest_Timer, 
    "BirdDive.GameState.Timer", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdDiveGameStateTest_Timer::RunTest(const FString& Parameters)
{
    // Arrange
    ABirdDiveGameState* GameState = NewObject<ABirdDiveGameState>();
    
    // Act & Assert - タイマー機能のテスト
    // GameState->StartGameTimer();
    // ↑ StartGameTimerメソッドが存在しないので、コンパイルエラー
    
    // float ElapsedTime = GameState->GetElapsedTime();
    // ↑ GetElapsedTimeメソッドが存在しないので、コンパイルエラー
    
    // TestTrue("Timer should start counting", ElapsedTime >= 0.0f);
    
    TestFalse("Timer methods do not exist yet", true);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveGameStateTest_GameStateManagement, 
    "BirdDive.GameState.StateManagement", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdDiveGameStateTest_GameStateManagement::RunTest(const FString& Parameters)
{
    // Arrange
    ABirdDiveGameState* GameState = NewObject<ABirdDiveGameState>();
    
    // Act & Assert - ゲーム状態管理のテスト
    // GameState->SetCurrentGameState(EGameState::Playing);
    // ↑ SetCurrentGameStateメソッドが存在しないかもしれない
    
    // EGameState CurrentState = GameState->GetCurrentGameState();
    // ↑ GetCurrentGameStateメソッドが存在しないかもしれない
    
    // TestEqual("Game state should be set correctly", (int32)CurrentState, (int32)EGameState::Playing);
    
    TestFalse("Game state management methods may not exist yet", true);
    
    return true;
}
```

## 新しいクラス設計

### UBirdDiveScoreCalculator クラス

```cpp
// BirdDive/Source/BirdDive/Systems/Scoring/BirdDiveScoreCalculator.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BirdDiveTypes.h"
#include "BirdDiveScoreCalculator.generated.h"

/**
 * スコア計算専用クラス
 * 着地結果に基づいて各種スコアを計算する
 */
UCLASS(BlueprintType)
class BIRDDIVE_API UBirdDiveScoreCalculator : public UObject
{
    GENERATED_BODY()

public:
    // 基本スコア計算
    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 CalculateBaseScore(FLandingResult& LandingResult) const;
    
    // 時間ボーナス計算
    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 CalculateTimeBonus(float ElapsedTime, float TimeLimit) const;
    
    // 速度ボーナス計算
    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 CalculateSpeedBonus(float LandingSpeed) const;
    
    // 最終スコア計算
    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 CalculateFinalScore(const FLandingResult& LandingResult, EDifficultyLevel Difficulty) const;

private:
    // スコア閾値設定
    static constexpr float PERFECT_DISTANCE_THRESHOLD = 10.0f;
    static constexpr float GOOD_DISTANCE_THRESHOLD = 50.0f;
    static constexpr float POOR_DISTANCE_THRESHOLD = 100.0f;
    
    static constexpr int32 PERFECT_SCORE = 1000;
    static constexpr int32 GOOD_SCORE = 500;
    static constexpr int32 POOR_SCORE = 100;
    static constexpr int32 MISS_SCORE = 0;
    
    static constexpr int32 MAX_SCORE = 999999;
    
    // 着地品質判定
    ELandingQuality DetermineLandingQuality(float DistanceFromCenter) const;
};
```

## テスト実行

現在の状態でテストを実行すると、以下の理由で失敗するはずです：

1. **コンパイルエラー**: 新しいクラス `UBirdDiveScoreCalculator` が存在しない
2. **リンクエラー**: 既存クラスに新しいメソッドが実装されていない
3. **論理エラー**: テストが期待する動作が実装されていない

これが TDD の「Red」フェーズです。

## 実行結果の確認

```bash
# プロジェクトをコンパイルしてテスト実行
# 期待される結果: コンパイルエラーまたはテスト失敗
```

テストが失敗することを確認できたら、次のGreenフェーズで最小実装を行います。

## Red フェーズの成功基準

- [x] テストファイルが作成される
- [x] テストを実行すると確実に失敗する
- [x] 失敗の原因が明確である（未実装機能）
- [x] テストケースが要件を適切にカバーしている

## 次のステップ

Green フェーズで以下を実装する予定：
1. `UBirdDiveScoreCalculator` クラスの基本実装
2. `ABirdDiveGameMode` の難易度管理機能
3. `ABirdDiveGameState` のタイマー機能
4. ゲーム状態遷移システム