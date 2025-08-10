# TASK-402: アニメーションブループリント実装 - テストケース仕様

## テスト戦略概要

Animation Blueprint（ABP_Bird）システムの実装に対して、TDDアプローチに基づく包括的なテストスイートを構築する。

## 単体テスト仕様

### UT-402-001: BirdAnimInstance基本機能テスト

#### UT-402-001-A: AnimInstance初期化テスト
```cpp
TEST_F(BirdAnimInstanceTest, AnimInstanceInitialization)
{
    // Given: 新しいBirdAnimInstance
    UBirdAnimInstance* AnimInstance = CreateAnimInstance();
    
    // When: AnimInstanceを初期化
    AnimInstance->NativeInitializeAnimation();
    
    // Then: 正常に初期化される
    EXPECT_TRUE(AnimInstance->IsValidLowLevel());
    EXPECT_NE(AnimInstance->GetOwningActor(), nullptr);
    EXPECT_NE(AnimInstance->GetSkelMeshComponent(), nullptr);
}
```

#### UT-402-001-B: パラメータ初期値テスト
```cpp
TEST_F(BirdAnimInstanceTest, DefaultParameterValues)
{
    // Given: 初期化済みAnimInstance
    UBirdAnimInstance* AnimInstance = CreateInitializedAnimInstance();
    
    // When: 初期パラメータを確認
    // Then: デフォルト値が設定されている
    EXPECT_FLOAT_EQ(AnimInstance->GetSpeed(), 0.0f);
    EXPECT_FLOAT_EQ(AnimInstance->GetVerticalVelocity(), 0.0f);
    EXPECT_FALSE(AnimInstance->GetIsFlying());
    EXPECT_FALSE(AnimInstance->GetIsLanding());
    EXPECT_EQ(AnimInstance->GetControlRigComponent(), nullptr);
}
```

#### UT-402-001-C: ControlRigComponent取得テスト
```cpp
TEST_F(BirdAnimInstanceTest, ControlRigComponentAccess)
{
    // Given: ControlRigComponentを持つBirdCharacter
    ABirdCharacter* BirdCharacter = CreateBirdCharacterWithControlRig();
    UBirdAnimInstance* AnimInstance = Cast<UBirdAnimInstance>(BirdCharacter->GetMesh()->GetAnimInstance());
    
    // When: ControlRigComponentを取得
    UBirdControlRigComponent* ControlRigComponent = AnimInstance->GetControlRigComponent();
    
    // Then: 有効なコンポーネントが取得される
    EXPECT_NE(ControlRigComponent, nullptr);
    EXPECT_EQ(ControlRigComponent, BirdCharacter->GetBirdControlRigComponent());
}
```

### UT-402-002: パラメータ更新テスト

#### UT-402-002-A: 速度パラメータ更新テスト
```cpp
TEST_F(BirdAnimInstanceTest, SpeedParameterUpdate)
{
    // Given: BirdCharacterとAnimInstance
    ABirdCharacter* BirdCharacter = CreateBirdCharacterWithAnimBP();
    UBirdAnimInstance* AnimInstance = GetAnimInstance(BirdCharacter);
    
    // When: キャラクターの速度を設定
    FVector Velocity = FVector(1000.0f, 500.0f, 200.0f);
    BirdCharacter->GetCharacterMovement()->Velocity = Velocity;
    
    // AnimInstance更新
    AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
    
    // Then: 速度パラメータが更新される
    float ExpectedSpeed = Velocity.Size();
    EXPECT_FLOAT_EQ(AnimInstance->GetSpeed(), ExpectedSpeed);
    EXPECT_FLOAT_EQ(AnimInstance->GetVerticalVelocity(), Velocity.Z);
}
```

#### UT-402-002-B: 飛行状態パラメータ更新テスト
```cpp
TEST_F(BirdAnimInstanceTest, FlightStateParameterUpdate)
{
    // Given: BirdCharacterとAnimInstance
    ABirdCharacter* BirdCharacter = CreateBirdCharacterWithAnimBP();
    UBirdAnimInstance* AnimInstance = GetAnimInstance(BirdCharacter);
    
    // When: 飛行状態を設定
    BirdCharacter->SetFlightState(EFlightState::Flying);
    AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
    
    // Then: 飛行フラグが更新される
    EXPECT_TRUE(AnimInstance->GetIsFlying());
    
    // When: 着地状態を設定
    BirdCharacter->SetFlightState(EFlightState::Landing);
    AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
    
    // Then: 着地フラグが更新される
    EXPECT_TRUE(AnimInstance->GetIsLanding());
    EXPECT_FALSE(AnimInstance->GetIsFlying());
}
```

