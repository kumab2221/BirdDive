# TASK-302: 第一人称カメラ実装 - テストケース定義

## テストケース概要

### テスト戦略
- **単体テスト**: FirstPersonCameraComponentの個別機能確認
- **統合テスト**: BirdCharacterとSplitScreenManagerとの連携確認
- **パフォーマンステスト**: カメラ更新・エフェクト処理負荷測定
- **視覚テスト**: カメラワーク・エフェクト品質確認

### テスト環境
- **ハードウェア**: NVIDIA RTX系、AMD RX系GPU対応
- **解像度**: 1920x1080, 2560x1440, 3840x2160
- **プラットフォーム**: Windows 11
- **UEバージョン**: 5.6

## 単体テストケース

### TC-302-001: FirstPersonCameraComponent基盤テスト

#### TC-302-001-01: コンポーネント初期化
```cpp
TEST(FirstPersonCameraTest, ComponentInitialization)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    AActor* TestActor = World->SpawnActor<AActor>();
    UFirstPersonCameraComponent* Camera = NewObject<UFirstPersonCameraComponent>(TestActor);
    
    // Act
    Camera->InitializeCamera();
    
    // Assert
    EXPECT_TRUE(Camera != nullptr);
    EXPECT_TRUE(Camera->GetCameraComponent() != nullptr);
    EXPECT_EQ(Camera->GetBaseFOV(), 90.0f);
    EXPECT_EQ(Camera->GetCurrentFOV(), 90.0f);
    
    World->DestroyWorld(true);
}
```

#### TC-302-001-02: 設定値検証
```cpp
TEST(FirstPersonCameraTest, SettingsValidation)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    
    // Act & Assert - FOV範囲設定
    Camera->SetFOVRange(60.0f, 120.0f);
    EXPECT_EQ(Camera->GetMinFOV(), 60.0f);
    EXPECT_EQ(Camera->GetMaxFOV(), 120.0f);
    
    // Act & Assert - ベースFOV設定
    Camera->SetBaseFOV(85.0f);
    EXPECT_EQ(Camera->GetBaseFOV(), 85.0f);
    
    // Act & Assert - 無効な値のクランプ
    Camera->SetFOVRange(150.0f, 30.0f); // 逆転した値
    EXPECT_LE(Camera->GetMinFOV(), Camera->GetMaxFOV());
    
    World->DestroyWorld(true);
}
```

### TC-302-002: FOV管理システムテスト

#### TC-302-002-01: 速度ベースFOV計算
```cpp
TEST(FirstPersonCameraTest, SpeedBasedFOVCalculation)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    Camera->SetFOVRange(60.0f, 120.0f);
    Camera->EnableSpeedBasedFOV(true);
    
    // Act & Assert - 低速時 (ベースFOV維持)
    Camera->UpdateFOVBasedOnSpeed(50.0f); // 最小閾値以下
    EXPECT_FLOAT_EQ(Camera->GetCurrentFOV(), 90.0f);
    
    // Act & Assert - 中速時 (FOV増加)
    Camera->UpdateFOVBasedOnSpeed(500.0f);
    float MidSpeedFOV = Camera->GetCurrentFOV();
    EXPECT_GT(MidSpeedFOV, 90.0f);
    EXPECT_LT(MidSpeedFOV, 120.0f);
    
    // Act & Assert - 高速時 (最大FOV)
    Camera->UpdateFOVBasedOnSpeed(2500.0f); // 最大閾値超過
    EXPECT_FLOAT_EQ(Camera->GetCurrentFOV(), 120.0f);
    
    World->DestroyWorld(true);
}
```

#### TC-302-002-02: FOVスムーズ補間
```cpp
TEST(FirstPersonCameraTest, FOVSmoothInterpolation)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    Camera->SetFOVRange(60.0f, 120.0f);
    Camera->EnableSpeedBasedFOV(true);
    
    float InitialFOV = Camera->GetCurrentFOV();
    
    // Act - 急激な速度変化
    Camera->UpdateFOVBasedOnSpeed(2000.0f);
    
    // シミュレートされたフレーム更新
    for (int32 Frame = 0; Frame < 10; ++Frame)
    {
        Camera->UpdateCamera(1.0f / 60.0f); // 60fps
    }
    
    // Assert - 滑らかな変化
    float FinalFOV = Camera->GetCurrentFOV();
    EXPECT_NE(FinalFOV, InitialFOV); // 変化あり
    EXPECT_LT(abs(FinalFOV - InitialFOV), 120.0f - 60.0f); // 段階的変化
    
    World->DestroyWorld(true);
}
```

