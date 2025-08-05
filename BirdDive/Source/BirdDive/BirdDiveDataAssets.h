#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BirdDiveTypes.h"
#include "BirdDiveDataAssets.generated.h"

/**
 * 難易度データアセット
 */
UCLASS(BlueprintType)
class BIRDDIVE_API UDifficultyDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    // 難易度レベル
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty")
    EDifficultyLevel DifficultyLevel = EDifficultyLevel::Normal;

    // 難易度設定
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty")
    FDifficultySettings Settings;

    // 表示名
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    FText DisplayName;

    // 説明文
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    FText Description;

    // コンストラクタ
    UDifficultyDataAsset()
    {
        DifficultyLevel = EDifficultyLevel::Normal;
        Settings = FDifficultySettings();
        DisplayName = FText::FromString(TEXT("Normal"));
        Description = FText::FromString(TEXT("Standard difficulty"));
    }
};

/**
 * ゲーム設定データアセット
 */
UCLASS(BlueprintType)
class BIRDDIVE_API UGameConfigDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    // デフォルト難易度設定マップ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    TMap<EDifficultyLevel, FDifficultySettings> DefaultDifficulties;

    // 物理パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
    float DefaultGravity = -980.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
    float MaxFlightSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
    float AirResistance = 0.1f;

    // スコアリング設定
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring")
    TMap<ELandingQuality, int32> BaseScores;

    // コンストラクタ
    UGameConfigDataAsset()
    {
        DefaultGravity = -980.0f;
        MaxFlightSpeed = 1000.0f;
        AirResistance = 0.1f;
        
        // デフォルト設定の初期化
        DefaultDifficulties.Empty();
        BaseScores.Empty();
    }
};