#### UT-402-002-C: 入力パラメータ更新テスト
```cpp
TEST_F(BirdAnimInstanceTest, InputParameterUpdate)
{
    // Given: BirdCharacterとAnimInstance
    ABirdCharacter* BirdCharacter = CreateBirdCharacterWithAnimBP();
    UBirdAnimInstance* AnimInstance = GetAnimInstance(BirdCharacter);
    
    // When: 入力値を設定
    FVector2D InputVector = FVector2D(0.5f, -0.8f);
    BirdCharacter->SetFlightInput(InputVector);
    AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
    
    // Then: 入力パラメータが更新される
    EXPECT_FLOAT_EQ(AnimInstance->GetInputX(), InputVector.X);
    EXPECT_FLOAT_EQ(AnimInstance->GetInputY(), InputVector.Y);
}
```

### UT-402-003: 状態マシンテスト

#### UT-402-003-A: 基本状態遷移テスト
```cpp
TEST_F(BirdAnimInstanceTest, BasicStateTransitions)
{
    // Given: AnimInstanceと状態マシン
    UBirdAnimInstance* AnimInstance = CreateAnimInstanceWithStateMachine();
    
    // When: Idle状態から開始
    AnimInstance->SetCurrentState("Idle");
    
    // 飛行状態への遷移条件を満たす
    AnimInstance->SetSpeed(500.0f);
    AnimInstance->SetIsFlying(true);
    AnimInstance->UpdateStateMachine(1.0f / 60.0f);
    
    // Then: Flying状態に遷移
    FString CurrentState = AnimInstance->GetCurrentStateName();
    EXPECT_EQ(CurrentState, "Flying");
}
```

#### UT-402-003-B: 速度ベース遷移テスト
```cpp
TEST_F(BirdAnimInstanceTest, SpeedBasedTransitions)
{
    // Given: Flying状態のAnimInstance
    UBirdAnimInstance* AnimInstance = CreateAnimInstanceInFlyingState();
    
    // When: 高速飛行条件を満たす
    AnimInstance->SetSpeed(1500.0f);
    AnimInstance->SetVerticalVelocity(200.0f);
    AnimInstance->UpdateStateMachine(1.0f / 60.0f);
    
    // Then: Climbing状態に遷移
    FString CurrentState = AnimInstance->GetCurrentStateName();
    EXPECT_EQ(CurrentState, "Climbing");
    
    // When: 降下条件を満たす
    AnimInstance->SetVerticalVelocity(-300.0f);
    AnimInstance->UpdateStateMachine(1.0f / 60.0f);
    
    // Then: Diving状態に遷移
    CurrentState = AnimInstance->GetCurrentStateName();
    EXPECT_EQ(CurrentState, "Diving");
}
```

#### UT-402-003-C: 着地遷移テスト
```cpp
TEST_F(BirdAnimInstanceTest, LandingTransitions)
{
    // Given: Flying状態のAnimInstance
    UBirdAnimInstance* AnimInstance = CreateAnimInstanceInFlyingState();
    
    // When: 着地条件を満たす
    AnimInstance->SetSpeed(100.0f);
    AnimInstance->SetIsLanding(true);
    AnimInstance->SetDistanceToGround(50.0f);
    AnimInstance->UpdateStateMachine(1.0f / 60.0f);
    
    // Then: Landing状態に遷移
    FString CurrentState = AnimInstance->GetCurrentStateName();
    EXPECT_EQ(CurrentState, "Landing");
    
    // When: 着地完了条件を満たす
    AnimInstance->SetDistanceToGround(0.0f);
    AnimInstance->SetIsFlying(false);
    AnimInstance->UpdateStateMachine(1.0f / 60.0f);
    
    // Then: Idle状態に遷移
    CurrentState = AnimInstance->GetCurrentStateName();
    EXPECT_EQ(CurrentState, "Idle");
}
```

### UT-402-004: ブレンドスペーステスト

