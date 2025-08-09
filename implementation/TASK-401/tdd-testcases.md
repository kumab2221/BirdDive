# TASK-401: ControlRigシステム実装 - テストケース仕様

## テスト戦略概要

ControlRigシステムの実装に対して、TDDアプローチに基づく包括的なテストスイートを構築する。

## 単体テスト仕様

### UT-401-001: BirdControlRigComponent基本機能テスト

#### UT-401-001-A: コンポーネント初期化テスト
```cpp
TEST_F(BirdControlRigComponentTest, ComponentInitialization)
{
    // Given: 新しいBirdControlRigComponent
    UBirdControlRigComponent* Component = CreateComponent();
    
    // When: コンポーネントを初期化
    Component->BeginPlay();
    
    // Then: 正常に初期化される
    EXPECT_TRUE(Component->IsInitialized());
    EXPECT_NE(Component->GetControlRigInstance(), nullptr);
    EXPECT_TRUE(Component->IsValidLowLevel());
}
```

#### UT-401-001-B: ControlRigインスタンス取得テスト
```cpp
TEST_F(BirdControlRigComponentTest, ControlRigInstanceAccess)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: ControlRigインスタンスを取得
    UControlRig* ControlRigInstance = Component->GetControlRigInstance();
    
    // Then: 有効なインスタンスが取得される
    EXPECT_NE(ControlRigInstance, nullptr);
    EXPECT_TRUE(ControlRigInstance->IsValidLowLevel());
    EXPECT_EQ(ControlRigInstance->GetClass(), UBirdControlRig::StaticClass());
}
```

#### UT-401-001-C: パラメータ設定テスト
```cpp
TEST_F(BirdControlRigComponentTest, ParameterSetting)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 制御パラメータを設定
    FBirdControlRigParams TestParams;
    TestParams.FlappingFrequency = 2.5f;
    TestParams.WingAngle = 45.0f;
    TestParams.WingSpread = 0.8f;
    Component->SetControlParameters(TestParams);
    
    // Then: パラメータが正しく設定される
    FBirdControlRigParams RetrievedParams = Component->GetControlParameters();
    EXPECT_FLOAT_EQ(RetrievedParams.FlappingFrequency, 2.5f);
    EXPECT_FLOAT_EQ(RetrievedParams.WingAngle, 45.0f);
    EXPECT_FLOAT_EQ(RetrievedParams.WingSpread, 0.8f);
}
```

### UT-401-002: 羽ばたき制御テスト

#### UT-401-002-A: 基本羽ばたき頻度テスト
```cpp
TEST_F(BirdControlRigComponentTest, BasicFlappingFrequency)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 飛行速度を設定
    float FlightSpeed = 1000.0f;
    Component->SetFlightSpeed(FlightSpeed);
    Component->UpdateFlapping(1.0f);
    
    // Then: 適切な羽ばたき頻度が設定される
    float ExpectedFrequency = Component->CalculateFlappingFrequency(FlightSpeed);
    FBirdControlRigParams Params = Component->GetControlParameters();
    EXPECT_FLOAT_EQ(Params.FlappingFrequency, ExpectedFrequency);
}
```

#### UT-401-002-B: 速度連動羽ばたきテスト
```cpp
TEST_F(BirdControlRigComponentTest, SpeedLinkedFlapping)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 異なる飛行速度を設定
    float LowSpeed = 500.0f;
    float HighSpeed = 2000.0f;
    
    Component->SetFlightSpeed(LowSpeed);
    float LowSpeedFrequency = Component->GetControlParameters().FlappingFrequency;
    
    Component->SetFlightSpeed(HighSpeed);
    float HighSpeedFrequency = Component->GetControlParameters().FlappingFrequency;
    
    // Then: 高速時の方が羽ばたき頻度が高い
    EXPECT_GT(HighSpeedFrequency, LowSpeedFrequency);
    EXPECT_GT(HighSpeedFrequency, 1.5f * LowSpeedFrequency);
}
```

