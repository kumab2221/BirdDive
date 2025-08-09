# TASK-302: 第一人称カメラ実装 - 要件定義

## 目標

Bird Dive Challengeの第一人称視点カメラシステムを実装する。鳥の頭部に追従し、速度に連動したFOV調整とポストプロセスエフェクトにより、没入感のある飛行体験を提供する。

## 機能要件

### 1. FirstPersonCameraComponentの実装

- **コンポーネント設計**: USceneComponent継承のカメラコンポーネント
- **アタッチ対象**: BirdCharacterの頭部ボーンまたはMeshコンポーネント
- **カメラタイプ**: UCameraComponentを内包した管理コンポーネント
- **統合**: 分割スクリーンシステムとの連携対応

### 2. 頭部追従システム

- **追従精度**: 鳥の頭部ボーンまたはSocketに正確に追従
- **スムーズ追従**: ラグやジッターのない滑らかな追従
- **回転同期**: 鳥の頭部回転との完全同期
- **オフセット調整**: カメラ位置の微調整可能

### 3. 速度連動FOV調整システム

- **ベースFOV**: 標準飛行時のベースFOV値 (90度)
- **FOV範囲**: 最小60度〜最大120度の動的範囲
- **速度マッピング**: 飛行速度に応じたFOV値の計算
- **スムーズ補間**: FOV変更時の滑らかなトランジション

### 4. ポストプロセスエフェクトシステム

- **モーションブラー**: 高速飛行時の動きの強調
- **歪みエフェクト**: 極高速時の視界歪み効果
- **明度調整**: 速度に応じた明度変化
- **エフェクト制御**: 酔いにくい設定の提供

## 技術要件

### アーキテクチャ設計

```cpp
// First Person Camera Component
UCLASS(BlueprintType, Blueprintable, Category = "Camera")
class BIRDDIVE_API UFirstPersonCameraComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    // Camera configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float BaseFOV = 90.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinFOV = 60.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxFOV = 120.0f;
    
    // Speed-based FOV adjustment
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
    UCurveFloat* SpeedToFOVCurve;
    
    // Camera management
    UFUNCTION(BlueprintCallable, Category = "Camera")
    UCameraComponent* GetCameraComponent() const;
    
    UFUNCTION(BlueprintCallable, Category = "Camera") 
    void UpdateFOV(float CurrentSpeed);
    
    // Post process effects
    UFUNCTION(BlueprintCallable, Category = "Effects")
    void SetMotionBlurIntensity(float Intensity);
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    UPostProcessComponent* PostProcessComponent;
};
```

### データ構造定義

```cpp
// Camera settings structure
USTRUCT(BlueprintType)
struct FFirstPersonCameraSettings
{
    GENERATED_BODY()
    
    // FOV settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
    float BaseFOV = 90.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
    float MinFOV = 60.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
    float MaxFOV = 120.0f;
    
    // Speed mapping
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    float MinSpeedForFOVChange = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    float MaxSpeedForFOVChange = 2000.0f;
    
    // Smoothing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
    float FOVInterpolationSpeed = 5.0f;
    
    // Post process
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float MaxMotionBlurAmount = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float SpeedThresholdForEffects = 500.0f;
    
    FFirstPersonCameraSettings()
    {
        BaseFOV = 90.0f;
        MinFOV = 60.0f;
        MaxFOV = 120.0f;
        MinSpeedForFOVChange = 100.0f;
        MaxSpeedForFOVChange = 2000.0f;
        FOVInterpolationSpeed = 5.0f;
        MaxMotionBlurAmount = 0.5f;
        SpeedThresholdForEffects = 500.0f;
    }
};
```

## パフォーマンス要件

### フレームレート目標
- **60FPS維持**: カメラ更新・エフェクト処理でも安定した60FPS
- **CPU負荷**: 追従計算処理時間 < 0.1ms
- **GPU負荷**: ポストプロセス負荷を最小限に抑制
- **メモリ使用量**: 追加メモリ使用量 < 10MB

### レスポンス性能
- **追従遅延**: 頭部動きから1フレーム以内の追従
- **FOV更新**: 速度変化から0.1秒以内のFOV反映
- **エフェクト適用**: リアルタイムエフェクト更新

## カメラ統合仕様

### BirdCharacterとの統合
- **取り付け位置**: 頭部ボーン ("head" socket) への取り付け
- **初期化タイミング**: BeginPlay時の自動セットアップ
- **更新タイミング**: Tick処理での継続的更新
- **クリーンアップ**: EndPlay時の適切な破棄

### SplitScreenManagerとの連携
- **カメラ登録**: 第一人称カメラの分割スクリーンへの登録
- **レンダーターゲット**: 上部/左側ビューポートへの割り当て
- **同期処理**: 分割スクリーンとのフレーム同期

## 品質要件

### 没入感
- **自然な視点**: 鳥の目線に合致した視点
- **滑らかな動き**: ジャークやティアリングのない滑らかさ
- **直感的FOV**: 速度感を自然に表現するFOV変化
- **適切なエフェクト**: 過度でない自然なエフェクト

