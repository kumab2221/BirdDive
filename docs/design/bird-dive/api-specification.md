# Bird Dive Challenge API仕様書

## Blueprint関数・イベント仕様

### ゲームプレイAPI

#### BirdDiveGameMode

##### StartGame
```cpp
UFUNCTION(BlueprintCallable, Category = "Gameplay")
void StartGame(EDifficultyLevel Difficulty);
```
**説明**: 指定された難易度でゲームを開始する
**パラメータ**: 
- `Difficulty`: 難易度レベル (Easy/Normal/Hard)
**戻り値**: なし
**使用例**:
```blueprint
StartGame(EDifficultyLevel::Normal)
```

##### EndGame
```cpp
UFUNCTION(BlueprintCallable, Category = "Gameplay")
void EndGame(const FLandingResult& Result);
```
**説明**: ゲームを終了し、結果を処理する
**パラメータ**:
- `Result`: 着地結果データ
**戻り値**: なし

##### CalculateScore
```cpp
UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Scoring")
int32 CalculateScore(float DistanceFromTarget, float GameTime, EDifficultyLevel Difficulty) const;
```
**説明**: 距離、時間、難易度からスコアを計算
**パラメータ**:
- `DistanceFromTarget`: 的までの距離
- `GameTime`: ゲーム経過時間
- `Difficulty`: 難易度
**戻り値**: 計算されたスコア

#### BirdCharacter

##### ApplyMovementInput
```cpp
UFUNCTION(BlueprintCallable, Category = "Input")
void ApplyMovementInput(FVector2D InputVector, float DeltaTime);
```
**説明**: プレイヤー入力を鳥の移動に適用
**パラメータ**:
- `InputVector`: 入力ベクトル (-1.0 ～ 1.0)
- `DeltaTime`: フレーム時間
**戻り値**: なし

##### GetCurrentFlightSpeed
```cpp
UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Physics")
float GetCurrentFlightSpeed() const;
```
**説明**: 現在の飛行速度を取得
**戻り値**: 速度 (cm/s)

##### CheckLandingConditions
```cpp
UFUNCTION(BlueprintCallable, Category = "Landing")
bool CheckLandingConditions(FLandingResult& OutResult);
```
**説明**: 着地条件をチェックし、結果を返す
**パラメータ**:
- `OutResult`: 着地結果（出力パラメータ）
**戻り値**: 着地可能かどうか

### カメラシステムAPI

#### SplitScreenManager

##### UpdateCameraViews
```cpp
UFUNCTION(BlueprintCallable, Category = "Camera")
void UpdateCameraViews(FVector BirdLocation, FVector BirdVelocity, float DeltaTime);
```
**説明**: 鳥の位置と速度に基づいてカメラビューを更新
**パラメータ**:
- `BirdLocation`: 鳥の現在位置
- `BirdVelocity`: 鳥の現在速度
- `DeltaTime`: フレーム時間

##### SetSplitScreenRatio
```cpp
UFUNCTION(BlueprintCallable, Category = "Display")
void SetSplitScreenRatio(float Ratio);
```
**説明**: 画面分割比率を設定
**パラメータ**:
- `Ratio`: 分割比率 (0.0 ～ 1.0)

##### GetCameraFOV
```cpp
UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Camera")
float GetCameraFOV(ECameraViewType ViewType) const;
```
**説明**: 指定されたカメラビューのFOVを取得
**パラメータ**:
- `ViewType`: カメラビュータイプ
**戻り値**: FOV値

### 物理システムAPI

#### WindForceComponent

##### CalculateWindForce
```cpp
UFUNCTION(BlueprintCallable, Category = "Physics")
FVector CalculateWindForce(float DeltaTime, FVector CurrentLocation);
```
**説明**: 現在位置とフレーム時間から風力を計算
**パラメータ**:
- `DeltaTime`: フレーム時間
- `CurrentLocation`: 現在位置
**戻り値**: 風力ベクトル

##### SetWindParameters
```cpp
UFUNCTION(BlueprintCallable, Category = "Settings")
void SetWindParameters(const FDifficultySettings& Settings);
```
**説明**: 風力パラメータを設定
**パラメータ**:
- `Settings`: 難易度設定

##### GetCurrentWindData  
```cpp
UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Data")
FWindData GetCurrentWindData() const;
```
**説明**: 現在の風データを取得
**戻り値**: 風データ構造体

### エフェクトシステムAPI

#### BirdEffectManager

##### PlayLandingEffect
```cpp
UFUNCTION(BlueprintCallable, Category = "Effects")
void PlayLandingEffect(ELandingQuality Quality, FVector Location, FRotator Rotation);
```
**説明**: 着地品質に応じたエフェクトを再生
**パラメータ**:
- `Quality`: 着地品質
- `Location`: 再生位置
- `Rotation`: 再生方向

