# TASK-103: 物理システム基盤実装 - 要件定義

## 概要

鳥の飛行物理を制御する基盤システムを実装します。風力計算、重力、空気抵抗、キャラクター移動コンポーネントを統合し、リアルで制御可能な飛行体験を提供します。

## 機能要件

### REQ-001: 鳥の基本物理
- 鳥キャラクターは重力の影響を受けて落下する
- プレイヤー入力により方向制御が可能
- 最大速度制限により安全な着地を実現
- 物理的に妥当な挙動を示す

### REQ-104: 風力システム
- Perlinノイズベースのリアルな風力計算
- 難易度別に風力強度を調整可能
- 位置と高度に応じて風力が変化
- パフォーマンスに配慮した効率的な計算

### REQ-105: キャラクター移動
- カスタムCharacterMovementComponentによる拡張
- 風力、重力、空気抵抗の統合処理
- スムーズで制御しやすい移動応答
- 物理パラメータのBlueprint公開

## 実装詳細

### 1. UWindForceComponent
```cpp
class BIRDDIVE_API UWindForceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Wind calculation functions
    UFUNCTION(BlueprintCallable)
    FVector CalculateWindForce(const FVector& Position, float Height) const;
    
    // Perlin noise wind generation
    UFUNCTION(BlueprintCallable)
    float GetPerlinNoiseValue(const FVector& Position, float Time) const;
    
    // Difficulty-based parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WindStrength = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float NoiseScale = 0.1f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeScale = 1.0f;
};
```

### 2. UBirdCharacterMovementComponent
```cpp
class BIRDDIVE_API UBirdCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    // Override physics integration
    virtual void PhysicsRotation(float DeltaTime) override;
    virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
    
    // Wind force integration
    UFUNCTION(BlueprintCallable)
    void ApplyWindForce(const FVector& WindForce, float DeltaTime);
    
    // Air resistance simulation
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AirResistance = 0.02f;
    
    // Custom gravity multiplier
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GravityMultiplier = 1.0f;
    
    // Max speed constraints
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxDiveSpeed = 2000.0f;
};
```

## 非機能要件

### パフォーマンス
- 風力計算処理時間 < 1ms
- 60FPS維持（分割スクリーン環境下でも）
- メモリ使用量最小化

### 品質
- 物理挙動の安定性確保
- 異常値検出と自動補正
- デバッグ情報の可視化対応

## テスト要件

### 単体テスト
1. **風力計算精度テスト**
   - Perlinノイズの値域確認（-1.0 ～ 1.0）
   - 難易度別パラメータの反映確認
   - 位置・高度による風力変化の検証

2. **物理パラメータテスト**
   - 重力加速度の正確性
   - 空気抵抗の効果確認
   - 速度制限の動作確認

3. **移動コンポーネントテスト**
   - 入力レスポンスの確認
   - 物理演算の統合動作
   - 異常値の処理確認

### 統合テスト
1. **キャラクター連携テスト**
   - ABirdCharacterとの統合動作
   - 入力システムとの連携
   - アニメーションシステムとの協調

2. **パフォーマンステスト**
   - 風力計算の処理時間測定
   - メモリ使用量の監視
   - フレームレート安定性確認

## 受け入れ基準

### 機能面
- [ ] 鳥が重力に従って自然に落下する
- [ ] 風の影響で鳥が横方向に適切に流される
- [ ] プレイヤー入力で方向制御が可能
- [ ] 難易度別の風力設定が正しく反映される
- [ ] 最大速度制限が機能する

### 技術面
- [ ] 全C++クラスがコンパイル成功
- [ ] Blueprintでパラメータ調整可能
- [ ] 単体テストが全て通過
- [ ] パフォーマンス要件を満たす

### ユーザビリティ面
- [ ] 制御しやすい飛行感覚
- [ ] 予測可能な物理挙動
- [ ] 適度な挑戦レベル

## エラーハンドリング

### 異常値対応
- 風力が異常値の場合は最大値で制限
- 速度が負値の場合は0にリセット
- NaN/Inf値の検出と補正

### パフォーマンス劣化時
- 風力計算の簡略化
- 更新頻度の動的調整
- エラーログの出力

## 依存関係

### 前提条件
- TASK-102完了：データ構造・列挙型実装
- ABirdCharacter基底クラス存在
- UE5.6のChaos Physics利用可能

### 後続タスクへの影響
- TASK-201: 鳥キャラクター実装（物理システム連携）
- TASK-401: ControlRig実装（物理連動アニメーション）
- TASK-701: パフォーマンス最適化（物理計算最適化）