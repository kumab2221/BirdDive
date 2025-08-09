# TASK-303: サイドビューカメラ実装 - 要件定義

## 目標

Bird Dive Challengeのサイドビューカメラシステムを実装する。横視点固定カメラで鳥の飛行を追従し、先読み機能により自然で予測可能なカメラワークを提供する。

## 機能要件

### 1. SideViewCameraComponentの実装

- **コンポーネント設計**: USceneComponent継承のカメラコンポーネント
- **カメラタイプ**: UCameraComponentを内包した管理コンポーネント
- **視点**: 横視点固定（Y軸方向から見た90度視点）
- **統合**: 分割スクリーンシステムとの連携対応

### 2. 鳥追従システム

- **追従精度**: 鳥の位置を滑らかに追従
- **距離維持**: 適切な距離を保持（設定可能）
- **高度追従**: 鳥の高度変化に対応
- **境界制限**: 設定された境界内でのカメラ移動制限

### 3. 先読み機能システム

- **速度連動**: 鳥の速度に応じた先読み距離
- **進行方向予測**: 鳥の進行方向を予測してカメラ位置調整
- **スムーズ補間**: 急激な方向転換時の滑らかなカメラ移動
- **設定可能**: 先読み強度・距離の調整機能

### 4. カメラ位置・角度制御

- **固定角度**: 横視点の固定角度維持
- **動的距離**: 鳥の速度に応じた動的距離調整
- **画面内保持**: 鳥が常に画面内に収まる制御
- **境界処理**: マップ境界での適切な処理

## 技術要件

### アーキテクチャ設計

```cpp
// Side View Camera Component
UCLASS(BlueprintType, Blueprintable, Category = "Camera")
class BIRDDIVE_API USideViewCameraComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    // Camera configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FollowDistance = 500.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FollowHeight = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float LookAheadDistance = 200.0f;
    
    // Following system
    UFUNCTION(BlueprintCallable, Category = "Side View Camera")
    void SetTargetActor(AActor* NewTarget);
    
    UFUNCTION(BlueprintCallable, Category = "Side View Camera")
    AActor* GetTargetActor() const;
    
    UFUNCTION(BlueprintCallable, Category = "Side View Camera")
    void UpdateCameraPosition(float DeltaTime);
    
    // Look-ahead system
    UFUNCTION(BlueprintCallable, Category = "Side View Camera")
    void SetLookAheadStrength(float Strength);
    
    UFUNCTION(BlueprintCallable, Category = "Side View Camera")
    FVector CalculateLookAheadPosition() const;
    
    // Camera management
    UFUNCTION(BlueprintCallable, Category = "Side View Camera")
    UCameraComponent* GetCameraComponent() const;
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComponent;
    
    UPROPERTY()
    AActor* TargetActor;
    
    FVector PreviousTargetPosition;
    FVector TargetVelocity;
    bool bIsInitialized;
};
```

### データ構造定義

```cpp
// Side View Camera Settings structure
USTRUCT(BlueprintType)
struct FSideViewCameraSettings
{
    GENERATED_BODY()
    
    // Following settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
    float FollowDistance = 500.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
    float FollowHeight = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
    float MinFollowDistance = 200.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
    float MaxFollowDistance = 1000.0f;
    
    // Look-ahead settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
    float LookAheadDistance = 200.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
    float LookAheadStrength = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
    float MaxLookAheadDistance = 400.0f;
    
    // Smoothing settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
    float FollowSmoothSpeed = 5.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
    float LookAheadSmoothSpeed = 3.0f;
    
    // Boundary settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boundary")
    FVector BoundaryMin = FVector(-5000, -5000, -1000);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boundary")
    FVector BoundaryMax = FVector(5000, 5000, 2000);
    
    // Screen keep settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
    float ScreenMargin = 0.1f; // 10% margin from screen edge
    
    FSideViewCameraSettings()
    {
        FollowDistance = 500.0f;
        FollowHeight = 0.0f;
        MinFollowDistance = 200.0f;
        MaxFollowDistance = 1000.0f;
        LookAheadDistance = 200.0f;
        LookAheadStrength = 1.0f;
        MaxLookAheadDistance = 400.0f;
        FollowSmoothSpeed = 5.0f;
        LookAheadSmoothSpeed = 3.0f;
        BoundaryMin = FVector(-5000, -5000, -1000);
        BoundaryMax = FVector(5000, 5000, 2000);
        ScreenMargin = 0.1f;
    }
};
```

## パフォーマンス要件

### フレームレート目標
- **60FPS維持**: カメラ更新・追従処理でも安定した60FPS
- **CPU負荷**: 追従計算処理時間 < 0.05ms
- **メモリ使用量**: 追加メモリ使用量 < 5MB
- **予測精度**: 先読み計算の正確性

### レスポンス性能
- **追従遅延**: 鳥の動きから1フレーム以内の追従
- **先読み更新**: 速度変化から0.1秒以内の先読み位置反映
- **境界処理**: 境界到達時の滑らかな制限

## カメラ統合仕様

### BirdCharacterとの統合
- **ターゲット設定**: BirdCharacterをターゲットとして設定
- **初期化タイミング**: BeginPlay時の自動セットアップ
- **更新タイミング**: Tick処理での継続的更新
- **クリーンアップ**: EndPlay時の適切な破棄

### SplitScreenManagerとの連携
- **カメラ登録**: サイドビューカメラの分割スクリーンへの登録
- **レンダーターゲット**: 下部/右側ビューポートへの割り当て
- **同期処理**: 分割スクリーンとのフレーム同期