#### UT-401-002-C: 飛行状態別翼角度テスト
```cpp
TEST_F(BirdControlRigComponentTest, FlightStateWingAngle)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 上昇状態を設定
    Component->SetFlightState(EFlightState::Ascending);
    Component->UpdateWingPosition(1.0f);
    float AscendingAngle = Component->GetControlParameters().WingAngle;
    
    // When: 下降状態を設定
    Component->SetFlightState(EFlightState::Descending);
    Component->UpdateWingPosition(1.0f);
    float DescendingAngle = Component->GetControlParameters().WingAngle;
    
    // Then: 状態に応じて翼角度が変化
    EXPECT_GT(AscendingAngle, 0.0f);
    EXPECT_LT(DescendingAngle, 0.0f);
    EXPECT_GT(AscendingAngle, DescendingAngle);
}
```

### UT-401-003: 風力応答制御テスト

#### UT-401-003-A: WindForceComponent連携テスト
```cpp
TEST_F(BirdControlRigComponentTest, WindForceIntegration)
{
    // Given: WindForceComponentを持つBirdCharacter
    ABirdCharacter* BirdCharacter = CreateTestBirdCharacter();
    UWindForceComponent* WindComponent = BirdCharacter->GetWindForceComponent();
    UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
    
    // When: 風力データを設定
    FVector WindForce = FVector(100.0f, 0.0f, 50.0f);
    WindComponent->SetWindForce(WindForce);
    ControlRigComponent->UpdateWindResponse(1.0f);
    
    // Then: 風力に応じて姿勢パラメータが変化
    FBirdControlRigParams Params = ControlRigComponent->GetControlParameters();
    EXPECT_NE(Params.BodyRotation, FRotator::ZeroRotator);
    EXPECT_GT(Params.WindInfluence.Size(), 0.0f);
}
```

#### UT-401-003-B: 風向き応答テスト
```cpp
TEST_F(BirdControlRigComponentTest, WindDirectionResponse)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 横風を設定
    FVector CrossWind = FVector(0.0f, 1000.0f, 0.0f);
    Component->ApplyWindForce(CrossWind);
    Component->UpdateWindResponse(1.0f);
    
    // Then: 横風に対して適切に傾斜
    FBirdControlRigParams Params = Component->GetControlParameters();
    EXPECT_NE(Params.BodyRotation.Roll, 0.0f);
    EXPECT_GT(FMath::Abs(Params.BodyRotation.Roll), 5.0f);
}
```

#### UT-401-003-C: 風力安定化テスト
```cpp
TEST_F(BirdControlRigComponentTest, WindStabilization)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 強風を適用して安定化を実行
    FVector StrongWind = FVector(1500.0f, 0.0f, 0.0f);
    Component->ApplyWindForce(StrongWind);
    Component->SetStabilizationStrength(1.0f);
    Component->UpdateWindResponse(1.0f);
    
    // Then: 翼が安定化姿勢を取る
    FBirdControlRigParams Params = Component->GetControlParameters();
    EXPECT_GT(Params.StabilizationStrength, 0.8f);
    EXPECT_GT(Params.WingSpread, 0.9f);
}
```

### UT-401-004: プロシージャル制御テスト

#### UT-401-004-A: 個別羽根制御テスト
```cpp
TEST_F(BirdControlRigComponentTest, IndividualFeatherControl)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 個別羽根パラメータを設定
    TArray<float> FeatherAngles = {10.0f, 15.0f, 8.0f, 12.0f, 20.0f};
    Component->SetIndividualFeatherAngles(FeatherAngles);
    
    // Then: 各羽根が独立して制御される
    TArray<float> RetrievedAngles = Component->GetIndividualFeatherAngles();
    EXPECT_EQ(RetrievedAngles.Num(), FeatherAngles.Num());
    for (int32 i = 0; i < FeatherAngles.Num(); i++)
    {
        EXPECT_FLOAT_EQ(RetrievedAngles[i], FeatherAngles[i]);
    }
}
```

#### UT-401-004-B: 尾羽方向制御テスト
```cpp
TEST_F(BirdControlRigComponentTest, TailFeatherDirectionControl)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 尾羽角度を設定
    float TailFanAngle = 30.0f;
    Component->SetTailFanAngle(TailFanAngle);
    
    // Then: 尾羽が指定角度に制御される
    FBirdControlRigParams Params = Component->GetControlParameters();
    EXPECT_FLOAT_EQ(Params.TailFanAngle, TailFanAngle);
    
    // When: 方向転換を実行
    FRotator TurnDirection = FRotator(0.0f, 45.0f, 0.0f);
    Component->ApplyTailControl(TurnDirection);
    
    // Then: 尾羽が方向制御に応答
    FBirdControlRigParams UpdatedParams = Component->GetControlParameters();
    EXPECT_NE(UpdatedParams.TailFanAngle, TailFanAngle);
}
```

