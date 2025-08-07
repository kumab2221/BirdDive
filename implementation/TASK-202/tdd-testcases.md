# TASK-202: ゲームモード・ステート実装 - TDDテストケース定義書

## テスト戦略

### テストレベル
1. **単体テスト**: 各クラス・関数の機能確認
2. **統合テスト**: システム間連携の確認
3. **エンドツーエンドテスト**: 完全なゲームフロー確認

### テスト環境
- **テストフレームワーク**: Unreal Engine Automation Tests
- **テスト実行方式**: C++単体テスト + Blueprint統合テスト

## 単体テストケース

### 1. スコア計算システムテスト

#### TestCase-001: 基本スコア計算精度
```cpp
// テストクラス: TestBirdDiveScoreCalculator_BasicScore
IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestBirdDiveScoreCalculator_BasicScore, 
    "BirdDive.ScoreCalculator.BasicScore", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

// テストケース
TEST_F(TestBirdDiveScoreCalculator_BasicScore, PerfectLanding_Returns1000Points) 
{
    // Arrange
    FLandingResult Result;
    Result.DistanceFromCenter = 5.0f; // 10単位以内
    
    // Act
    int32 Score = ScoreCalculator->CalculateBaseScore(Result);
    
    // Assert
    EXPECT_EQ(1000, Score);
    EXPECT_EQ(ELandingQuality::Perfect, Result.Quality);
}

TEST_F(TestBirdDiveScoreCalculator_BasicScore, GoodLanding_Returns500Points)
TEST_F(TestBirdDiveScoreCalculator_BasicScore, PoorLanding_Returns100Points)
TEST_F(TestBirdDiveScoreCalculator_BasicScore, MissLanding_Returns0Points)
```

#### TestCase-002: ボーナススコア計算精度
```cpp
TEST_F(TestBirdDiveScoreCalculator_BonusScore, TimeBonus_CalculatesCorrectly)
{
    // Arrange
    float ElapsedTime = 30.0f;
    float TimeLimit = 60.0f;
    
    // Act  
    int32 TimeBonus = ScoreCalculator->CalculateTimeBonus(ElapsedTime, TimeLimit);
    
    // Assert
    EXPECT_EQ(300, TimeBonus); // (60-30) × 10 = 300
}

TEST_F(TestBirdDiveScoreCalculator_BonusScore, SpeedBonus_CalculatesCorrectly)
TEST_F(TestBirdDiveScoreCalculator_BonusScore, SpeedBonus_MinimumZero)
```

#### TestCase-003: 最終スコア統合
```cpp
TEST_F(TestBirdDiveScoreCalculator_FinalScore, Integration_AllBonuses)
{
    // Arrange
    FLandingResult Result;
    Result.BaseScore = 1000;
    Result.BonusScore = 500;
    EDifficultyLevel Difficulty = EDifficultyLevel::Hard;
    
    // Act
    int32 FinalScore = ScoreCalculator->CalculateFinalScore(Result, Difficulty);
    
    // Assert
    EXPECT_EQ(2250, FinalScore); // (1000+500) × 1.5 = 2250
}

TEST_F(TestBirdDiveScoreCalculator_FinalScore, OverflowPrevention_MaxValue)
TEST_F(TestBirdDiveScoreCalculator_FinalScore, NegativeScorePrevention_MinValue)
```

### 2. 難易度システムテスト

#### TestCase-004: 難易度設定管理
```cpp
TEST_F(TestBirdDiveGameMode_Difficulty, EasyDifficulty_SetsCorrectParameters)
{
    // Arrange & Act
    GameMode->SetDifficulty(EDifficultyLevel::Easy);
    FDifficultySettings Settings = GameMode->GetCurrentDifficultySettings();
    
    // Assert
    EXPECT_FLOAT_EQ(0.5f, Settings.WindStrengthMultiplier);
    EXPECT_FLOAT_EQ(0.8f, Settings.GravityMultiplier);
    EXPECT_FLOAT_EQ(1.0f, Settings.ScoreMultiplier);
    EXPECT_FLOAT_EQ(90.0f, Settings.TimeLimit);
}

TEST_F(TestBirdDiveGameMode_Difficulty, NormalDifficulty_SetsCorrectParameters)
TEST_F(TestBirdDiveGameMode_Difficulty, HardDifficulty_SetsCorrectParameters)
```