#### UT-402-004-A: 飛行ブレンドスペーステスト
```cpp
TEST_F(BirdAnimInstanceTest, FlightBlendSpaceTest)
{
    // Given: ブレンドスペースを持つAnimInstance
    UBirdAnimInstance* AnimInstance = CreateAnimInstanceWithBlendSpace();
    
    // When: 異なる速度・垂直速度を設定
    AnimInstance->SetSpeed(1000.0f);
    AnimInstance->SetVerticalVelocity(500.0f);
    
    // ブレンドスペース評価を実行
    FVector BlendInput = AnimInstance->GetFlightBlendSpaceInput();
    
    // Then: 適切なブレンド入力値が計算される
    EXPECT_FLOAT_EQ(BlendInput.X, 1000.0f); // Speed
    EXPECT_FLOAT_EQ(BlendInput.Y, 500.0f);  // VerticalVelocity
    
    // ブレンドウェイトが適切に計算される
    TArray<float> BlendWeights = AnimInstance->GetFlightBlendWeights();
    float TotalWeight = 0.0f;
    for (float Weight : BlendWeights)
    {
        TotalWeight += Weight;
        EXPECT_GE(Weight, 0.0f);
        EXPECT_LE(Weight, 1.0f);
    }
    EXPECT_FLOAT_EQ(TotalWeight, 1.0f);
}
```

#### UT-402-004-B: 方向制御ブレンドスペーステスト
```cpp
TEST_F(BirdAnimInstanceTest, DirectionalBlendSpaceTest)
{
    // Given: 方向制御ブレンドスペースを持つAnimInstance
    UBirdAnimInstance* AnimInstance = CreateAnimInstanceWithDirectionalBlendSpace();
    
    // When: 入力方向を設定
    AnimInstance->SetInputX(0.7f);
    AnimInstance->SetInputY(-0.3f);
    
    // ブレンドスペース評価を実行
    FVector2D BlendInput = AnimInstance->GetDirectionalBlendSpaceInput();
    
    // Then: 適切なブレンド入力値が計算される
    EXPECT_FLOAT_EQ(BlendInput.X, 0.7f);
    EXPECT_FLOAT_EQ(BlendInput.Y, -0.3f);
    
    // 旋回アニメーションが適用される
    float TurnWeight = AnimInstance->GetTurnAnimationWeight();
    EXPECT_GT(TurnWeight, 0.0f);
}
```

#### UT-402-004-C: ブレンドスペース補間テスト
```cpp
TEST_F(BirdAnimInstanceTest, BlendSpaceInterpolationTest)
{
    // Given: ブレンドスペースAnimInstance
    UBirdAnimInstance* AnimInstance = CreateAnimInstanceWithBlendSpace();
    
    // When: 速度を段階的に変更
    TArray<float> TestSpeeds = {0.0f, 500.0f, 1000.0f, 1500.0f, 2000.0f};
    
    for (float Speed : TestSpeeds)
    {
        AnimInstance->SetSpeed(Speed);
        AnimInstance->SetVerticalVelocity(0.0f);
        
        // ブレンドウェイトを取得
        TArray<float> BlendWeights = AnimInstance->GetFlightBlendWeights();
        
        // Then: 滑らかな補間が行われる
        EXPECT_GT(BlendWeights.Num(), 0);
        
        // 速度に応じた適切なブレンドが行われる
        if (Speed == 0.0f)
        {
            EXPECT_GT(BlendWeights[0], 0.8f); // Idle animation
        }
        else if (Speed >= 2000.0f)
        {
            EXPECT_GT(BlendWeights[BlendWeights.Num() - 1], 0.8f); // Fast flight animation
        }
    }
}
```

### UT-402-005: 物理アニメーション統合テスト

#### UT-402-005-A: ControlRig連携テスト
```cpp
TEST_F(BirdAnimInstanceTest, ControlRigIntegration)
{
    // Given: ControlRigとAnimInstanceを持つBirdCharacter
    ABirdCharacter* BirdCharacter = CreateFullyIntegratedBirdCharacter();
    UBirdAnimInstance* AnimInstance = GetAnimInstance(BirdCharacter);
    UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
    
    // When: ControlRigパラメータを設定
    FBirdControlRigParams ControlRigParams;
    ControlRigParams.FlappingFrequency = 2.5f;
    ControlRigParams.WingAngle = 30.0f;
    ControlRigComponent->SetControlParameters(ControlRigParams);
    
    // AnimInstance更新
    AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
    
    // Then: AnimInstanceがControlRigパラメータを取得
    float FlappingFrequency = AnimInstance->GetFlappingFrequency();
    float WingAngle = AnimInstance->GetWingAngle();
    
    EXPECT_FLOAT_EQ(FlappingFrequency, 2.5f);
    EXPECT_FLOAT_EQ(WingAngle, 30.0f);
}
```

