# TASK-201: 鳥キャラクター実装 - 要件定義

## 概要

プレイヤーが操作する鳥キャラクターの完全実装を行います。物理システム（TASK-103）と統合し、直感的で制御しやすい飛行体験を提供する高品質なキャラクターシステムを構築します。

## 機能要件

### REQ-001: 基本飛行制御
- プレイヤー入力による鳥の方向制御
- マウス/キーボード/ゲームパッドでの操作対応
- スムーズで予測可能な飛行挙動
- 物理システムとの完全統合

### REQ-005: 入力システム統合
- UE5.6 Enhanced Input Systemの活用
- 複数入力デバイスの同時対応
- 入力感度の調整機能
- デッドゾーン処理の実装

### REQ-105: 飛行状態管理
- 飛行状態の適切な管理（Idle, Flying, Gliding, Landing等）
- 状態遷移の滑らかな処理
- アニメーション連携の準備
- デバッグ用状態表示

## 実装詳細

### 1. ABirdCharacter クラス拡張
```cpp
class BIRDDIVE_API ABirdCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Enhanced Input System integration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* FlyAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* LookAction;

    // Flight state management
    UFUNCTION(BlueprintCallable, Category = "Flight")
    EFlightState GetCurrentFlightState() const;

    UFUNCTION(BlueprintCallable, Category = "Flight")
    void SetFlightState(EFlightState NewState);

    // Landing detection
    UFUNCTION(BlueprintCallable, Category = "Flight")
    bool IsLanding() const;

    UFUNCTION(BlueprintCallable, Category = "Flight")
    FLandingResult PerformLandingCheck();

    // Speed monitoring
    UFUNCTION(BlueprintCallable, Category = "Flight")
    float GetCurrentSpeed() const;

    UFUNCTION(BlueprintCallable, Category = "Flight")
    bool IsOverSpeedLimit() const;

protected:
    // Flight state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
    EFlightState CurrentFlightState = EFlightState::Idle;

    // Physics integration
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
    class UWindForceComponent* WindForceComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
    class UBirdCharacterMovementComponent* BirdMovementComponent;

    // Input handling
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    void HandleFlyInput(const struct FInputActionValue& Value);
    void HandleLookInput(const struct FInputActionValue& Value);

    // Flight parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Parameters")
    float FlyForceMultiplier = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Parameters")
    float TurnSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Parameters")
    float MaxSafeSpeed = 2000.0f;
};
```

### 2. 飛行状態管理システム
```cpp
class BIRDDIVE_API UFlightStateManager : public UActorComponent
{
    GENERATED_BODY()

public:
    // State transitions
    UFUNCTION(BlueprintCallable)
    bool TransitionToState(EFlightState NewState);

    // State validation
    UFUNCTION(BlueprintCallable)
    bool CanTransitionTo(EFlightState TargetState) const;

    // State events
    UPROPERTY(BlueprintAssignable)
    FOnFlightStateChanged OnFlightStateChanged;

protected:
    // State machine logic
    bool ValidateStateTransition(EFlightState From, EFlightState To) const;
    void ProcessStateEntry(EFlightState State);
    void ProcessStateExit(EFlightState State);

private:
    UPROPERTY()
    EFlightState CurrentState = EFlightState::Idle;

    UPROPERTY()
    EFlightState PreviousState = EFlightState::Idle;
};
```

### 3. 着地判定システム
```cpp
class BIRDDIVE_API ULandingDetectionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Landing detection
    UFUNCTION(BlueprintCallable)
    FLandingResult CheckLanding();

    UFUNCTION(BlueprintCallable)
    bool IsNearGround(float& OutDistance) const;

    // Landing events
    UPROPERTY(BlueprintAssignable)
    FOnLandingDetected OnLandingDetected;

protected:
    // Detection parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing Detection")
    float LandingTraceDistance = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landing Detection")
    float LandingSpeedThreshold = 100.0f;

    // Landing quality calculation
    ELandingQuality CalculateLandingQuality(const FHitResult& HitResult, float LandingSpeed) const;
    int32 CalculateBaseScore(ELandingQuality Quality, float Accuracy) const;
};
```

## 非機能要件