#### TestCase-005: 難易度適用処理
```cpp
TEST_F(TestBirdDiveGameMode_DifficultyApplication, ApplyDifficulty_UpdatesWindSystem)
{
    // Arrange
    auto MockWindSystem = MakeShared<MockWindForceComponent>();
    GameMode->SetWindSystem(MockWindSystem);
    
    // Act
    GameMode->SetDifficulty(EDifficultyLevel::Hard);
    GameMode->ApplyDifficultySettings();
    
    // Assert
    EXPECT_CALL(MockWindSystem, SetStrengthMultiplier(1.5f));
    EXPECT_TRUE(MockWindSystem->VerifyCallsMade());
}
```

### 3. タイマー管理システムテスト

#### TestCase-006: タイマー基本動作
```cpp
TEST_F(TestBirdDiveGameState_Timer, StartTimer_BeginsCountingTime)
{
    // Arrange & Act
    GameState->StartGameTimer();
    float InitialTime = GameState->GetElapsedTime();
    
    // 少し待つ
    FPlatformProcess::Sleep(0.1f);
    float AfterTime = GameState->GetElapsedTime();
    
    // Assert
    EXPECT_GT(AfterTime, InitialTime);
    EXPECT_LT(AfterTime, 1.0f); // 1秒未満
}

TEST_F(TestBirdDiveGameState_Timer, StopTimer_StopsCountingTime)
TEST_F(TestBirdDiveGameState_Timer, ResetTimer_ResetsToZero)
```

#### TestCase-007: 制限時間管理
```cpp
TEST_F(TestBirdDiveGameState_Timer, TimeLimit_TriggersGameEnd)
{
    // Arrange
    GameState->SetTimeLimit(0.1f); // 0.1秒制限
    GameState->StartGameTimer();
    
    // Act - タイムアップまで待つ
    FPlatformProcess::Sleep(0.2f);
    
    // Assert
    EXPECT_EQ(EGameState::GameOver, GameState->GetCurrentGameState());
}
```

### 4. ゲーム状態遷移テスト

#### TestCase-008: 状態遷移制御
```cpp
TEST_F(TestBirdDiveGameMode_StateMachine, StartGame_TransitionsToPlaying)
{
    // Arrange
    GameMode->SetCurrentGameState(EGameState::Menu);
    
    // Act
    GameMode->StartGame(EDifficultyLevel::Normal);
    
    // Assert
    EXPECT_EQ(EGameState::Playing, GameMode->GetCurrentGameState());
}

TEST_F(TestBirdDiveGameMode_StateMachine, EndGame_TransitionsToGameOver)
TEST_F(TestBirdDiveGameMode_StateMachine, PauseGame_TransitionsToPaused)
TEST_F(TestBirdDiveGameMode_StateMachine, ResumeGame_TransitionsToPlaying)
```

#### TestCase-009: 状態変更通知
```cpp  
TEST_F(TestBirdDiveGameState_StateNotification, StateChange_TriggersDelegate)
{
    // Arrange
    bool bDelegateTriggered = false;
    EGameState ReceivedState = EGameState::Menu;
    
    GameState->OnGameStateChanged.AddLambda([&](EGameState NewState) {
        bDelegateTriggered = true;
        ReceivedState = NewState;
    });
    
    // Act
    GameState->SetCurrentGameState(EGameState::Playing);
    
    // Assert
    EXPECT_TRUE(bDelegateTriggered);
    EXPECT_EQ(EGameState::Playing, ReceivedState);
}
```

### 5. ゲームフロー統合テスト

#### TestCase-010: ゲーム開始処理
```cpp
TEST_F(TestBirdDiveGameMode_GameFlow, StartGame_InitializesAllSystems)
{
    // Arrange
    GameMode->SetCurrentGameState(EGameState::Menu);
    
    // Act
    GameMode->StartGame(EDifficultyLevel::Normal);
    
    // Assert
    // タイマーが開始されている
    EXPECT_GT(GameState->GetElapsedTime(), 0.0f);
    
    // スコアがリセットされている
    EXPECT_EQ(0, GameState->GetCurrentScore());
    
    // 状態が適切に設定されている
    EXPECT_EQ(EGameState::Playing, GameState->GetCurrentGameState());
    
    // 難易度設定が適用されている
    FDifficultySettings Settings = GameMode->GetCurrentDifficultySettings();
    EXPECT_FLOAT_EQ(1.0f, Settings.WindStrengthMultiplier);
}
```

