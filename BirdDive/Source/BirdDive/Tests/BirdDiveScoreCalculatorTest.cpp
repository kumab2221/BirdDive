#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "../BirdDiveTypes.h"
#include "../Systems/Scoring/BirdDiveScoreCalculator.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveScoreCalculatorTest_BasicScore, 
    "BirdDive.ScoreCalculator.BasicScore", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdDiveScoreCalculatorTest_BasicScore::RunTest(const FString& Parameters)
{
    // Arrange - Perfect着地のテストデータ
    FLandingResult PerfectResult;
    PerfectResult.DistanceFromCenter = 5.0f; // Perfect範囲内（10単位以内）
    
    // Act - スコア計算クラスを作成してテスト実行
    UBirdDiveScoreCalculator* ScoreCalculator = NewObject<UBirdDiveScoreCalculator>();
    int32 Score = ScoreCalculator->CalculateBaseScore(PerfectResult);
    
    // Assert - Perfect着地で1000点、品質がPerfectになることを確認
    TestEqual("Perfect landing should return 1000 points", Score, 1000);
    TestEqual("Landing quality should be Perfect", (int32)PerfectResult.Quality, (int32)ELandingQuality::Perfect);
    TestEqual("BaseScore should be set in result", PerfectResult.BaseScore, 1000);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveScoreCalculatorTest_TimeBonus, 
    "BirdDive.ScoreCalculator.TimeBonus", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdDiveScoreCalculatorTest_TimeBonus::RunTest(const FString& Parameters)
{
    // Arrange - 時間ボーナステスト用データ
    float ElapsedTime = 30.0f;
    float TimeLimit = 60.0f;
    int32 ExpectedBonus = 300; // (60-30) × 10 = 300
    
    // Act - 時間ボーナス計算を実行
    UBirdDiveScoreCalculator* ScoreCalculator = NewObject<UBirdDiveScoreCalculator>();
    int32 TimeBonus = ScoreCalculator->CalculateTimeBonus(ElapsedTime, TimeLimit);
    
    // Assert - 時間ボーナスが正しく計算されることを確認
    TestEqual("Time bonus should be calculated correctly", TimeBonus, ExpectedBonus);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdDiveScoreCalculatorTest_FinalScore, 
    "BirdDive.ScoreCalculator.FinalScore", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdDiveScoreCalculatorTest_FinalScore::RunTest(const FString& Parameters)
{
    // Arrange - 最終スコア計算テスト用データ
    FLandingResult TestResult;
    TestResult.BaseScore = 1000;
    TestResult.BonusScore = 500;
    EDifficultyLevel TestDifficulty = EDifficultyLevel::Hard; // 1.5倍
    
    int32 ExpectedFinalScore = 2250; // (1000+500) × 1.5 = 2250
    
    // Act - 最終スコア計算を実行
    UBirdDiveScoreCalculator* ScoreCalculator = NewObject<UBirdDiveScoreCalculator>();
    int32 FinalScore = ScoreCalculator->CalculateFinalScore(TestResult, TestDifficulty);
    
    // Assert - 難易度倍率を含む最終スコアが正しく計算されることを確認
    TestEqual("Final score should include difficulty multiplier", FinalScore, ExpectedFinalScore);
    
    return true;
}