#### UT-402-005-B: 物理アニメーション適用テスト
```cpp
TEST_F(BirdAnimInstanceTest, PhysicsAnimationApplication)
{
    // Given: 物理アニメーション対応AnimInstance
    UBirdAnimInstance* AnimInstance = CreatePhysicsEnabledAnimInstance();
    
    // When: 風力パラメータを設定
    FVector WindInfluence = FVector(100.0f, 50.0f, 0.0f);
    AnimInstance->SetWindInfluence(WindInfluence);
    AnimInstance->SetPhysicsBlendWeight(0.7f);
    
    // 物理アニメーションを更新
    AnimInstance->UpdatePhysicsAnimation(1.0f / 60.0f);
    
    // Then: 風力が羽根アニメーションに影響
    TArray<float> FeatherPhysicsWeights = AnimInstance->GetFeatherPhysicsWeights();
    for (float Weight : FeatherPhysicsWeights)
    {
        EXPECT_GT(Weight, 0.0f);
        EXPECT_LE(Weight, 1.0f);
    }
    
    // 物理ブレンドが適用される
    float PhysicsBlendWeight = AnimInstance->GetPhysicsBlendWeight();
    EXPECT_FLOAT_EQ(PhysicsBlendWeight, 0.7f);
}
```

#### UT-402-005-C: リアルタイム物理応答テスト
```cpp
TEST_F(BirdAnimInstanceTest, RealtimePhysicsResponse)
{
    // Given: リアルタイム物理対応AnimInstance
    UBirdAnimInstance* AnimInstance = CreateRealtimePhysicsAnimInstance();
    
    // When: 突発的な風力変化を適用
    FVector SuddenWind = FVector(1000.0f, 0.0f, 0.0f);
    AnimInstance->ApplySuddenWindForce(SuddenWind);
    
    // 複数フレームで応答を観察
    TArray<FTransform> WingTransforms;
    for (int32 Frame = 0; Frame < 10; Frame++)
    {
        AnimInstance->UpdatePhysicsAnimation(1.0f / 60.0f);
        WingTransforms.Add(AnimInstance->GetWingBoneTransform());
    }
    
    // Then: 段階的な応答が確認される
    for (int32 i = 1; i < WingTransforms.Num(); i++)
    {
        // 前フレームと比較して変化があることを確認
        EXPECT_NE(WingTransforms[i].GetRotation(), WingTransforms[i-1].GetRotation());
        
        // 過度な変化でないことを確認（安定性）
        float AngularDifference = FQuat::Error(WingTransforms[i].GetRotation(), WingTransforms[i-1].GetRotation());
        EXPECT_LT(AngularDifference, FMath::DegreesToRadians(10.0f));
    }
}
```

## 統合テスト仕様

### IT-402-001: BirdCharacter統合テスト

#### IT-402-001-A: 完全統合テスト
```cpp
TEST_F(BirdCharacterIntegrationTest, FullAnimationSystemIntegration)
{
    // Given: 完全に設定されたBirdCharacter
    ABirdCharacter* BirdCharacter = CreateFullyConfiguredBirdCharacter();
    UBirdAnimInstance* AnimInstance = GetAnimInstance(BirdCharacter);
    
    // When: 複雑な飛行シーケンスを実行
    // 離陸
    BirdCharacter->SetFlightState(EFlightState::Flying);
    BirdCharacter->GetCharacterMovement()->Velocity = FVector(500.0f, 0.0f, 300.0f);
    Tick(1.0f);
    
    EXPECT_EQ(AnimInstance->GetCurrentStateName(), "Climbing");
    
    // 巡航飛行
    BirdCharacter->GetCharacterMovement()->Velocity = FVector(1000.0f, 0.0f, 0.0f);
    Tick(2.0f);
    
    EXPECT_EQ(AnimInstance->GetCurrentStateName(), "Flying");
    
    // 旋回
    BirdCharacter->SetFlightInput(FVector2D(0.8f, 0.0f));
    Tick(1.0f);
    
    EXPECT_GT(AnimInstance->GetTurnAnimationWeight(), 0.5f);
    
    // 着地
    BirdCharacter->SetFlightState(EFlightState::Landing);
    BirdCharacter->GetCharacterMovement()->Velocity = FVector(200.0f, 0.0f, -100.0f);
    Tick(3.0f);
    
    EXPECT_EQ(AnimInstance->GetCurrentStateName(), "Idle");
}
```