## 品質要件

### 自然なカメラワーク
- **滑らかな追従**: ジャークやティアリングのない滑らかな動き
- **予測可能性**: プレイヤーが予測しやすいカメラ動作
- **適切な距離**: 鳥が見やすい適切な距離維持
- **画面内保持**: 鳥が画面外に出ない制御

### 安定性・信頼性
- **エラー耐性**: ターゲット喪失・null参照への対応
- **メモリ管理**: メモリリーク防止
- **パフォーマンス**: 負荷変動時の安定性
- **デバッグ支援**: 開発時のデバッグ情報

### 設定柔軟性
- **調整可能**: 追従距離・先読み強度の動的調整
- **境界設定**: マップに応じた境界設定
- **プリセット**: 異なるゲームシーンに応じたプリセット
- **リアルタイム**: ゲーム中の設定変更対応

## インターフェース仕様

### Blueprint統合

```cpp
// Blueprint callable functions
UFUNCTION(BlueprintCallable, Category = "Side View Camera")
void SetFollowDistance(float NewDistance);

UFUNCTION(BlueprintCallable, Category = "Side View Camera")
float GetFollowDistance() const;

UFUNCTION(BlueprintCallable, Category = "Side View Camera")
void SetLookAheadStrength(float Strength);

UFUNCTION(BlueprintCallable, Category = "Side View Camera")
float GetLookAheadStrength() const;

UFUNCTION(BlueprintCallable, Category = "Side View Camera")
void SetTargetActor(AActor* NewTarget);

UFUNCTION(BlueprintCallable, Category = "Side View Camera")
void SetBoundaryLimits(FVector MinBounds, FVector MaxBounds);

// Blueprint events
UFUNCTION(BlueprintImplementableEvent, Category = "Side View Camera")
void OnTargetOutOfBounds(const FVector& TargetLocation);

UFUNCTION(BlueprintImplementableEvent, Category = "Side View Camera")
void OnCameraReachedBoundary(const FVector& BoundaryLocation);
```

### C++インターフェース

```cpp
// Public methods
class BIRDDIVE_API USideViewCameraComponent
{
public:
    void InitializeCamera();
    void UpdateCamera(float DeltaTime);
    void Shutdown();
    
    // Following system
    void SetTargetActor(AActor* NewTarget);
    AActor* GetTargetActor() const;
    void UpdateFollowPosition(float DeltaTime);
    
    // Look-ahead system
    FVector CalculateLookAheadPosition() const;
    void UpdateLookAhead(float DeltaTime);
    
    // Boundary system
    void SetBoundaryLimits(const FVector& MinBounds, const FVector& MaxBounds);
    bool IsPositionInBounds(const FVector& Position) const;
    FVector ClampToBoundary(const FVector& Position) const;
    
    // Settings
    void ApplySettings(const FSideViewCameraSettings& Settings);
    const FSideViewCameraSettings& GetSettings() const;
};
```

## テスト観点

### 単体テスト
- カメラコンポーネント初期化の正確性
- 追従アルゴリズムの精度
- 先読み計算の正確性
- 境界制限の動作確認

### 統合テスト
- BirdCharacterとの統合確認
- SplitScreenManagerとの連携
- 物理システムとの協調動作
- 複雑な飛行パターンでの追従確認

### パフォーマンステスト
- CPU使用率測定（追従処理）
- メモリ使用量測定
- フレームレート安定性確認
- 境界処理の負荷測定

### 視覚テスト
- カメラ追従の滑らかさ確認
- 先読み機能の自然さ評価
- 画面内保持機能の確認
- 境界処理の視覚的確認

## 受け入れ基準

### ✅ 必須機能
- [ ] 鳥を滑らかに追従する
- [ ] 先読み機能が自然に動作する
- [ ] 鳥が常に適切に画面内に収まる
- [ ] 分割スクリーンシステムと統合される
- [ ] 60FPS@1080pで安定動作する

### ✅ 品質基準
- [ ] 追従遅延が1フレーム以内
- [ ] 先読み予測が自然で予測可能
- [ ] 境界処理が滑らかで自然
- [ ] CPU負荷が0.05ms以下

### ✅ ユーザビリティ
- [ ] 直感的な設定インターフェース
- [ ] Blueprint統合の完全性
- [ ] デバッグ情報の適切な表示
- [ ] リアルタイム設定変更対応

## 実装方針

### フェーズ1: 基本カメラシステム
1. USideViewCameraComponent作成
2. 基本的な鳥追従実装
3. UCameraComponent統合

### フェーズ2: 先読みシステム
1. 速度ベース先読み計算実装
2. スムーズな先読み補間
3. 設定可能な先読み強度

### フェーズ3: 境界・制限システム
1. 境界設定・制限機能
2. 画面内保持機能
3. 境界到達時の処理

### フェーズ4: 統合・最適化
1. SplitScreenManagerとの統合
2. パフォーマンス最適化
3. Blueprint統合とUI

## リスク・考慮事項

### 技術的リスク
- 高速移動時の追従精度
- 急激な方向転換への対応
- 境界処理の複雑性

### 対応策
- 効率的な予測アルゴリズム採用
- 適切な補間・フィルタリング
- 段階的な境界制限実装

## 成功指標
- 滑らかで予測可能なカメラワークの実現
- 鳥が常に画面内に適切に表示される
- 自然で直感的な先読み機能の提供
- 分割スクリーンでの安定した動作