# TASK-102: データ構造・列挙型実装 - 要件定義

## 概要

Bird Dive Challengeで使用するデータ構造、列挙型、DataAsset、デリゲートを実装する。
これらは設計書で定義された仕様に基づき、Blueprint から利用可能な形で提供される。

## 対象要件

- **REQ-101**: 基本列挙型の実装
- **REQ-102**: データ構造体の実装  
- **REQ-103**: DataAsset クラスの実装

## 要件詳細

### REQ-101: 基本列挙型の実装

#### 1.1 EDifficultyLevel
```cpp
UENUM(BlueprintType)
enum class EDifficultyLevel : uint8
{
    Easy    UMETA(DisplayName = "Easy"),      // 初心者向け
    Normal  UMETA(DisplayName = "Normal"),    // 標準
    Hard    UMETA(DisplayName = "Hard")       // 上級者向け
};
```

#### 1.2 EGameState  
```cpp
UENUM(BlueprintType)
enum class EGameState : uint8
{
    Menu        UMETA(DisplayName = "Menu"),        // メニュー画面
    Playing     UMETA(DisplayName = "Playing"),     // ゲームプレイ中
    Paused      UMETA(DisplayName = "Paused"),      // 一時停止
    GameOver    UMETA(DisplayName = "GameOver"),    // ゲーム終了
    Results     UMETA(DisplayName = "Results")      // 結果表示
};
```

#### 1.3 EFlightState
```cpp
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
```

#### 1.4 ELandingQuality
```cpp
UENUM(BlueprintType)
enum class ELandingQuality : uint8
{
    Miss        UMETA(DisplayName = "Miss"),        // 的外れ
    Poor        UMETA(DisplayName = "Poor"),        // 不正確
    Good        UMETA(DisplayName = "Good"),        // 良好
    Perfect     UMETA(DisplayName = "Perfect")      // 完璧
};
```

### REQ-102: データ構造体の実装

#### 2.1 FDifficultySettings
```cpp
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

    // コンストラクタ
    FDifficultySettings();
};
```

#### 2.2 FWindData
```cpp
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

    // コンストラクタ
    FWindData();
};
```

#### 2.3 FLandingResult
```cpp
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

    // コンストラクタ
    FLandingResult();
};
```

#### 2.4 FGameSessionData
```cpp
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

    // コンストラクタ
    FGameSessionData();
};
```

### REQ-103: DataAsset クラスの実装

#### 3.1 UDifficultyDataAsset
```cpp
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
    UDifficultyDataAsset();
};
```

#### 3.2 UGameConfigDataAsset
```cpp
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
    UGameConfigDataAsset();
};
```

### REQ-104: デリゲート定義

#### 4.1 ゲームイベント用デリゲート
```cpp
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

## 受け入れ基準

### 機能要件

1. **構造体とデータ管理**
   - すべての構造体が適切にシリアライゼーション可能
   - Blueprint統合が完全に機能
   - デフォルト値が合理的に設定

2. **列挙型の運用**
   - 文字列変換が正常に動作
   - Blueprint上で適切に表示
   - 将来的な値追加への対応

3. **DataAsset統合**
   - エディタ上でアセット作成・編集可能
   - ランタイムでの読み込み・アクセス正常
   - 設定変更の即時反映

### 非機能要件

1. **パフォーマンス**
   - データアクセスがFrame Rateに影響しない
   - メモリ使用量が適切な範囲内
   - ガベージコレクションへの負荷最小

2. **保守性**
   - データ構造が拡張しやすい設計
   - 設定変更が容易
   - デバッグ情報が適切に表示

3. **統合性**
   - 既存の基底クラス群との完全な互換性
   - UE5のエディタツールとの統合
   - Blueprint開発者フレンドリー

## テスト要件

### 単体テスト

1. **構造体テスト**
   - デフォルト値の確認
   - メンバー変数の読み書き
   - シリアライゼーション/デシリアライゼーション

2. **列挙型テスト**
   - 値の範囲確認
   - 文字列変換の正確性
   - Blueprint統合機能

3. **DataAssetテスト**
   - アセット作成・読み込み
   - 設定値の保存・復元
   - 設定変更時の動作

### 統合テスト

1. **基底クラス連携テスト**
   - GameStateとのデータ連携
   - PlayerControllerでの設定反映
   - Characterでのステータス適用

2. **Blueprint統合テスト**
   - Blueprint上でのデータアクセス
   - UI表示での値反映
   - イベント配信機能

## 実装制約

### 技術制約
- UE5.6のUObject/UStruct システム準拠
- C++17標準に準拠
- Blueprint統合必須
- エディタツール統合

### パフォーマンス制約
- データアクセス時間は1ms以下
- メモリ使用量は構造体群全体で5MB以下
- ガベージコレクション負荷は最小限

### 設計制約
- 将来的な拡張を考慮した設計
- モジュール間の低結合を維持
- 設定の階層化と継承サポート

## 成功指標

1. **実装完了率**: 100%
2. **単体テスト成功率**: 100%
3. **Blueprint統合率**: 100%
4. **DataAsset作成成功率**: 100%
5. **パフォーマンス要件達成**: 100%
6. **コードカバレッジ**: 90%以上

## リスク要因

### 高リスク
- UE5のリフレクションシステムとの統合問題
- Blueprint統合時の型変換エラー
- DataAssetのエディタ統合問題

### 中リスク
- パフォーマンス要件の未達成
- 構造体の拡張性不足
- 設定値の妥当性検証不備

### 低リスク
- 命名規則の逸脱
- ドキュメント不足
- テストケースの不足

## 完了定義

以下の全ての条件を満たした時点で要件完了とします：

1. ✅ 全ての列挙型・構造体がコンパイル成功
2. ✅ 全DataAssetクラスが正常に動作
3. ✅ Blueprint統合が完全に機能
4. ✅ 全単体テストが成功
5. ✅ 全統合テストが成功
6. ✅ パフォーマンステストが要件を満たす
7. ✅ エディタ統合が完了
8. ✅ ドキュメントが更新完了

---

**作成日**: 2025-08-05  
**作成者**: Claude Code (TASK-102 TDD実装)  
**レビュー状態**: 未レビュー