### TC-302-003: カメラアタッチメントテスト

#### TC-302-003-01: ソケット・ボーンへのアタッチ
```cpp
TEST(FirstPersonCameraTest, SocketBoneAttachment)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    USkeletalMeshComponent* SkeletalMesh = CreateTestSkeletalMesh(World);
    
    // モックソケット作成
    FName HeadSocketName = "head_socket";
    
    // Act
    bool bAttachSuccess = Camera->AttachToSocketOrBone(SkeletalMesh, HeadSocketName);
    
    // Assert
    EXPECT_TRUE(bAttachSuccess);
    EXPECT_EQ(Camera->GetAttachParent(), SkeletalMesh);
    EXPECT_EQ(Camera->GetAttachSocketName(), HeadSocketName);
    
    // Act - 無効なソケット名
    bool bInvalidAttach = Camera->AttachToSocketOrBone(SkeletalMesh, "invalid_socket");
    
    // Assert
    EXPECT_FALSE(bInvalidAttach);
    
    World->DestroyWorld(true);
}
```

#### TC-302-003-02: 位置・回転追従精度
```cpp
TEST(FirstPersonCameraTest, TransformFollowingAccuracy)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    USkeletalMeshComponent* SkeletalMesh = CreateTestSkeletalMesh(World);
    Camera->AttachToSocketOrBone(SkeletalMesh, "head_socket");
    
    // Act - 親の変形
    FTransform InitialTransform = SkeletalMesh->GetComponentTransform();
    FTransform NewTransform = InitialTransform;
    NewTransform.SetLocation(InitialTransform.GetLocation() + FVector(100, 0, 0));
    NewTransform.SetRotation(FQuat::MakeFromEuler(FVector(0, 45, 0)));
    
    SkeletalMesh->SetWorldTransform(NewTransform);
    Camera->UpdateCamera(1.0f / 60.0f);
    
    // Assert - 追従精度 (誤差1cm以内)
    FVector CameraLocation = Camera->GetComponentLocation();
    FVector ExpectedLocation = NewTransform.TransformPosition(FVector::ZeroVector);
    EXPECT_LT(FVector::Dist(CameraLocation, ExpectedLocation), 1.0f);
    
    World->DestroyWorld(true);
}
```

### TC-302-004: ポストプロセスエフェクトテスト

#### TC-302-004-01: モーションブラー制御
```cpp
TEST(FirstPersonCameraTest, MotionBlurControl)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    Camera->SetPostProcessEffects(true);
    
    // Act & Assert - 低速時 (モーションブラー無効)
    Camera->UpdatePostProcessEffects(100.0f, 1.0f / 60.0f);
    float LowSpeedBlur = Camera->GetMotionBlurIntensity();
    EXPECT_FLOAT_EQ(LowSpeedBlur, 0.0f);
    
    // Act & Assert - 高速時 (モーションブラー有効)
    Camera->UpdatePostProcessEffects(1500.0f, 1.0f / 60.0f);
    float HighSpeedBlur = Camera->GetMotionBlurIntensity();
    EXPECT_GT(HighSpeedBlur, 0.0f);
    EXPECT_LE(HighSpeedBlur, 0.5f); // 最大値制限
    
    World->DestroyWorld(true);
}
```

