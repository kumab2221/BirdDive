# TASK-102: データ構造・列挙型実装 - 最小実装 (GREEN フェーズ)

## 概要

TDD の GREEN フェーズとして、RED フェーズで作成した失敗するテストを通すための最小限の実装を行います。過度な実装は避け、テストが通る必要最小限のコードのみを作成します。

## 実装ファイル

### 1. BirdDiveTypes.h

列挙型と構造体の定義：

```cpp
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
struct BIRDDIVE_API FWindData
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
    FWindData()
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
```

### 2. BirdDiveDataAssets.h

DataAsset クラスの定義：

```cpp
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
```

### 3. BirdDiveDataAssets.cpp

DataAsset クラスの実装：

```cpp
#include "BirdDiveDataAssets.h"
#include "BirdDiveTypes.h"

// UDifficultyDataAsset は主にデータ保持のみなので、追加実装は不要

// UGameConfigDataAsset は主にデータ保持のみなので、追加実装は不要
```

### 4. BirdDiveDelegates.h

デリゲート定義：

```cpp
#pragma once

#include "CoreMinimal.h"
#include "BirdDiveTypes.h"
#include "BirdDiveDelegates.generated.h"

// ゲーム状態変更時のデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameState, NewGameState);

// スコア更新時のデリゲート  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, int32, NewScore);

// 速度変更時のデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedChanged, float, NewSpeed);

// 着地時のデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLandingCompleted, const FLandingResult&, Result);

// 警告表示用デリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWarningTriggered, const FText&, WarningMessage);
```

## 実装手順

### Step 1: ファイル作成
ヘッダーファイルと実装ファイルを作成します。

### Step 2: コンパイル確認
最小限の実装でコンパイルが通ることを確認します。

### Step 3: テスト実行
作成したテストが全て通ることを確認します。

## 実装のポイント

### 最小限の原則
- テストが通る最小限の実装のみ
- 余分な機能や最適化は避ける
- デフォルト値は要件通りに設定

### Blueprint 統合
- 全ての型に適切な UCLASS/USTRUCT/UENUM マクロ
- BlueprintType メタデータの設定
- 適切な UPROPERTY マクロの使用

### コンストラクタ
- 全構造体にデフォルトコンストラクタ
- 要件通りのデフォルト値設定
- 明示的な初期化

## 期待される結果

### コンパイル成功
- 全ファイルがエラーなしでコンパイル
- リンクエラーなし
- 適切な Blueprint 統合

### テスト成功
- 全列挙型テストが成功
- 全構造体テストが成功  
- 全 DataAsset テストが成功
- Blueprint 統合テストが成功

## 成功条件

- [ ] BirdDiveTypes.h が作成され、コンパイル成功
- [ ] BirdDiveDataAssets.h/.cpp が作成され、コンパイル成功
- [ ] BirdDiveDelegates.h が作成され、コンパイル成功
- [ ] 全自動テストが成功（21個）
- [ ] Blueprint からアクセス可能
- [ ] 過度な実装がない（最小限）

---

**作成日**: 2025-08-05  
**作成者**: Claude Code (TASK-102 TDD実装)  
**フェーズ**: GREEN (最小実装)  
**ステータス**: 実装準備完了