#### IT-402-001-B: 入力応答統合テスト
```cpp
TEST_F(BirdCharacterIntegrationTest, InputResponseIntegration)
{
    // Given: プレイヤー制御可能なBirdCharacter
    ABirdCharacter* BirdCharacter = CreatePlayerControlledBirdCharacter();
    UBirdAnimInstance* AnimInstance = GetAnimInstance(BirdCharacter);
    ABirdDivePlayerController* PlayerController = Cast<ABirdDivePlayerController>(BirdCharacter->GetController());
    
    // When: プレイヤー入力をシミュレート
    PlayerController->AddPitchInput(0.5f);
    PlayerController->AddYawInput(-0.3f);
    PlayerController->AddRollInput(0.8f);
    
    // システム更新
    Tick(0.1f);
    
    // Then: 入力がアニメーションに反映される
    EXPECT_GT(AnimInstance->GetInputX(), 0.0f);
    EXPECT_LT(AnimInstance->GetInputY(), 0.0f);
    EXPECT_GT(AnimInstance->GetRollInput(), 0.0f);
    
    // 対応するアニメーションブレンドが実行される
    float DirectionalBlendWeight = AnimInstance->GetDirectionalBlendWeight();
    EXPECT_GT(DirectionalBlendWeight, 0.0f);
}
```

### IT-402-002: 物理システム統合テスト

#### IT-402-002-A: 風力システム統合テスト
```cpp
TEST_F(PhysicsSystemIntegrationTest, WindSystemIntegration)
{
    // Given: 風力環境のBirdCharacter
    ABirdCharacter* BirdCharacter = CreateBirdCharacterInWindEnvironment();
    UBirdAnimInstance* AnimInstance = GetAnimInstance(BirdCharacter);
    UWindForceComponent* WindComponent = BirdCharacter->GetWindForceComponent();
    
    // When: 様々な風力パターンを適用
    TArray<FVector> WindPatterns = {
        FVector(500.0f, 0.0f, 0.0f),    // 正面風
        FVector(0.0f, 800.0f, 0.0f),    // 横風
        FVector(-300.0f, 0.0f, 200.0f), // 向かい風+上昇気流
        FVector(1000.0f, 500.0f, -100.0f) // 複合風
    };
    
    for (const FVector& WindForce : WindPatterns)
    {
        WindComponent->SetWindForce(WindForce);
        
        // 風力安定化まで待機
        for (int32 i = 0; i < 60; i++)
        {
            Tick(1.0f / 60.0f);
        }
        
        // Then: アニメーションが風力に適応
        FVector CurrentWindInfluence = AnimInstance->GetWindInfluence();
        EXPECT_GT(CurrentWindInfluence.Size(), 0.0f);
        
        // 風力の方向性が反映される
        FVector NormalizedWind = WindForce.GetSafeNormal();
        FVector NormalizedInfluence = CurrentWindInfluence.GetSafeNormal();
        float Correlation = FVector::DotProduct(NormalizedWind, NormalizedInfluence);
        EXPECT_GT(Correlation, 0.3f); // ある程度の相関
    }
}
```

### IT-402-003: パフォーマンス統合テスト

#### IT-402-003-A: マルチキャラクターパフォーマンステスト
```cpp
TEST_F(PerformanceIntegrationTest, MultiCharacterPerformance)
{
    // Given: 複数のBirdCharacter
    TArray<ABirdCharacter*> Birds;
    for (int32 i = 0; i < 20; i++)
    {
        ABirdCharacter* Bird = CreateFullyAnimatedBirdCharacter();
        Birds.Add(Bird);
        
        // 異なる飛行パターンを設定
        Bird->SetFlightState(EFlightState::Flying);
        Bird->GetCharacterMovement()->Velocity = FVector(
            FMath::RandRange(500.0f, 1500.0f),
            FMath::RandRange(-200.0f, 200.0f),
            FMath::RandRange(-100.0f, 300.0f)
        );
    }
    
    // When: 同時アニメーション処理を実行
    float TotalFrameTime = 0.0f;
    int32 FrameCount = 300; // 5秒間
    
    for (int32 Frame = 0; Frame < FrameCount; Frame++)
    {
        float FrameStartTime = GetCurrentTime();
        Tick(1.0f / 60.0f);
        float FrameEndTime = GetCurrentTime();
        TotalFrameTime += (FrameEndTime - FrameStartTime);
    }
    
    // Then: パフォーマンス要件を満たす
    float AverageFrameTime = TotalFrameTime / FrameCount;
    EXPECT_LT(AverageFrameTime, 16.67f); // 60FPS要件
    
    // メモリ使用量チェック
    SIZE_T MemoryUsage = GetCurrentMemoryUsage();
    EXPECT_LT(MemoryUsage, 600 * 1024 * 1024); // 600MB以下
}
```