#### TestCase-011: ゲーム終了処理
```cpp
TEST_F(TestBirdDiveGameMode_GameFlow, EndGame_FinalizesAllSystems)
{
    // Arrange - ゲーム中状態にする
    GameMode->StartGame(EDifficultyLevel::Normal);
    GameState->SetCurrentScore(500);
    
    // Act - 着地結果でゲーム終了
    FLandingResult LandingResult;
    LandingResult.Quality = ELandingQuality::Good;
    LandingResult.DistanceFromCenter = 30.0f;
    LandingResult.LandingSpeed = 400.0f;
    
    GameMode->EndGame(LandingResult);
    
    // Assert
    // タイマーが停止している
    float ElapsedTime1 = GameState->GetElapsedTime();
    FPlatformProcess::Sleep(0.1f);
    float ElapsedTime2 = GameState->GetElapsedTime();
    EXPECT_FLOAT_EQ(ElapsedTime1, ElapsedTime2);
    
    // 最終スコアが計算されている
    EXPECT_GT(GameState->GetFinalScore(), 0);
    
    // 状態が適切に変更されている
    EXPECT_EQ(EGameState::GameOver, GameState->GetCurrentGameState());
}
```

## エラーケーステスト

### TestCase-012: エッジケース処理
```cpp
TEST_F(TestBirdDiveScoreCalculator_EdgeCases, ZeroDistance_ReturnsPerfectScore)
TEST_F(TestBirdDiveScoreCalculator_EdgeCases, NegativeDistance_TreatedAsAbsolute)
TEST_F(TestBirdDiveScoreCalculator_EdgeCases, ExtremelyHighSpeed_DoesNotCrash)
TEST_F(TestBirdDiveGameState_EdgeCases, NegativeTime_DoesNotCrash)
TEST_F(TestBirdDiveGameMode_EdgeCases, InvalidDifficulty_UsesDefault)
```

## パフォーマンステスト

### TestCase-013: 処理時間テスト
```cpp
TEST_F(TestBirdDiveScoreCalculator_Performance, ScoreCalculation_CompletesUnder1ms)
{
    // Arrange
    FLandingResult Result;
    Result.DistanceFromCenter = 25.0f;
    Result.LandingSpeed = 500.0f;
    
    // Act & Assert
    auto StartTime = FPlatformTime::Seconds();
    
    for (int32 i = 0; i < 1000; ++i)
    {
        ScoreCalculator->CalculateFinalScore(Result, EDifficultyLevel::Normal);
    }
    
    auto EndTime = FPlatformTime::Seconds();
    double AverageTime = (EndTime - StartTime) / 1000.0;
    
    EXPECT_LT(AverageTime, 0.001); // 1ms未満
}
```

## テスト実行計画

### フェーズ1: 基本機能テスト (30分)
1. スコア計算システムの単体テスト実行
2. 難易度システムの単体テスト実行
3. タイマーシステムの単体テスト実行

### フェーズ2: 統合テスト (20分)
1. ゲーム状態遷移テスト実行
2. システム間連携テスト実行

### フェーズ3: エンドツーエンドテスト (20分)
1. 完全なゲームフローテスト実行
2. エラーケース・パフォーマンステスト実行

### フェーズ4: 回帰テスト (10分)
1. 既存機能への影響確認
2. 全テストスイート実行

## テストデータ

### テスト用難易度設定
```cpp
// テスト用のモック難易度設定
const FDifficultySettings TestEasySettings = {0.5f, 0.8f, 1.0f, 90.0f};
const FDifficultySettings TestNormalSettings = {1.0f, 1.0f, 1.2f, 60.0f};
const FDifficultySettings TestHardSettings = {1.5f, 1.2f, 1.5f, 45.0f};
```

### テスト用着地結果
```cpp
// Perfect着地
const FLandingResult PerfectLanding = {ELandingQuality::Perfect, 5.0f, 300.0f, 1000, 500};

// Good着地  
const FLandingResult GoodLanding = {ELandingQuality::Good, 30.0f, 600.0f, 500, 200};

// Poor着地
const FLandingResult PoorLanding = {ELandingQuality::Poor, 80.0f, 900.0f, 100, 50};

// Miss着地
const FLandingResult MissLanding = {ELandingQuality::Miss, 150.0f, 1200.0f, 0, 0};
```

## 成功基準

### 機能テスト
- [ ] 全単体テストが成功 (100%)
- [ ] 全統合テストが成功 (100%)  
- [ ] エッジケース処理が適切に動作

### パフォーマンステスト  
- [ ] スコア計算処理時間 < 1ms
- [ ] 状態遷移処理時間 < 0.1ms
- [ ] メモリリークなし

### 品質テスト
- [ ] コードカバレッジ > 90%
- [ ] 静的解析エラー 0件
- [ ] 実行時エラー 0件

## テスト環境要件
- Unreal Engine 5.6
- Visual Studio 2022  
- テストランナー: UE Automation Tests
- コードカバレッジツール: 内蔵カバレッジ機能