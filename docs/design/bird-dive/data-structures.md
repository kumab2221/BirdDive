# Bird Dive Challenge データ構造設計

## セーブデータシステム

### セーブデータファイル構造

```cpp
// SaveGame クラス定義
UCLASS(BlueprintType)
class BIRDDIVE_API UBirdDiveSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // プレイヤー統計
    UPROPERTY(BlueprintReadWrite, Category = "Statistics")
    FGameStatistics PlayerStatistics;

    // ハイスコア記録
    UPROPERTY(BlueprintReadWrite, Category = "Scores")
    TMap<EDifficultyLevel, TArray<FHighScoreEntry>> HighScores;

    // 設定データ
    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FPlayerSettings PlayerSettings;

    // アンロック状況
    UPROPERTY(BlueprintReadWrite, Category = "Progress")
    FUnlockStatus UnlockStatus;

    // セーブデータバージョン
    UPROPERTY(BlueprintReadWrite, Category = "Meta")
    int32 SaveVersion = 1;

    // 最終更新日時
    UPROPERTY(BlueprintReadWrite, Category = "Meta")
    FDateTime LastUpdated;
};
```

### データ構造定義

#### ハイスコアエントリ
```cpp
USTRUCT(BlueprintType)
struct BIRDDIVE_API FHighScoreEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Score")
    int32 Score = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Score")
    float CompletionTime = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Score")
    ELandingQuality LandingQuality = ELandingQuality::Failed;

    UPROPERTY(BlueprintReadWrite, Category = "Score")
    EDifficultyLevel Difficulty = EDifficultyLevel::Normal;

    UPROPERTY(BlueprintReadWrite, Category = "Score")
    FDateTime AchievedDate;

    UPROPERTY(BlueprintReadWrite, Category = "Score")
    FString PlayerName = TEXT("Player");

    // ソート用比較演算子
    bool operator>(const FHighScoreEntry& Other) const
    {
        return Score > Other.Score;
    }
};
```

#### プレイヤー設定
```cpp
USTRUCT(BlueprintType)
struct BIRDDIVE_API FPlayerSettings
{
    GENERATED_BODY()

    // オーディオ設定
    UPROPERTY(BlueprintReadWrite, Category = "Audio")
    float MasterVolume = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Audio")
    float SFXVolume = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Audio")
    float MusicVolume = 0.7f;

    // 画質設定
    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 QualityLevel = 3; // 0-4 (Low to Epic)

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    bool bVSyncEnabled = true;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    int32 TargetFrameRate = 60;

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    FIntPoint ScreenResolution = FIntPoint(1920, 1080);

    UPROPERTY(BlueprintReadWrite, Category = "Graphics")
    bool bFullscreen = true;

    // ゲームプレイ設定
    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    float MouseSensitivity = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    bool bInvertMouseY = false;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    EDifficultyLevel PreferredDifficulty = EDifficultyLevel::Normal;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    bool bShowTutorial = true;

    // UI設定
    UPROPERTY(BlueprintReadWrite, Category = "Interface")
    float UIScale = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Interface")
    bool bShowPerformanceStats = false;

    UPROPERTY(BlueprintReadWrite, Category = "Interface")
    bool bShowDebugInfo = false;
};
```

#### アンロック状況
```cpp
USTRUCT(BlueprintType)
struct BIRDDIVE_API FUnlockStatus
{
    GENERATED_BODY()

    // 難易度アンロック
    UPROPERTY(BlueprintReadWrite, Category = "Difficulty")
    TSet<EDifficultyLevel> UnlockedDifficulties;

    // 実績アンロック
    UPROPERTY(BlueprintReadWrite, Category = "Achievements")
    TSet<FString> UnlockedAchievements;

    // カスタマイゼーション
    UPROPERTY(BlueprintReadWrite, Category = "Customization")
    TSet<FString> UnlockedBirdSkins;

    UPROPERTY(BlueprintReadWrite, Category = "Customization")
    TSet<FString> UnlockedEffects;

    // 統計ベースのアンロック
    UPROPERTY(BlueprintReadWrite, Category = "Progress")
    bool bUnlockedHardMode = false;

    UPROPERTY(BlueprintReadWrite, Category = "Progress")
    bool bUnlockedCustomization = false;
};
```

