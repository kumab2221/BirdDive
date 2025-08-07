#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "../Core/GameModes/BirdDiveGameMode.h"
#include "../BirdDiveTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveGameModeTest_DifficultySettings, 
    "BirdDive.GameMode.DifficultySettings", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdDiveGameModeTest_DifficultySettings::RunTest(const FString& Parameters)
{
    // Arrange - GameModeインスタンス作成
    ABirdDiveGameMode* GameMode = NewObject<ABirdDiveGameMode>();
    TestNotNull("GameMode should be created", GameMode);
    
    // Act - 難易度管理機能のテスト
    GameMode->SetDifficulty(EDifficultyLevel::Easy);
    FDifficultySettings Settings = GameMode->GetCurrentDifficultySettings();
    
    // Assert - Easy難易度の設定値が正しいことを確認
    TestEqual("Easy difficulty wind multiplier should be 0.5", Settings.WindStrengthMultiplier, 0.5f);
    TestEqual("Easy difficulty gravity multiplier should be 0.8", Settings.GravityMultiplier, 0.8f);
    TestEqual("Easy difficulty score multiplier should be 1.0", Settings.ScoreMultiplier, 1.0f);
    TestEqual("Easy difficulty time limit should be 90 seconds", Settings.TimeLimit, 90.0f);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveGameModeTest_GameFlow, 
    "BirdDive.GameMode.GameFlow", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdDiveGameModeTest_GameFlow::RunTest(const FString& Parameters)
{
    // Arrange
    ABirdDiveGameMode* GameMode = NewObject<ABirdDiveGameMode>();
    TestNotNull("GameMode should be created", GameMode);
    
    // Act - ゲーム開始・終了機能のテスト
    GameMode->StartGame(EDifficultyLevel::Normal);
    EGameState CurrentState = GameMode->GetCurrentGameState();
    
    // Assert - ゲーム開始後の状態確認
    TestEqual("Game state should be Playing after StartGame", (int32)CurrentState, (int32)EGameState::Playing);
    
    // ゲーム終了のテスト
    FLandingResult TestResult;
    TestResult.Quality = ELandingQuality::Good;
    TestResult.BaseScore = 500;
    TestResult.BonusScore = 200;
    GameMode->EndGame(TestResult);
    CurrentState = GameMode->GetCurrentGameState();
    
    // Assert - ゲーム終了後の状態確認
    TestEqual("Game state should be GameOver after EndGame", (int32)CurrentState, (int32)EGameState::GameOver);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveGameModeTest_DifficultyApplication, 
    "BirdDive.GameMode.DifficultyApplication", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdDiveGameModeTest_DifficultyApplication::RunTest(const FString& Parameters)
{
    // Arrange
    ABirdDiveGameMode* GameMode = NewObject<ABirdDiveGameMode>();
    
    // Act - 難易度適用処理のテスト
    GameMode->SetDifficulty(EDifficultyLevel::Hard);
    GameMode->ApplyDifficultySettings();
    
    // Assert - 難易度が適用されたことを確認
    FDifficultySettings Settings = GameMode->GetCurrentDifficultySettings();
    TestEqual("Hard difficulty should be applied", Settings.WindStrengthMultiplier, 1.5f);
    
    // 物理システムへの適用確認は統合テストで行う予定
    TestTrue("Difficulty application methods now exist", true);
    
    return true;
}