## パフォーマンステスト仕様

### PT-402-001: アニメーション処理時間テスト

#### PT-402-001-A: AnimInstance更新時間テスト
```cpp
TEST_F(PerformanceTest, AnimInstanceUpdateTime)
{
    // Given: 複雑なAnimInstance
    UBirdAnimInstance* AnimInstance = CreateComplexAnimInstance();
    
    // When: 更新処理時間を測定
    float TotalTime = 0.0f;
    int32 Iterations = 1000;
    
    for (int32 i = 0; i < Iterations; i++)
    {
        float StartTime = GetCurrentTime();
        AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
        float EndTime = GetCurrentTime();
        TotalTime += (EndTime - StartTime);
    }
    
    // Then: 平均処理時間が要件内
    float AverageTime = TotalTime / Iterations;
    EXPECT_LT(AverageTime, 1.5f); // 1.5ms以下
}
```

### PT-402-002: メモリ使用量テスト

#### PT-402-002-A: AnimInstanceメモリテスト
```cpp
TEST_F(PerformanceTest, AnimInstanceMemoryUsage)
{
    // Given: 初期メモリ状態
    SIZE_T InitialMemory = GetCurrentMemoryUsage();
    
    // When: 多数のAnimInstanceを作成
    TArray<UBirdAnimInstance*> AnimInstances;
    for (int32 i = 0; i < 100; i++)
    {
        UBirdAnimInstance* AnimInstance = CreateAnimInstance();
        AnimInstances.Add(AnimInstance);
    }
    
    SIZE_T PeakMemory = GetCurrentMemoryUsage();
    
    // 全て破棄
    for (UBirdAnimInstance* AnimInstance : AnimInstances)
    {
        AnimInstance->ConditionalBeginDestroy();
    }
    AnimInstances.Empty();
    ForceGarbageCollection();
    
    SIZE_T FinalMemory = GetCurrentMemoryUsage();
    
    // Then: メモリ使用量が要件内
    SIZE_T MemoryPerInstance = (PeakMemory - InitialMemory) / 100;
    EXPECT_LT(MemoryPerInstance, 300 * 1024); // 300KB以下/Instance
    
    SIZE_T MemoryLeak = FinalMemory - InitialMemory;
    EXPECT_LT(MemoryLeak, 5 * 1024 * 1024); // 5MB以下のリーク
}
```

## 視覚テスト仕様

### VT-402-001: アニメーション品質テスト

#### VT-402-001-A: 状態遷移スムーズさテスト
- **手順**:
  1. BirdCharacterを様々な飛行状態で配置
  2. 状態遷移トリガーを発生
  3. 遷移アニメーションの視覚確認
- **評価基準**:
  - 状態遷移にポップ（不自然な跳び）がない
  - ブレンド時間が適切（長すぎない、短すぎない）
  - 物理的に自然な遷移

#### VT-402-001-B: ブレンドスペース品質テスト
- **手順**:
  1. 様々な速度・方向入力でテスト実行
  2. ブレンドスペース結果の視覚確認
  3. パラメータ変化時のブレンド品質確認
- **評価基準**:
  - 補間が滑らかで自然
  - 極端なパラメータでも破綻しない
  - リアルタイム変化に適切に追従

#### VT-402-001-C: 物理連携品質テスト
- **手順**:
  1. 風力環境でのアニメーション確認
  2. ControlRigとの統合品質確認
  3. 物理アニメーション応答性確認
- **評価基準**:
  - 物理とアニメーションの一体感
  - 風力変化への自然な応答
  - ControlRigとの違和感ないブレンド