## ゲーム内データ管理

### ゲームセッションデータ
```cpp
USTRUCT(BlueprintType)
struct BIRDDIVE_API FGameSessionData
{
    GENERATED_BODY()

    // セッション基本情報
    UPROPERTY(BlueprintReadWrite, Category = "Session")
    FGuid SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "Session")
    EDifficultyLevel SelectedDifficulty = EDifficultyLevel::Normal;

    UPROPERTY(BlueprintReadWrite, Category = "Session")
    FDateTime SessionStartTime;

    UPROPERTY(BlueprintReadWrite, Category = "Session")
    float SessionDuration = 0.0f;

    // ゲームプレイデータ
    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    TArray<FVector> FlightPath;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    TArray<FWindData> WindHistory;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    TArray<FVector> InputHistory;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
    FLandingResult FinalResult;

    // パフォーマンスデータ
    UPROPERTY(BlueprintReadWrite, Category = "Performance")
    TArray<float> FrameTimeHistory;

    UPROPERTY(BlueprintReadWrite, Category = "Performance")
    float AverageFrameRate = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Performance")
    float MinFrameRate = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Performance")
    float MaxFrameRate = 0.0f;
};
```

### リアルタイムゲーム状態
```cpp
USTRUCT(BlueprintType)
struct BIRDDIVE_API FRealTimeGameState
{
    GENERATED_BODY()

    // 現在の状態
    UPROPERTY(BlueprintReadWrite, Category = "State")
    EGameState CurrentGameState = EGameState::Menu;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    float ElapsedTime = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    int32 CurrentScore = 0;

    // 鳥の状態
    UPROPERTY(BlueprintReadWrite, Category = "Bird")
    FVector BirdPosition = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Bird")
    FVector BirdVelocity = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Bird")
    FRotator BirdRotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadWrite, Category = "Bird")
    float CurrentSpeed = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Bird")
    float DistanceToTarget = 0.0f;

    // 環境状態
    UPROPERTY(BlueprintReadWrite, Category = "Environment")
    FWindData CurrentWind;

    UPROPERTY(BlueprintReadWrite, Category = "Environment")
    float TargetRadius = 100.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Environment")
    FVector TargetLocation = FVector::ZeroVector;

    // UI状態
    UPROPERTY(BlueprintReadWrite, Category = "Interface")
    bool bShowDebugInfo = false;

    UPROPERTY(BlueprintReadWrite, Category = "Interface")
    bool bGamePaused = false;

    UPROPERTY(BlueprintReadWrite, Category = "Interface")
    TArray<FString> ActiveNotifications;
};
```

## データテーブル定義

### 難易度設定テーブル
```cpp
// DataTable行構造体
USTRUCT(BlueprintType)
struct BIRDDIVE_API FDifficultyTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FDifficultySettings Settings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
    int32 UnlockRequirement = 0; // 必要スコア

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
    TArray<FString> PrerequisiteAchievements;
};
```

### スコア閾値テーブル
```cpp
USTRUCT(BlueprintType)
struct BIRDDIVE_API FScoreThresholdRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    ELandingQuality QualityLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    float MaxDistance = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    float MaxSpeed = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    int32 BaseScore = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    float ScoreMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    FText QualityText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    FLinearColor QualityColor = FLinearColor::White;
};
```

### 実績定義テーブル
```cpp
USTRUCT(BlueprintType)
struct BIRDDIVE_API FAchievementRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString AchievementID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
    int32 RequiredValue = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
    FString StatisticKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
    TArray<FString> UnlockRewards;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
    bool bIsSecret = false;
};
```

## データ永続化システム