## 統合テスト仕様

### IT-401-001: BirdCharacter統合テスト

#### IT-401-001-A: コンポーネント統合テスト
```cpp
TEST_F(BirdCharacterIntegrationTest, ControlRigComponentIntegration)
{
    // Given: 完全なBirdCharacterセットアップ
    ABirdCharacter* BirdCharacter = CreateFullyConfiguredBirdCharacter();
    UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
    
    // When: キャラクターを飛行状態にする
    BirdCharacter->SetFlightState(EFlightState::Flying);
    BirdCharacter->GetCharacterMovement()->Velocity = FVector(1000.0f, 0.0f, 100.0f);
    
    // Then: ControlRigが自動的に更新される
    Tick(1.0f);
    FBirdControlRigParams Params = ControlRigComponent->GetControlParameters();
    EXPECT_GT(Params.FlappingFrequency, 0.0f);
    EXPECT_NE(Params.WingAngle, 0.0f);
}
```

#### IT-401-001-B: 物理システム連携テスト
```cpp
TEST_F(BirdCharacterIntegrationTest, PhysicsSystemIntegration)
{
    // Given: 風力環境のBirdCharacter
    ABirdCharacter* BirdCharacter = CreateBirdCharacterWithWind();
    UWindForceComponent* WindComponent = BirdCharacter->GetWindForceComponent();
    UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
    
    // When: 風力を適用
    FVector WindForce = FVector(500.0f, 200.0f, 0.0f);
    WindComponent->ApplyWindForce(WindForce);
    
    // Then: ControlRigが風力に応答
    for (int32 i = 0; i < 60; i++) // 1秒間シミュレート
    {
        Tick(1.0f / 60.0f);
    }
    
    FBirdControlRigParams Params = ControlRigComponent->GetControlParameters();
    EXPECT_GT(Params.WindInfluence.Size(), 0.0f);
    EXPECT_NE(Params.BodyRotation, FRotator::ZeroRotator);
}
```

### IT-401-002: アニメーションシステム統合テスト

#### IT-401-002-A: AnimBP連携テスト
```cpp
TEST_F(AnimationSystemIntegrationTest, AnimBPIntegration)
{
    // Given: AnimBPを持つBirdCharacter
    ABirdCharacter* BirdCharacter = CreateBirdCharacterWithAnimBP();
    UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
    USkeletalMeshComponent* MeshComponent = BirdCharacter->GetMesh();
    
    // When: ControlRigパラメータを変更
    FBirdControlRigParams Params;
    Params.FlappingFrequency = 3.0f;
    Params.WingAngle = 25.0f;
    ControlRigComponent->SetControlParameters(Params);
    
    // Then: アニメーションに反映される
    Tick(0.1f);
    
    // ボーンの位置/回転を確認
    FTransform WingBoneTransform = MeshComponent->GetBoneTransform(FName("wing_l"), EBoneSpaces::ComponentSpace);
    EXPECT_NE(WingBoneTransform.GetRotation(), FQuat::Identity);
}
```

## パフォーマンステスト仕様

### PT-401-001: フレームレートテスト

#### PT-401-001-A: 60FPS維持テスト
```cpp
TEST_F(PerformanceTest, SixtyFPSMaintenance)
{
    // Given: 複数のBirdCharacter
    TArray<ABirdCharacter*> Birds;
    for (int32 i = 0; i < 10; i++)
    {
        Birds.Add(CreateBirdCharacterWithControlRig());
    }
    
    // When: 1秒間フルシミュレーション実行
    float TotalFrameTime = 0.0f;
    int32 FrameCount = 60;
    
    for (int32 Frame = 0; Frame < FrameCount; Frame++)
    {
        float FrameStartTime = GetCurrentTime();
        Tick(1.0f / 60.0f);
        float FrameEndTime = GetCurrentTime();
        TotalFrameTime += (FrameEndTime - FrameStartTime);
    }
    
    // Then: 平均フレーム時間が16.67ms以下
    float AverageFrameTime = TotalFrameTime / FrameCount;
    EXPECT_LT(AverageFrameTime, 16.67f);
}
```

### PT-401-002: メモリ使用量テスト