### 快適性
- **酔いにくさ**: モーションシックネス軽減
- **視認性**: 重要な情報の視認性確保
- **調整可能性**: プレイヤー好みに応じた設定調整
- **一貫性**: 安定した視点体験

### 安定性
- **エラー耐性**: ボーン欠損・メッシュ変更への対応
- **メモリ管理**: メモリリーク防止
- **パフォーマンス**: 負荷変動時の安定性
- **デバッグ支援**: 開発時のデバッグ情報

## インターフェース仕様

### Blueprint統合

```cpp
// Blueprint callable functions
UFUNCTION(BlueprintCallable, Category = "First Person Camera")
void SetBaseFOV(float NewFOV);

UFUNCTION(BlueprintCallable, Category = "First Person Camera")
float GetCurrentFOV() const;

UFUNCTION(BlueprintCallable, Category = "First Person Camera")
void SetFOVRange(float NewMinFOV, float NewMaxFOV);

UFUNCTION(BlueprintCallable, Category = "First Person Camera")
void EnableSpeedBasedFOV(bool bEnable);

UFUNCTION(BlueprintCallable, Category = "First Person Camera")
void SetPostProcessEffects(bool bEnable);

// Blueprint events
UFUNCTION(BlueprintImplementableEvent, Category = "First Person Camera")
void OnFOVChanged(float OldFOV, float NewFOV);

UFUNCTION(BlueprintImplementableEvent, Category = "First Person Camera")
void OnHighSpeedEffectActivated(float Speed);
```

### C++インターフェース

```cpp
// Public methods
class BIRDDIVE_API UFirstPersonCameraComponent
{
public:
    void InitializeCamera();
    void UpdateCamera(float DeltaTime);
    void Shutdown();
    
    // FOV management
    void SetBaseFOV(float NewFOV);
    float GetCurrentFOV() const;
    void UpdateFOVBasedOnSpeed(float CurrentSpeed);
    
    // Attachment
    bool AttachToSocketOrBone(USkeletalMeshComponent* Mesh, const FName& SocketName);
    void DetachFromParent();
    
    // Effects
    void UpdatePostProcessEffects(float Speed, float DeltaTime);
    void SetMotionBlurIntensity(float Intensity);
    
    // Settings
    void ApplySettings(const FFirstPersonCameraSettings& Settings);
    const FFirstPersonCameraSettings& GetSettings() const;
};
```

## テスト観点

### 単体テスト
- カメラコンポーネント初期化の正確性
- FOV計算アルゴリズムの精度
- 速度マッピング関数の動作確認
- ポストプロセス設定の適用確認

### 統合テスト
- BirdCharacterとの統合確認
- SplitScreenManagerとの連携
- 物理システムとの協調動作
- 入力システムとの連携

### パフォーマンステスト
- CPU使用率測定（カメラ更新処理）
- GPU負荷測定（ポストプロセス）
- メモリ使用量測定
- フレームレート安定性確認

### 視覚テスト
- カメラ追従の滑らかさ確認
- FOV変化の自然さ評価
- エフェクト品質の確認
- 酔いにくさの主観評価

## 受け入れ基準

### ✅ 必須機能
- [ ] 鳥の頭部に正確に追従する
- [ ] 速度に応じてFOVが動的に変化する
- [ ] 分割スクリーンシステムと統合される
- [ ] 60FPS@1080pで安定動作する
- [ ] メモリリークが発生しない

### ✅ 品質基準
- [ ] 追従遅延が1フレーム以内
- [ ] FOV変化が滑らかで自然
- [ ] ポストプロセスエフェクトが適切に機能
- [ ] 酔いにくい快適な視点を提供

### ✅ ユーザビリティ
- [ ] 直感的な設定インターフェース
- [ ] Blueprint統合の完全性
- [ ] デバッグ情報の適切な表示
- [ ] 設定変更の即座反映

## 実装方針

### フェーズ1: 基本カメラシステム
1. UFirstPersonCameraComponent作成
2. 基本的なカメラ追従実装
3. BirdCharacterへの統合

### フェーズ2: FOV管理システム
1. 速度ベースFOV計算実装
2. スムーズなFOV補間
3. 設定可能なFOV範囲

### フェーズ3: エフェクトシステム
1. ポストプロセス統合
2. モーションブラー実装
3. エフェクト制御システム

### フェーズ4: 統合・最適化
1. SplitScreenManagerとの統合
2. パフォーマンス最適化
3. Blueprint統合とUI

## リスク・考慮事項

### 技術的リスク
- 頭部ボーン・ソケットの可用性
- ポストプロセス負荷による性能影響
- カメラ更新頻度による負荷

### 対応策
- 代替アタッチメント方法の準備
- エフェクト品質の動的調整
- 効率的な更新スケジューリング

## 成功指標
- 滑らかで自然なカメラ追従の実現
- 速度感を効果的に表現するFOV変化
- 酔いにくい快適な視点体験の提供
- 分割スクリーンでの安定した動作