## エラーハンドリングテスト仕様

### ET-402-001: 異常状態テスト

#### ET-402-001-A: 無効な状態遷移テスト
```cpp
TEST_F(ErrorHandlingTest, InvalidStateTransition)
{
    // Given: 特定状態のAnimInstance
    UBirdAnimInstance* AnimInstance = CreateAnimInstanceInSpecificState("Flying");
    
    // When: 無効な遷移を試行
    bool TransitionResult = AnimInstance->TryTransitionToState("InvalidState");
    
    // Then: 適切にエラーハンドリング
    EXPECT_FALSE(TransitionResult);
    EXPECT_EQ(AnimInstance->GetCurrentStateName(), "Flying"); // 状態変化なし
    EXPECT_TRUE(AnimInstance->GetLastError().Contains("Invalid state"));
}
```

#### ET-402-001-B: 破損アニメーションアセットテスト
```cpp
TEST_F(ErrorHandlingTest, CorruptedAnimationAsset)
{
    // Given: 破損アニメーションを含むAnimInstance
    UBirdAnimInstance* AnimInstance = CreateAnimInstanceWithCorruptedAsset();
    
    // When: 破損アニメーション再生を試行
    bool PlayResult = AnimInstance->PlayAnimation("CorruptedAnimation");
    
    // Then: フォールバック処理
    EXPECT_FALSE(PlayResult);
    EXPECT_NE(AnimInstance->GetCurrentAnimation(), nullptr); // フォールバックアニメーション
    EXPECT_TRUE(AnimInstance->GetLastError().Contains("Corrupted asset"));
}
```

#### ET-402-001-C: メモリ不足テスト
```cpp
TEST_F(ErrorHandlingTest, OutOfMemoryHandling)
{
    // Given: メモリ制限環境
    SetMemoryLimit(100 * 1024 * 1024); // 100MB制限
    
    // When: 大量のAnimInstanceを作成
    TArray<UBirdAnimInstance*> AnimInstances;
    bool CreationSuccess = true;
    
    for (int32 i = 0; i < 1000 && CreationSuccess; i++)
    {
        UBirdAnimInstance* AnimInstance = CreateAnimInstance();
        if (AnimInstance)
        {
            AnimInstances.Add(AnimInstance);
        }
        else
        {
            CreationSuccess = false;
        }
    }
    
    // Then: 適切なメモリ不足処理
    EXPECT_FALSE(CreationSuccess);
    EXPECT_GT(AnimInstances.Num(), 0); // 一部は作成成功
    
    // 既存のAnimInstanceは正常動作
    for (UBirdAnimInstance* AnimInstance : AnimInstances)
    {
        EXPECT_TRUE(AnimInstance->IsValidLowLevel());
    }
}
```

## テスト実行計画

### Phase 1: 単体テスト実装
1. BirdAnimInstance基本機能テスト
2. パラメータ更新テスト
3. 状態マシンテスト
4. ブレンドスペーステスト
5. 物理アニメーション統合テスト

### Phase 2: 統合テスト実装
1. BirdCharacter統合テスト
2. 物理システム統合テスト
3. パフォーマンス統合テスト

### Phase 3: パフォーマンステスト実装
1. アニメーション処理時間テスト
2. メモリ使用量テスト
3. マルチキャラクターテスト

### Phase 4: 品質テスト実装
1. 視覚テスト
2. エラーハンドリングテスト
3. 総合的な品質評価テスト

## 継続的インテグレーション

### 自動テスト実行
- 全単体テストの自動実行
- パフォーマンステストの定期実行
- リグレッションテストの自動実行

### 品質メトリクス
- テストカバレッジ: 90%以上
- パフォーマンス要件: 全て満足
- 視覚品質評価: 良好以上

## 成功指標

### 機能指標
- [ ] 全受け入れ基準の達成
- [ ] 単体テスト100%パス
- [ ] 統合テスト100%パス

### 品質指標
- [ ] アニメーション品質評価: 良好以上
- [ ] 状態遷移スムーズさ: 違和感なし
- [ ] ブレンド品質: 自然

### パフォーマンス指標
- [ ] 60FPS安定動作確認
- [ ] メモリ使用量基準内
- [ ] 処理時間基準内

このテストケース仕様に基づいて、実装と並行してテストを作成し、品質の高いAnimation Blueprintシステムを実現します。