#### TC-302-004-02: エフェクト有効/無効切り替え
```cpp
TEST(FirstPersonCameraTest, EffectsToggle)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    
    // Act & Assert - エフェクト有効
    Camera->SetPostProcessEffects(true);
    Camera->UpdatePostProcessEffects(1000.0f, 1.0f / 60.0f);
    EXPECT_TRUE(Camera->ArePostProcessEffectsEnabled());
    
    // Act & Assert - エフェクト無効
    Camera->SetPostProcessEffects(false);
    Camera->UpdatePostProcessEffects(1000.0f, 1.0f / 60.0f);
    EXPECT_FALSE(Camera->ArePostProcessEffectsEnabled());
    EXPECT_FLOAT_EQ(Camera->GetMotionBlurIntensity(), 0.0f);
    
    World->DestroyWorld(true);
}
```

## 統合テストケース

### TC-302-101: BirdCharacter統合テスト

#### TC-302-101-01: BirdCharacterへの統合
```cpp
TEST(FirstPersonCameraIntegrationTest, BirdCharacterIntegration)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* Bird = World->SpawnActor<ABirdCharacter>();
    
    // Act
    UFirstPersonCameraComponent* FPCamera = Bird->GetFirstPersonCamera();
    
    // Assert
    EXPECT_TRUE(FPCamera != nullptr);
    EXPECT_TRUE(FPCamera->IsAttachedToParent());
    EXPECT_EQ(FPCamera->GetAttachParent(), Bird->GetMesh());
    
    World->DestroyWorld(true);
}
```

#### TC-302-101-02: 速度データ連携
```cpp
TEST(FirstPersonCameraIntegrationTest, SpeedDataIntegration)
{
    // Arrange
    auto [Bird, World] = CreateTestBirdCharacter();
    UFirstPersonCameraComponent* FPCamera = Bird->GetFirstPersonCamera();
    
    // Act - 鳥を移動させる
    FVector InitialLocation = Bird->GetActorLocation();
    FVector TargetLocation = InitialLocation + FVector(1000, 0, 0);
    
    // 移動をシミュレート
    for (int32 Frame = 0; Frame < 60; ++Frame)
    {
        Bird->SetActorLocation(FMath::Lerp(InitialLocation, TargetLocation, Frame / 60.0f));
        Bird->Tick(1.0f / 60.0f);
    }
    
    // Assert - 速度に応じたFOV変化
    float CurrentFOV = FPCamera->GetCurrentFOV();
    EXPECT_NE(CurrentFOV, 90.0f); // ベースFOVから変化
    
    World->DestroyWorld(true);
}
```

### TC-302-102: SplitScreenManager統合テスト

#### TC-302-102-01: 分割スクリーン統合
```cpp
TEST(FirstPersonCameraIntegrationTest, SplitScreenIntegration)
{
    // Arrange
    auto [Bird, World] = CreateTestBirdCharacter();
    USplitScreenManager* SplitScreen = CreateTestSplitScreenManager(World);
    UFirstPersonCameraComponent* FPCamera = Bird->GetFirstPersonCamera();
    
    // Act - 第一人称カメラをビューポート0に割り当て
    bool bAssignSuccess = SplitScreen->AssignCamera(0, FPCamera->GetCameraComponent());
    
    // Assert
    EXPECT_TRUE(bAssignSuccess);
    EXPECT_EQ(SplitScreen->GetAssignedCamera(0), FPCamera->GetCameraComponent());
    
    World->DestroyWorld(true);
}
```

## パフォーマンステストケース

### TC-302-201: カメラ更新性能テスト

#### TC-302-201-01: 更新処理負荷測定
```cpp
TEST(FirstPersonCameraPerformanceTest, UpdatePerformance)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    FPerformanceProfiler Profiler;
    
    const int32 TestFrames = 3600; // 60秒分 at 60fps
    
    // Act - 継続的な更新処理
    Profiler.StartProfiling();
    
    for (int32 Frame = 0; Frame < TestFrames; ++Frame)
    {
        Camera->UpdateCamera(1.0f / 60.0f);
        Camera->UpdateFOVBasedOnSpeed(FMath::RandRange(0.0f, 2000.0f));
    }
    
    Profiler.StopProfiling();
    
    // Assert - 1フレームあたりの処理時間
    float AverageFrameTime = Profiler.GetAverageFrameTime();
    EXPECT_LT(AverageFrameTime, 0.1f); // 0.1ms以下
    
    World->DestroyWorld(true);
}
```