#### PT-401-002-A: メモリリークテスト
```cpp
TEST_F(PerformanceTest, MemoryLeakTest)
{
    // Given: 初期メモリ使用量を記録
    SIZE_T InitialMemory = GetCurrentMemoryUsage();
    
    // When: ControlRigComponentを大量作成・破棄
    for (int32 i = 0; i < 1000; i++)
    {
        UBirdControlRigComponent* Component = CreateComponent();
        Component->BeginPlay();
        Component->EndPlay(EEndPlayReason::Destroyed);
        Component->ConditionalBeginDestroy();
    }
    
    // Force garbage collection
    ForceGarbageCollection();
    
    // Then: メモリ使用量が初期値に戻る
    SIZE_T FinalMemory = GetCurrentMemoryUsage();
    SIZE_T MemoryDifference = FinalMemory - InitialMemory;
    EXPECT_LT(MemoryDifference, 10 * 1024 * 1024); // 10MB以下
}
```

### PT-401-003: 処理時間プロファイリングテスト

#### PT-401-003-A: ControlRig処理時間テスト
```cpp
TEST_F(PerformanceTest, ControlRigProcessingTime)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 更新処理を測定
    float TotalTime = 0.0f;
    int32 Iterations = 1000;
    
    for (int32 i = 0; i < Iterations; i++)
    {
        float StartTime = GetCurrentTime();
        Component->UpdateControlRig(1.0f / 60.0f);
        float EndTime = GetCurrentTime();
        TotalTime += (EndTime - StartTime);
    }
    
    // Then: 平均処理時間が2ms以下
    float AverageTime = TotalTime / Iterations;
    EXPECT_LT(AverageTime, 2.0f);
}
```

## 視覚テスト仕様

### VT-401-001: アニメーション品質テスト

#### VT-401-001-A: 羽ばたき自然性テスト
- **手順**:
  1. BirdCharacterを飛行状態で配置
  2. 異なる飛行速度でテスト実行
  3. 羽ばたきアニメーションの視覚確認
- **評価基準**:
  - 羽ばたきが自然で滑らか
  - 速度変化に適切に追従
  - アニメーション間の遷移がスムーズ

#### VT-401-001-B: 風力応答テスト
- **手順**:
  1. 様々な風向き・風力を設定
  2. 鳥の姿勢変化を視覚確認
  3. 風力変化への応答性を確認
- **評価基準**:
  - 風向きに応じた適切な傾斜
  - 風力に応じた安定化動作
  - 物理的に妥当な姿勢制御

## エラーハンドリングテスト仕様

### ET-401-001: 異常状態テスト

#### ET-401-001-A: ControlRig未初期化テスト
```cpp
TEST_F(ErrorHandlingTest, UninitializedControlRig)
{
    // Given: 未初期化のComponent
    UBirdControlRigComponent* Component = CreateComponent();
    
    // When: 操作を試行
    FBirdControlRigParams Params;
    
    // Then: 適切にエラーハンドリング
    EXPECT_FALSE(Component->SetControlParameters(Params));
    EXPECT_TRUE(Component->GetLastError().Contains("Not initialized"));
}
```

#### ET-401-001-B: 無効パラメータテスト
```cpp
TEST_F(ErrorHandlingTest, InvalidParameters)
{
    // Given: 初期化済みComponent
    UBirdControlRigComponent* Component = CreateInitializedComponent();
    
    // When: 無効なパラメータを設定
    FBirdControlRigParams InvalidParams;
    InvalidParams.FlappingFrequency = -1.0f; // 負の値
    InvalidParams.WingAngle = 180.0f; // 範囲外
    
    // Then: パラメータが拒否される
    EXPECT_FALSE(Component->SetControlParameters(InvalidParams));
    EXPECT_TRUE(Component->GetLastError().Contains("Invalid parameter"));
}
```

## テスト実行計画

### Phase 1: 単体テスト実装
1. BirdControlRigComponent基本機能テスト
2. 羽ばたき制御テスト
3. 風力応答制御テスト
4. プロシージャル制御テスト

### Phase 2: 統合テスト実装
1. BirdCharacter統合テスト
2. アニメーションシステム統合テスト
3. 物理システム統合テスト

### Phase 3: パフォーマンステスト実装
1. フレームレートテスト
2. メモリ使用量テスト
3. 処理時間プロファイリングテスト

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

このテストケース仕様に基づいて、実装と並行してテストを作成し、品質の高いControlRigシステムを実現します。