##### StopAllEffects
```cpp
UFUNCTION(BlueprintCallable, Category = "Effects")
void StopAllEffects();
```
**説明**: 全てのエフェクトを停止

##### PreloadEffectSystems
```cpp
UFUNCTION(BlueprintCallable, Category = "Optimization")
void PreloadEffectSystems(const TArray<UNiagaraSystem*>& Systems);
```
**説明**: エフェクトシステムを事前ロード
**パラメータ**:
- `Systems`: プリロードするエフェクトシステム配列

### UIシステムAPI

#### GameplayHUD

##### UpdateScoreDisplay
```cpp
UFUNCTION(BlueprintImplementableEvent, Category = "UI")
void UpdateScoreDisplay(int32 NewScore, int32 ScoreChange);
```
**説明**: スコア表示を更新（Blueprint実装）
**パラメータ**:
- `NewScore`: 新しいスコア
- `ScoreChange`: スコア変化量

##### UpdateTimerDisplay
```cpp
UFUNCTION(BlueprintImplementableEvent, Category = "UI")  
void UpdateTimerDisplay(float ElapsedTime, bool bWarning);
```
**説明**: タイマー表示を更新
**パラメータ**: 
- `ElapsedTime`: 経過時間
- `bWarning`: 警告状態かどうか

##### ShowSpeedWarning
```cpp
UFUNCTION(BlueprintImplementableEvent, Category = "UI")
void ShowSpeedWarning(float CurrentSpeed, float MaxSpeed);
```
**説明**: 速度警告を表示
**パラメータ**:
- `CurrentSpeed`: 現在速度
- `MaxSpeed`: 最大許容速度

### データ管理API

#### BirdSaveManager

##### SaveGameData
```cpp
UFUNCTION(BlueprintCallable, Category = "Save System")
bool SaveGameData(const FString& SlotName = TEXT("DefaultSave"));
```
**説明**: ゲームデータをセーブ
**パラメータ**:
- `SlotName`: セーブスロット名
**戻り値**: セーブ成功可否

##### LoadGameData
```cpp
UFUNCTION(BlueprintCallable, Category = "Save System")
bool LoadGameData(const FString& SlotName = TEXT("DefaultSave"));
```
**説明**: ゲームデータをロード
**パラメータ**:
- `SlotName`: セーブスロット名
**戻り値**: ロード成功可否

##### GetHighScores
```cpp
UFUNCTION(BlueprintCallable, Category = "Data")
TArray<FHighScoreEntry> GetHighScores(EDifficultyLevel Difficulty, int32 MaxEntries = 10);
```
**説明**: ハイスコア一覧を取得
**パラメータ**:
- `Difficulty`: 難易度
- `MaxEntries`: 最大取得件数
**戻り値**: ハイスコアエントリ配列

##### UpdatePlayerStatistics
```cpp
UFUNCTION(BlueprintCallable, Category = "Statistics")
void UpdatePlayerStatistics(const FLandingResult& Result);
```
**説明**: プレイヤー統計を更新
**パラメータ**:
- `Result`: ゲーム結果

### 設定・コンフィグAPI

#### GameConfigManager

##### ApplyVideoSettings
```cpp
UFUNCTION(BlueprintCallable, Category = "Settings")
void ApplyVideoSettings(const FVideoSettings& Settings);
```
**説明**: ビデオ設定を適用
**パラメータ**:
- `Settings`: ビデオ設定構造体

##### ApplyAudioSettings
```cpp
UFUNCTION(BlueprintCallable, Category = "Settings")
void ApplyAudioSettings(const FAudioSettings& Settings);
```
**説明**: オーディオ設定を適用
**パラメータ**:
- `Settings`: オーディオ設定構造体

##### GetCurrentSettings
```cpp
UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Settings")
FPlayerSettings GetCurrentSettings() const;
```
**説明**: 現在の設定を取得
**戻り値**: プレイヤー設定構造体

### デバッグ・開発支援API

#### BirdDebugManager

##### SetDebugVisualization
```cpp
UFUNCTION(BlueprintCallable, Category = "Debug", CallInEditor = true)
void SetDebugVisualization(bool bEnabled, EDebugVisualizationType Type);
```
**説明**: デバッグ可視化の有効/無効を設定
**パラメータ**:
- `bEnabled`: 有効フラグ
- `Type`: 可視化タイプ

##### LogPerformanceMetrics
```cpp
UFUNCTION(BlueprintCallable, Category = "Debug")
void LogPerformanceMetrics();
```
**説明**: パフォーマンス統計をログ出力

