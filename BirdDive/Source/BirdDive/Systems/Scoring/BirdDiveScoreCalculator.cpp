#include "BirdDiveScoreCalculator.h"
#include "Math/UnrealMathUtility.h"

int32 UBirdDiveScoreCalculator::CalculateBaseScore(FLandingResult& LandingResult) const
{
    // 距離から着地品質を判定
    ELandingQuality Quality = DetermineLandingQuality(LandingResult.DistanceFromCenter);
    LandingResult.Quality = Quality;
    
    // 品質に応じた基本スコア算出
    int32 BaseScore = MISS_SCORE;
    switch (Quality)
    {
        case ELandingQuality::Perfect:
            BaseScore = PERFECT_SCORE;
            break;
        case ELandingQuality::Good:
            BaseScore = GOOD_SCORE;
            break;
        case ELandingQuality::Poor:
            BaseScore = POOR_SCORE;
            break;
        case ELandingQuality::Miss:
        default:
            BaseScore = MISS_SCORE;
            break;
    }
    
    LandingResult.BaseScore = BaseScore;
    return BaseScore;
}

int32 UBirdDiveScoreCalculator::CalculateTimeBonus(float ElapsedTime, float TimeLimit) const
{
    if (ElapsedTime >= TimeLimit)
    {
        return 0;
    }
    
    float RemainingTime = TimeLimit - ElapsedTime;
    int32 TimeBonus = FMath::RoundToInt(RemainingTime * 10.0f);
    
    return FMath::Max(0, TimeBonus);
}

int32 UBirdDiveScoreCalculator::CalculateSpeedBonus(float LandingSpeed) const
{
    // 速度が低いほど高ボーナス
    int32 SpeedBonus = FMath::RoundToInt((1000.0f - LandingSpeed) * 2.0f);
    
    return FMath::Max(0, SpeedBonus);
}

int32 UBirdDiveScoreCalculator::CalculateFinalScore(const FLandingResult& LandingResult, EDifficultyLevel Difficulty) const
{
    int32 TotalScore = LandingResult.BaseScore + LandingResult.BonusScore;
    float Multiplier = GetDifficultyMultiplier(Difficulty);
    
    int32 FinalScore = FMath::RoundToInt(TotalScore * Multiplier);
    
    // オーバーフロー防止
    FinalScore = FMath::Clamp(FinalScore, 0, MAX_SCORE);
    
    return FinalScore;
}

ELandingQuality UBirdDiveScoreCalculator::DetermineLandingQuality(float DistanceFromCenter) const
{
    float Distance = FMath::Abs(DistanceFromCenter);
    
    if (Distance <= PERFECT_DISTANCE_THRESHOLD)
    {
        return ELandingQuality::Perfect;
    }
    else if (Distance <= GOOD_DISTANCE_THRESHOLD)
    {
        return ELandingQuality::Good;
    }
    else if (Distance <= POOR_DISTANCE_THRESHOLD)
    {
        return ELandingQuality::Poor;
    }
    else
    {
        return ELandingQuality::Miss;
    }
}

float UBirdDiveScoreCalculator::GetDifficultyMultiplier(EDifficultyLevel Difficulty) const
{
    switch (Difficulty)
    {
        case EDifficultyLevel::Easy:
            return 1.0f;
        case EDifficultyLevel::Normal:
            return 1.2f;
        case EDifficultyLevel::Hard:
            return 1.5f;
        default:
            return 1.0f;
    }
}