### パフォーマンス
- Tick処理の最適化（60FPS維持）
- 入力レスポンス時間 < 16ms
- 状態遷移処理時間 < 1ms

### ユーザビリティ
- 直感的で学習しやすい操作感
- 操作設定のカスタマイズ対応
- 視覚的フィードバックの充実

### 拡張性
- アニメーションシステムとの連携準備
- UIシステムとの統合対応
- マルチプレイヤー対応の基盤

## テスト要件

### 単体テスト
1. **入力レスポンステスト**
   - Enhanced Input Actionの正常動作
   - 複数デバイスからの入力処理
   - 入力感度の調整確認

2. **飛行状態管理テスト**
   - 状態遷移の妥当性確認
   - 不正な状態遷移の拒否
   - 状態イベントの適切な発火

3. **着地判定精度テスト**
   - 地面検出の正確性
   - 着地品質の計算精度
   - スコア計算の妥当性

### 統合テスト
1. **物理システム連携テスト**
   - WindForceComponentとの統合
   - BirdCharacterMovementComponentとの協調
   - 物理パラメータの適切な反映

2. **入力システム統合テスト**
   - Enhanced Input Systemとの完全統合
   - 入力からキャラクター動作までの一貫性
   - 複数入力デバイスの同時動作

### E2Eテスト
1. **基本飛行操作テスト**
   - 離陸から着地までの完全フロー
   - プレイヤー操作に対する適切な応答
   - 物理挙動の自然さ

## 受け入れ基準

### 機能面
- [ ] プレイヤー入力で鳥が直感的に制御可能
- [ ] 飛行状態が適切に管理・表示される
- [ ] 着地時に正確な判定と評価が行われる
- [ ] 速度制限が適切に機能する
- [ ] 物理システムと完全に統合されている

### 技術面
- [ ] 全C++クラスがコンパイル成功
- [ ] Enhanced Input Systemが正しく統合
- [ ] 単体テストが全て通過
- [ ] パフォーマンス要件を満たす
- [ ] Blueprint公開が適切に行われている

### ユーザビリティ面
- [ ] 操作が直感的で学習しやすい
- [ ] レスポンスが滑らかで遅延がない
- [ ] 視覚的フィードバックが適切
- [ ] デバッグ情報が確認可能

## エラーハンドリング

### 入力エラー対応
- 不正な入力値の検出と補正
- 入力デバイス切断時の適切な処理
- Enhanced Input System初期化失敗時の対応

### 物理エラー対応
- 異常な物理値の検出と補正
- 地面貫通の防止機能
- 速度異常時の安全停止

### 状態エラー対応
- 不正な状態遷移の防止
- 状態不整合の自動修復
- デッドロック状態の回避

## 依存関係

### 前提条件
- TASK-103完了：物理システム基盤
- UE5.6 Enhanced Input System
- BirdDiveTypes.h（EFlightState, FLandingResult等）

### 後続タスクへの影響
- TASK-301: 分割スクリーンシステム（カメラ追従対象）
- TASK-401: ControlRigシステム（アニメーション連携）
- TASK-501: UI実装（キャラクター情報表示）

## 実装フェーズ

### Phase 1: 基本構造
1. ABirdCharacter基底クラスの拡張
2. UFlightStateManagerの実装
3. Enhanced Input System統合

### Phase 2: 物理統合
1. 物理システムコンポーネントの統合
2. 飛行制御ロジックの実装
3. パフォーマンス最適化

### Phase 3: 着地システム
1. ULandingDetectionComponentの実装
2. 着地品質評価システム
3. スコア計算ロジック

### Phase 4: 品質向上
1. エラーハンドリングの実装
2. デバッグ機能の充実
3. Blueprint統合の完成

## 成功指標

### 定量的指標
- 入力レスポンス時間: < 16ms
- フレームレート: 60FPS維持
- 着地判定精度: 95%以上
- テストカバレッジ: 90%以上

### 定性的指標
- 操作の自然さ
- 学習の容易さ
- デバッグの効率性
- コードの保守性

この要件定義に基づき、プレイヤーが直感的に操作でき、技術的に堅牢な鳥キャラクターシステムを実装します。