##### TeleportBirdToHeight
```cpp
UFUNCTION(BlueprintCallable, Category = "Debug", CallInEditor = true)
void TeleportBirdToHeight(float Height);
```
**説明**: 鳥を指定高度にテレポート（デバッグ用）
**パラメータ**:
- `Height`: 目標高度

### イベント・デリゲートAPI

#### ゲームプレイイベント

##### OnGameStarted
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStarted, EDifficultyLevel, Difficulty);
```
**説明**: ゲーム開始時に発火
**パラメータ**: 選択された難易度

##### OnGameEnded  
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, const FLandingResult&, Result);
```
**説明**: ゲーム終了時に発火  
**パラメータ**: ゲーム結果

##### OnScoreChanged
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChanged, int32, NewScore, int32, Delta);
```
**説明**: スコア変更時に発火
**パラメータ**: 新スコア、変化量

##### OnSpeedWarning
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpeedWarning, float, CurrentSpeed, float, MaxSpeed);
```
**説明**: 速度警告時に発火
**パラメータ**: 現在速度、最大速度

### パフォーマンスAPI

#### PerformanceMonitor

##### GetCurrentFPS
```cpp
UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Performance")
float GetCurrentFPS() const;
```
**説明**: 現在のFPSを取得
**戻り値**: FPS値

##### GetMemoryUsage
```cpp
UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Performance")
float GetMemoryUsage() const;
```
**説明**: メモリ使用量を取得（MB）
**戻り値**: メモリ使用量

##### StartPerformanceCapture
```cpp
UFUNCTION(BlueprintCallable, Category = "Performance")
void StartPerformanceCapture(float Duration);
```
**説明**: パフォーマンス計測を開始
**パラメータ**:
- `Duration`: 計測時間（秒）

## エラーハンドリング

### エラーコード定義
```cpp
UENUM(BlueprintType)
enum class EBirdDiveErrorCode : uint8
{
    None                UMETA(DisplayName = "No Error"),
    InvalidDifficulty   UMETA(DisplayName = "Invalid Difficulty"),
    SaveLoadFailed      UMETA(DisplayName = "Save/Load Failed"),
    AssetLoadFailed     UMETA(DisplayName = "Asset Load Failed"),
    PhysicsError        UMETA(DisplayName = "Physics Error"),
    RenderingError      UMETA(DisplayName = "Rendering Error"),
    AudioError          UMETA(DisplayName = "Audio Error"),
    NetworkError        UMETA(DisplayName = "Network Error")
};
```

### エラーハンドリング関数
```cpp
UFUNCTION(BlueprintCallable, Category = "Error Handling")
void HandleGameError(EBirdDiveErrorCode ErrorCode, const FString& ErrorMessage);

UFUNCTION(BlueprintCallable, Category = "Error Handling")  
bool RecoverFromError(EBirdDiveErrorCode ErrorCode);

UFUNCTION(BlueprintImplementableEvent, Category = "Error Handling")
void OnErrorOccurred(EBirdDiveErrorCode ErrorCode, const FString& Message);
```

## API使用例

### ゲーム開始シーケンス
```blueprint
// メニューからゲーム開始
1. GetDifficultySelection() -> EDifficultyLevel
2. ValidateGameStart() -> bool
3. LoadGameAssets() -> void
4. InitializeGameSession() -> void
5. StartGame(Difficulty) -> void
6. SpawnBirdCharacter() -> ABirdCharacter*
7. SetupCameras() -> void
8. BeginGameplay() -> void
```

### 着地処理シーケンス
```blueprint
// 着地検出から結果表示まで
1. OnHit(HitResult) -> void
2. CheckLandingConditions() -> FLandingResult
3. CalculateScore() -> int32
4. PlayLandingEffect() -> void
5. UpdatePlayerStatistics() -> void
6. SaveGameData() -> bool
7. ShowResultScreen() -> void
```

### 設定変更シーケンス  
```blueprint
// 設定画面での変更適用
1. GetCurrentSettings() -> FPlayerSettings
2. ModifySettings() -> FPlayerSettings
3. ValidateSettings() -> bool
4. ApplyVideoSettings() -> void
5. ApplyAudioSettings() -> void
6. SaveSettings() -> bool
7. UpdateUI() -> void
```

## APIバージョニング

- **Version 1.0**: 基本ゲームプレイ機能
- **Version 1.1**: デバッグ機能拡張
- **Version 1.2**: パフォーマンス最適化API
- **Version 2.0**: マルチプレイヤー対応（将来）

各バージョンの互換性は後方互換性を保持し、廃止予定APIには適切な警告を表示します。