#### TC-302-201-02: メモリ使用量テスト
```cpp
TEST(FirstPersonCameraPerformanceTest, MemoryUsage)
{
    // Arrange
    size_t BaselineMemory = GetCurrentMemoryUsage();
    
    // Act - 複数カメラの作成・破棄
    TArray<UFirstPersonCameraComponent*> Cameras;
    
    for (int32 i = 0; i < 100; ++i)
    {
        UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
        auto Camera = CreateTestCamera(World);
        Cameras.Add(Camera.Key);
    }
    
    size_t PeakMemory = GetCurrentMemoryUsage();
    
    // クリーンアップ
    for (auto Camera : Cameras)
    {
        Camera->Shutdown();
    }
    Cameras.Empty();
    
    size_t FinalMemory = GetCurrentMemoryUsage();
    
    // Assert
    size_t PerCameraMemory = (PeakMemory - BaselineMemory) / 100;
    EXPECT_LT(PerCameraMemory, 10 * 1024 * 1024); // 10MB以下
    EXPECT_LE(FinalMemory, BaselineMemory * 1.1f); // メモリリーク検証
}
```

### TC-302-202: ポストプロセス性能テスト

#### TC-302-202-01: エフェクト処理負荷
```cpp
TEST(FirstPersonCameraPerformanceTest, PostProcessPerformance)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    Camera->SetPostProcessEffects(true);
    
    FGPUProfiler GPUProfiler;
    
    // Act - エフェクト処理
    GPUProfiler.StartProfiling();
    
    for (int32 Frame = 0; Frame < 300; ++Frame) // 5秒間
    {
        float Speed = FMath::Sin(Frame * 0.1f) * 1000.0f + 1000.0f; // 0-2000の範囲で変動
        Camera->UpdatePostProcessEffects(Speed, 1.0f / 60.0f);
    }
    
    GPUProfiler.StopProfiling();
    
    // Assert - GPU使用率
    float AverageGPUUsage = GPUProfiler.GetAverageGPUUsage();
    EXPECT_LT(AverageGPUUsage, 5.0f); // 5%以下の追加GPU使用率
    
    World->DestroyWorld(true);
}
```

## 視覚テストケース

### TC-302-301: カメラワーク品質テスト

#### 手動テスト項目
1. **追従スムーズネス**: カメラが頭部に滑らかに追従するか
2. **FOV変化自然さ**: 速度変化時のFOV変化が自然か
3. **エフェクト品質**: モーションブラー・歪みエフェクトが適切か
4. **酔いにくさ**: 長時間プレイでも不快感がないか

#### 自動化テスト
```cpp
TEST(FirstPersonCameraVisualTest, CameraFollowSmoothnessn)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    USkeletalMeshComponent* HeadMesh = CreateTestSkeletalMesh(World);
    Camera->AttachToSocketOrBone(HeadMesh, "head");
    
    TArray<FVector> CameraPositions;
    TArray<FVector> HeadPositions;
    
    // Act - 頭部の複雑な動きをシミュレート
    for (int32 Frame = 0; Frame < 120; ++Frame) // 2秒間
    {
        float Time = Frame / 60.0f;
        FVector HeadPos = FVector(
            FMath::Sin(Time * 2.0f) * 100,
            FMath::Cos(Time * 1.5f) * 80,
            FMath::Sin(Time * 3.0f) * 50
        );
        
        HeadMesh->SetRelativeLocation(HeadPos);
        Camera->UpdateCamera(1.0f / 60.0f);
        
        CameraPositions.Add(Camera->GetComponentLocation());
        HeadPositions.Add(HeadMesh->GetComponentLocation());
    }
    
    // Assert - 追従精度と滑らかさ
    for (int32 i = 0; i < CameraPositions.Num(); ++i)
    {
        float Distance = FVector::Dist(CameraPositions[i], HeadPositions[i]);
        EXPECT_LT(Distance, 5.0f); // 5cm以内の追従精度
        
        if (i > 0)
        {
            float FrameDistance = FVector::Dist(CameraPositions[i], CameraPositions[i-1]);
            EXPECT_LT(FrameDistance, 50.0f); // 急激な動きなし
        }
    }
    
    World->DestroyWorld(true);
}
```

