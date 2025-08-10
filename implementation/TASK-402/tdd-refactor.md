# TASK-402: アニメーションブループリント実装 - REFACTOR段階

## 目的

GREEN段階で実装した最小限のコードを改善し、保守性、パフォーマンス、可読性を向上させる。

## リファクタリングの原則

1. 機能を変更せずに内部構造を改善
2. テストが通り続けることを確認
3. コードの重複を除去
4. 可読性と保守性を向上
5. パフォーマンスの最適化

## 現在のコード分析

### 改善すべき点

1. **状態遷移ロジックの複雑さ** - UpdateStateMachine内の条件分岐が複雑
2. **Magic Number の多用** - 速度閾値等がハードコーディングされている
3. **エラーハンドリングの一貫性不足** - 様々な場所でのnullチェックが統一されていない
4. **パフォーマンス最適化の余地** - 不要な計算の削除、キャッシュ活用
5. **コメントの不足** - 複雑なロジックの説明が不足

## リファクタリング計画

### 1. 定数の外部化

現在ハードコーディングされている値を設定可能にする：

```cpp
// Header file (BirdAnimInstance.h)
public:
    // Configuration constants
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float FlightStartSpeedThreshold = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float ClimbingSpeedThreshold = 1000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float ClimbingVerticalVelocityThreshold = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float DivingVerticalVelocityThreshold = -200.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float LandingSpeedThreshold = 200.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float LandingDistanceThreshold = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Space Config")
    float SlowFlightSpeedThreshold = 500.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Space Config")
    float MediumFlightSpeedThreshold = 1000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Config")
    int32 FeatherCount = 10;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Config")
    float WindInfluenceScale = 1000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Config")
    float DirectionalBlendScale = 0.8f;
```

### 2. 状態遷移ロジックの構造化

複雑な条件判定をメソッドに分割：

```cpp
// Private helper methods
private:
    // State transition condition checkers
    bool ShouldTransitionToFlying() const;
    bool ShouldTransitionToClimbing() const;
    bool ShouldTransitionToDiving() const;
    bool ShouldTransitionToLanding() const;
    bool ShouldTransitionToIdle() const;
    
    // State transition handlers
    void HandleIdleState();
    void HandleFlyingState();
    void HandleLandingState();
    void HandleClimbingState();
    void HandleDivingState();
    void HandleGlidingState();
```

### 3. エラーハンドリングの統一

一貫したエラーハンドリング機能の実装：

```cpp
private:
    // Error handling utilities
    bool ValidateRequiredReferences() const;
    void LogWarning(const FString& Message) const;
    void LogError(const FString& Message) const;
    void SetLastError(const FString& Error);
    bool IsInitializedProperly() const;
```

### 4. パフォーマンス最適化

計算結果のキャッシュとフレーム間での最適化：

```cpp
private:
    // Performance optimization
    mutable bool bCachedBlendWeightsValid = false;
    mutable TArray<float> CachedBlendWeights;
    
    // Frame optimization
    int32 LastUpdateFrame = -1;
    float LastDeltaTime = 0.0f;
    
    // Computation caching
    void InvalidateBlendWeightCache();
    const TArray<float>& GetCachedBlendWeights() const;
    bool ShouldSkipUpdate(float DeltaTime) const;
```

### 5. コメントと文書化の改善

各メソッドに適切なコメントを追加：

```cpp
/**
 * Updates the state machine based on current flight parameters
 * Handles automatic transitions between flight states based on:
 * - Speed thresholds
 * - Vertical velocity
 * - Flight state flags
 * - Distance to ground
 * 
 * @param DeltaTime Time elapsed since last update
 */
void UpdateStateMachine(float DeltaTime);

/**
 * Calculates blend space input for flight locomotion animations
 * 
 * @return FVector with X=Speed, Y=VerticalVelocity, Z=0
 */
FVector GetFlightBlendSpaceInput() const;
```

## 具体的な実装改善

### リファクタリング後の主要メソッド

#### 1. UpdateStateMachine の改善

```cpp
void UBirdAnimInstance::UpdateStateMachine(float DeltaTime)
{
    if (!ValidateRequiredReferences())
    {
        LogError(TEXT("Failed to validate required references for state machine update"));
        return;
    }
    
    const FString PreviousState = CurrentStateName;
    FString NewState = CurrentStateName;
    
    // State-specific transition handling
    if (CurrentStateName == TEXT("Idle"))
        HandleIdleState();
    else if (CurrentStateName == TEXT("Flying"))
        HandleFlyingState();
    else if (CurrentStateName == TEXT("Landing"))
        HandleLandingState();
    else if (CurrentStateName == TEXT("Climbing"))
        HandleClimbingState();
    else if (CurrentStateName == TEXT("Diving"))
        HandleDivingState();
    else if (CurrentStateName == TEXT("Gliding"))
        HandleGlidingState();
    
    // Log state changes for debugging
    if (CurrentStateName != PreviousState)
    {
        UE_LOG(LogAnimation, Log, TEXT("State transition: %s -> %s"), *PreviousState, *CurrentStateName);
    }
}
```

#### 2. 条件チェックメソッドの実装

```cpp
bool UBirdAnimInstance::ShouldTransitionToFlying() const
{
    return bIsFlying && Speed > FlightStartSpeedThreshold;
}

bool UBirdAnimInstance::ShouldTransitionToClimbing() const
{
    return CurrentStateName == TEXT("Flying") && 
           VerticalVelocity > ClimbingVerticalVelocityThreshold && 
           Speed > ClimbingSpeedThreshold;
}

bool UBirdAnimInstance::ShouldTransitionToDiving() const
{
    return (CurrentStateName == TEXT("Flying") || CurrentStateName == TEXT("Climbing")) &&
           VerticalVelocity < DivingVerticalVelocityThreshold;
}

bool UBirdAnimInstance::ShouldTransitionToLanding() const
{
    return (CurrentStateName == TEXT("Flying") || CurrentStateName == TEXT("Diving")) &&
           bIsLanding && 
           Speed < LandingSpeedThreshold && 
           DistanceToGround < LandingDistanceThreshold;
}

bool UBirdAnimInstance::ShouldTransitionToIdle() const
{
    return CurrentStateName == TEXT("Landing") && 
           !bIsFlying && 
           DistanceToGround <= 0.0f;
}
```

