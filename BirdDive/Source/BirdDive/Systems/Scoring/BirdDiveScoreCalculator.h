#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../BirdDiveTypes.h"
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
    int32 CalculateBaseScore(UPARAM(ref) FLandingResult& LandingResult) const;
    
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
    
    // 難易度倍率取得
    float GetDifficultyMultiplier(EDifficultyLevel Difficulty) const;
};