### セーブシステムマネージャー
```cpp
UCLASS(BlueprintType)
class BIRDDIVE_API UBirdSaveManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // セーブ・ロード機能
    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool SaveGameData(const FString& SlotName = TEXT("DefaultSave"));

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool LoadGameData(const FString& SlotName = TEXT("DefaultSave"));

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool DoesSaveExist(const FString& SlotName = TEXT("DefaultSave"));

    // データアクセス
    UFUNCTION(BlueprintCallable, Category = "Data")
    UBirdDiveSaveGame* GetCurrentSaveData() const { return CurrentSaveData; }

    UFUNCTION(BlueprintCallable, Category = "Data")
    void UpdatePlayerStatistics(const FLandingResult& Result);

    UFUNCTION(BlueprintCallable, Category = "Data")
    void AddHighScore(const FHighScoreEntry& Entry);

    UFUNCTION(BlueprintCallable, Category = "Data")
    TArray<FHighScoreEntry> GetHighScores(EDifficultyLevel Difficulty, int32 MaxEntries = 10);

    // 設定管理
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyPlayerSettings(const FPlayerSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "Settings")
    FPlayerSettings GetPlayerSettings() const;

    // 実績システム
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void CheckAchievements(const FLandingResult& Result);

    UFUNCTION(BlueprintCallable, Category = "Achievements")
    bool IsAchievementUnlocked(const FString& AchievementID) const;

private:
    UPROPERTY()
    UBirdDiveSaveGame* CurrentSaveData;

    UPROPERTY()
    UDataTable* AchievementTable;

    void InitializeDefaultSaveData();
    void ValidateSaveData();
    void MigrateSaveData(int32 FromVersion, int32 ToVersion);
};
```

## メモリ管理戦略

### オブジェクトプーリング
```cpp
UCLASS()
class BIRDDIVE_API UBirdObjectPool : public UObject
{
    GENERATED_BODY()

public:
    // エフェクトプール
    UFUNCTION(BlueprintCallable, Category = "Pool")
    UNiagaraComponent* GetPooledEffect(UNiagaraSystem* System);

    UFUNCTION(BlueprintCallable, Category = "Pool")
    void ReturnPooledEffect(UNiagaraComponent* Effect);

    // オーディオプール
    UFUNCTION(BlueprintCallable, Category = "Pool")
    UAudioComponent* GetPooledAudio();

    UFUNCTION(BlueprintCallable, Category = "Pool")
    void ReturnPooledAudio(UAudioComponent* Audio);

    // プール管理
    UFUNCTION(BlueprintCallable, Category = "Pool")
    void InitializePools(int32 EffectPoolSize = 10, int32 AudioPoolSize = 5);

    UFUNCTION(BlueprintCallable, Category = "Pool")
    void CleanupPools();

private:
    UPROPERTY()
    TArray<UNiagaraComponent*> EffectPool;

    UPROPERTY()
    TArray<UAudioComponent*> AudioPool;

    UPROPERTY()
    TArray<UNiagaraComponent*> ActiveEffects;

    UPROPERTY()
    TArray<UAudioComponent*> ActiveAudio;
};
```

### ストリーミング管理
```cpp
UCLASS()
class BIRDDIVE_API UBirdAssetManager : public UObject
{
    GENERATED_BODY()

public:
    // アセット非同期ロード
    UFUNCTION(BlueprintCallable, Category = "Asset Loading")
    void LoadAssetsAsync(const TArray<TSoftObjectPtr<UObject>>& AssetsToLoad, 
                        FStreamableDelegate OnLoadComplete);

    // アセットアンロード
    UFUNCTION(BlueprintCallable, Category = "Asset Loading")
    void UnloadAssets(const TArray<TSoftObjectPtr<UObject>>& AssetsToUnload);

    // プリロード管理
    UFUNCTION(BlueprintCallable, Category = "Asset Loading")
    void PreloadEssentialAssets();

    UFUNCTION(BlueprintCallable, Category = "Asset Loading")
    void PreloadDifficultyAssets(EDifficultyLevel Difficulty);

private:
    UPROPERTY()
    TSharedPtr<FStreamableHandle> CurrentHandle;

    UPROPERTY()
    TArray<UObject*> LoadedAssets;

    void OnAssetLoadComplete();
};
```