#### 3. ブレンドウェイト計算の最適化

```cpp
const TArray<float>& UBirdAnimInstance::GetCachedBlendWeights() const
{
    if (!bCachedBlendWeightsValid)
    {
        CachedBlendWeights.Empty();
        CachedBlendWeights.AddZeroed(4);
        
        // Determine which weight to set based on speed
        if (Speed <= 0.0f)
            CachedBlendWeights[0] = 1.0f; // Idle
        else if (Speed <= SlowFlightSpeedThreshold)
            CachedBlendWeights[1] = 1.0f; // Slow flight
        else if (Speed <= MediumFlightSpeedThreshold)
            CachedBlendWeights[2] = 1.0f; // Medium flight
        else
            CachedBlendWeights[3] = 1.0f; // Fast flight
        
        bCachedBlendWeightsValid = true;
    }
    
    return CachedBlendWeights;
}

TArray<float> UBirdAnimInstance::GetFlightBlendWeights() const
{
    return GetCachedBlendWeights();
}
```

#### 4. エラーハンドリングの統一

```cpp
bool UBirdAnimInstance::ValidateRequiredReferences() const
{
    if (!CachedBirdCharacter)
    {
        LogError(TEXT("CachedBirdCharacter is null"));
        return false;
    }
    
    return true;
}

void UBirdAnimInstance::LogError(const FString& Message) const
{
    UE_LOG(LogAnimation, Error, TEXT("BirdAnimInstance: %s"), *Message);
    const_cast<UBirdAnimInstance*>(this)->SetLastError(Message);
}

void UBirdAnimInstance::LogWarning(const FString& Message) const
{
    UE_LOG(LogAnimation, Warning, TEXT("BirdAnimInstance: %s"), *Message);
}

void UBirdAnimInstance::SetLastError(const FString& Error)
{
    LastError = Error;
}
```

## パフォーマンス改善点

### 1. フレームスキップ最適化

```cpp
bool UBirdAnimInstance::ShouldSkipUpdate(float DeltaTime) const
{
    // Skip update if delta time is too small
    const float MinDeltaTime = 1.0f / 120.0f; // 120 FPS threshold
    return DeltaTime < MinDeltaTime;
}

void UBirdAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);
    
    if (ShouldSkipUpdate(DeltaTimeX))
        return;
    
    // Invalidate caches that depend on updated values
    InvalidateBlendWeightCache();
    
    // Perform updates
    UpdateMovementParameters();
    UpdateFlightStateParameters();
    UpdateInputParameters();
    UpdateControlRigParameters();
    UpdateBlendSpaceParameters();
    
    LastDeltaTime = DeltaTimeX;
}
```

### 2. メモリ最適化

```cpp
void UBirdAnimInstance::UpdatePhysicsAnimation(float DeltaTime)
{
    // Validate references first
    if (!ValidateRequiredReferences())
        return;
    
    // Update ControlRig parameters
    if (CachedControlRigComponent)
    {
        const FBirdControlRigParams& ControlParams = CachedControlRigComponent->GetCurrentParameters();
        FlappingFrequency = ControlParams.FlappingFrequency;
        WingAngle = ControlParams.WingAngle;
    }
    
    // Update feather physics weights (reserve capacity)
    CachedFeatherPhysicsWeights.Empty(FeatherCount);
    CachedFeatherPhysicsWeights.Reserve(FeatherCount);
    
    const float WindMagnitude = WindInfluence.Size();
    const float NormalizedWeight = FMath::Clamp(WindMagnitude / WindInfluenceScale, 0.0f, 1.0f);
    
    for (int32 i = 0; i < FeatherCount; i++)
    {
        CachedFeatherPhysicsWeights.Add(NormalizedWeight);
    }
    
    // Update wing bone transform
    UpdateWingBoneTransform(WindMagnitude);
}
```

## コード品質改善

### 1. Magic Number の削除
- すべてのハードコーディングされた数値を設定プロパティに変更
- Blueprint で調整可能にすることで設計者が容易に調整可能

### 2. メソッドの責任分離
- 大きなメソッドを複数の小さなメソッドに分割
- 各メソッドが単一の責任を持つように設計

### 3. エラーハンドリングの統一
- 一貫したロギング方式の採用
- エラー状態の適切な処理とリカバリー

### 4. パフォーマンス最適化
- 不要な計算の削除
- 結果のキャッシュ活用
- メモリ使用量の最適化

## テストとの整合性確保

リファクタリング後も全てのテストが通ることを確認：

1. 既存のテストを実行
2. 新しい機能に対するテスト追加
3. パフォーマンステスト実行
4. メモリリークテスト実行

## 完了基準

### 機能面
- [ ] 全ての既存テストがパス
- [ ] 新機能が正常に動作
- [ ] エラーハンドリングが適切に動作

### 品質面
- [ ] コード重複の除去
- [ ] 可読性の向上
- [ ] コメント・文書化の完備

### パフォーマンス面
- [ ] 処理時間の改善確認
- [ ] メモリ使用量の最適化確認
- [ ] フレームレート安定性の確認

このリファクタリングにより、コードの品質、保守性、パフォーマンスが大幅に向上し、将来の機能拡張に対してより柔軟な基盤が構築される。