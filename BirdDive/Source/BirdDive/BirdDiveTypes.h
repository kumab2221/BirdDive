#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BirdDiveTypes.generated.h"

/**
 * 難易度レベル
 */
UENUM(BlueprintType)
enum class EDifficultyLevel : uint8
{
    Easy    UMETA(DisplayName = "Easy"),      // 初心者向け
    Normal  UMETA(DisplayName = "Normal"),    // 標準
    Hard    UMETA(DisplayName = "Hard")       // 上級者向け
};

/**
 * ゲーム状態
 */
UENUM(BlueprintType)
enum class EGameState : uint8
{
    Menu        UMETA(DisplayName = "Menu"),        // メニュー画面
    Playing     UMETA(DisplayName = "Playing"),     // ゲームプレイ中
    Paused      UMETA(DisplayName = "Paused"),      // 一時停止
    GameOver    UMETA(DisplayName = "GameOver"),    // ゲーム終了
    Results     UMETA(DisplayName = "Results")      // 結果表示
};

/**
 * 飛行状態
 */
UENUM(BlueprintType)
enum class EFlightState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),        // 待機状態
    Flying      UMETA(DisplayName = "Flying"),      // 飛行中
    Gliding     UMETA(DisplayName = "Gliding"),     // 滑空中
    Climbing    UMETA(DisplayName = "Climbing"),    // 上昇中
    Diving      UMETA(DisplayName = "Diving"),      // 降下中
    Turning     UMETA(DisplayName = "Turning"),     // 旋回中
    Landing     UMETA(DisplayName = "Landing"),     // 着地中
    Landed      UMETA(DisplayName = "Landed")       // 着地完了
};

/**
 * 着地品質
 */
UENUM(BlueprintType)
enum class ELandingQuality : uint8
{
    Miss        UMETA(DisplayName = "Miss"),        // 的外れ
    Poor        UMETA(DisplayName = "Poor"),        // 不正確
    Good        UMETA(DisplayName = "Good"),        // 良好
    Perfect     UMETA(DisplayName = "Perfect")      // 完璧
};

/**
 * 難易度設定
 */
USTRUCT(BlueprintType)
struct BIRDDIVE_API FDifficultySettings
{
    GENERATED_BODY()

    // 風力強度係数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    float WindStrengthMultiplier = 1.0f;

    // 重力係数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    float GravityMultiplier = 1.0f;

    // スコア倍率
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    float ScoreMultiplier = 1.0f;

    // 制限時間（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    float TimeLimit = 60.0f;

    // デフォルトコンストラクタ
    FDifficultySettings()
    {
        WindStrengthMultiplier = 1.0f;
        GravityMultiplier = 1.0f;
        ScoreMultiplier = 1.0f;
        TimeLimit = 60.0f;
    }
};

/**
 * 風データ
 */
USTRUCT(BlueprintType)
struct BIRDDIVE_API FBirdDiveWindData
{
    GENERATED_BODY()

    // 風向ベクトル（正規化済み）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    FVector Direction = FVector::ZeroVector;

    // 風力強度（0.0-1.0）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    float Strength = 0.0f;

    // 乱流強度（0.0-1.0）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    float Turbulence = 0.0f;

    // デフォルトコンストラクタ
    FBirdDiveWindData()
    {
        Direction = FVector::ZeroVector;
        Strength = 0.0f;
        Turbulence = 0.0f;
    }
};

/**
 * 着地結果
 */
USTRUCT(BlueprintType)
struct BIRDDIVE_API FLandingResult
{
    GENERATED_BODY()

    // 着地品質
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing")
    ELandingQuality Quality = ELandingQuality::Miss;

    // 的の中心からの距離
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing")
    float DistanceFromCenter = 0.0f;

    // 着地時の速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing")
    float LandingSpeed = 0.0f;

    // 基本スコア
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    int32 BaseScore = 0;

    // ボーナススコア
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    int32 BonusScore = 0;

    // デフォルトコンストラクタ
    FLandingResult()
    {
        Quality = ELandingQuality::Miss;
        DistanceFromCenter = 0.0f;
        LandingSpeed = 0.0f;
        BaseScore = 0;
        BonusScore = 0;
    }
};

/**
 * ゲームセッションデータ
 */
USTRUCT(BlueprintType)
struct BIRDDIVE_API FGameSessionData
{
    GENERATED_BODY()

    // セッション開始時刻
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FDateTime StartTime;

    // 選択された難易度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    EDifficultyLevel Difficulty = EDifficultyLevel::Normal;

    // 最終スコア
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    int32 FinalScore = 0;

    // プレイ時間（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    float PlayTime = 0.0f;

    // 着地結果
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FLandingResult LandingResult;

    // デフォルトコンストラクタ
    FGameSessionData()
    {
        StartTime = FDateTime::Now();
        Difficulty = EDifficultyLevel::Normal;
        FinalScore = 0;
        PlayTime = 0.0f;
        LandingResult = FLandingResult();
    }
};