## エラーハンドリングテストケース

### TC-302-401: 異常系処理テスト

#### TC-302-401-01: アタッチメント失敗時処理
```cpp
TEST(FirstPersonCameraErrorTest, AttachmentFailureHandling)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    
    // Act & Assert - nullメッシュ
    bool bNullAttach = Camera->AttachToSocketOrBone(nullptr, "head");
    EXPECT_FALSE(bNullAttach);
    EXPECT_NO_THROW(Camera->UpdateCamera(1.0f / 60.0f));
    
    // Act & Assert - 存在しないソケット
    USkeletalMeshComponent* Mesh = CreateTestSkeletalMesh(World);
    bool bInvalidSocket = Camera->AttachToSocketOrBone(Mesh, "nonexistent_socket");
    EXPECT_FALSE(bInvalidSocket);
    EXPECT_NO_THROW(Camera->UpdateCamera(1.0f / 60.0f));
    
    World->DestroyWorld(true);
}
```

#### TC-302-401-02: 無効な設定値処理
```cpp
TEST(FirstPersonCameraErrorTest, InvalidSettingsHandling)
{
    // Arrange
    auto [Camera, World] = CreateTestCamera();
    
    // Act & Assert - 無効なFOV値
    EXPECT_NO_THROW(Camera->SetBaseFOV(-10.0f)); // 負の値
    EXPECT_NO_THROW(Camera->SetBaseFOV(200.0f)); // 極大値
    
    // 値が適切な範囲にクランプされることを確認
    float ClampedFOV = Camera->GetBaseFOV();
    EXPECT_GE(ClampedFOV, 30.0f);  // 最小値
    EXPECT_LE(ClampedFOV, 160.0f); // 最大値
    
    // Act & Assert - 無効なFOV範囲
    EXPECT_NO_THROW(Camera->SetFOVRange(120.0f, 60.0f)); // 逆転した範囲
    EXPECT_LE(Camera->GetMinFOV(), Camera->GetMaxFOV());
    
    World->DestroyWorld(true);
}
```

## テスト実行手順

### 自動テスト実行
```bash
# 全単体テスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.FirstPersonCamera"

# 特定カテゴリテスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.FirstPersonCamera.Component"
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.FirstPersonCamera.Performance"

# 統合テスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.FirstPersonCamera.Integration"
```

### 手動テスト実行
1. BirdCharacterでテストマップ読み込み
2. 第一人称カメラモードに切り替え
3. 様々な速度での飛行テスト
4. ポストプロセスエフェクトの確認
5. 長時間プレイでの快適性確認

## 成功基準

### 自動テスト
- [ ] 全単体テストケースが通過 (100%)
- [ ] 統合テストケースが通過 (100%)
- [ ] パフォーマンステストが要件クリア (100%)
- [ ] エラーハンドリングテストが通過 (100%)

### 手動テスト
- [ ] カメラワークが滑らかで自然
- [ ] 速度感のあるFOV変化が実現
- [ ] エフェクトが適切に機能
- [ ] 酔いにくい快適な視点

### 品質基準
- [ ] 追従遅延1フレーム以内
- [ ] CPU負荷0.1ms以下
- [ ] メモリ使用量10MB以下
- [ ] コードカバレッジ90%以上

## テストデータ・ヘルパー

### テスト用ヘルパー関数
```cpp
// Test helper functions
std::pair<UFirstPersonCameraComponent*, UWorld*> CreateTestCamera();
std::pair<ABirdCharacter*, UWorld*> CreateTestBirdCharacter();
USkeletalMeshComponent* CreateTestSkeletalMesh(UWorld* World);
USplitScreenManager* CreateTestSplitScreenManager(UWorld* World);
size_t GetCurrentMemoryUsage();
```

### テストデータ
- 標準FOV設定: BaseFOV=90°, MinFOV=60°, MaxFOV=120°
- 速度範囲: 0-2500 units/second
- テスト解像度: 1920x1080, 2560x1440
- フレームレート: 60fps基準

このテストスイートにより、TASK-302の完全な検証が可能になります。