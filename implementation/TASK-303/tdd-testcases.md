# TASK-303: サイドビューカメラ実装 - テストケース定義

## テストケース概要

### テスト戦略
- **単体テスト**: SideViewCameraComponentの個別機能確認
- **統合テスト**: BirdCharacterとSplitScreenManagerとの連携確認
- **パフォーマンステスト**: カメラ追従・先読み処理負荷測定
- **視覚テスト**: カメラワーク・追従品質確認

### テスト環境
- **ハードウェア**: NVIDIA RTX系、AMD RX系GPU対応
- **解像度**: 1920x1080, 2560x1440, 3840x2160
- **プラットフォーム**: Windows 11
- **UEバージョン**: 5.6

## 単体テストケース

### TC-303-001: SideViewCameraComponent基盤テスト

#### TC-303-001-01: コンポーネント初期化
```cpp
TEST(SideViewCameraTest, ComponentInitialization)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    AActor* TestActor = World->SpawnActor<AActor>();
    USideViewCameraComponent* Camera = NewObject<USideViewCameraComponent>(TestActor);
    
    // Act
    Camera->InitializeCamera();
    
    // Assert
    EXPECT_TRUE(Camera != nullptr);
    EXPECT_TRUE(Camera->GetCameraComponent() != nullptr);
    EXPECT_EQ(Camera->GetFollowDistance(), 500.0f);
    EXPECT_EQ(Camera->GetLookAheadStrength(), 1.0f);
    
    World->DestroyWorld(true);
}
```

#### TC-303-001-02: 設定値検証
```cpp
TEST(SideViewCameraTest, SettingsValidation)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    
    // Act & Assert - 追従距離設定
    Camera->SetFollowDistance(300.0f);
    EXPECT_EQ(Camera->GetFollowDistance(), 300.0f);
    
    // Act & Assert - 先読み強度設定
    Camera->SetLookAheadStrength(0.5f);
    EXPECT_EQ(Camera->GetLookAheadStrength(), 0.5f);
    
    // Act & Assert - 境界設定
    FVector MinBounds(-1000, -1000, -500);
    FVector MaxBounds(1000, 1000, 500);
    Camera->SetBoundaryLimits(MinBounds, MaxBounds);
    
    const auto& Settings = Camera->GetSettings();
    EXPECT_EQ(Settings.BoundaryMin, MinBounds);
    EXPECT_EQ(Settings.BoundaryMax, MaxBounds);
    
    World->DestroyWorld(true);
}
```

### TC-303-002: 追従システムテスト

#### TC-303-002-01: ターゲット設定・追従
```cpp
TEST(SideViewCameraTest, TargetFollowing)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    AActor* TargetActor = World->SpawnActor<AActor>();
    TargetActor->SetActorLocation(FVector(100, 200, 50));
    
    // Act - ターゲット設定
    Camera->SetTargetActor(TargetActor);
    
    // Assert - ターゲット設定確認
    EXPECT_EQ(Camera->GetTargetActor(), TargetActor);
    
    // Act - 追従更新
    for (int32 Frame = 0; Frame < 10; ++Frame)
    {
        Camera->UpdateCamera(1.0f / 60.0f);
    }
    
    // Assert - カメラ位置がターゲットに追従
    FVector CameraLocation = Camera->GetComponentLocation();
    FVector TargetLocation = TargetActor->GetActorLocation();
    
    // 横視点なので、Y座標はFollowDistance分離れている
    EXPECT_NEAR(CameraLocation.X, TargetLocation.X, 50.0f);
    EXPECT_NEAR(CameraLocation.Y, TargetLocation.Y - 500.0f, 50.0f); // Default follow distance
    EXPECT_NEAR(CameraLocation.Z, TargetLocation.Z, 50.0f);
    
    World->DestroyWorld(true);
}
```

#### TC-303-002-02: 滑らかな追従
```cpp
TEST(SideViewCameraTest, SmoothFollowing)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    AActor* TargetActor = World->SpawnActor<AActor>();
    Camera->SetTargetActor(TargetActor);
    
    FVector InitialTargetPos = FVector(0, 0, 0);
    FVector FinalTargetPos = FVector(500, 0, 100);
    TargetActor->SetActorLocation(InitialTargetPos);
    
    TArray<FVector> CameraPositions;
    
    // Act - ターゲットを段階的に移動させながら追従確認
    for (int32 Frame = 0; Frame < 60; ++Frame) // 1秒間
    {
        float Alpha = Frame / 59.0f;
        FVector CurrentTargetPos = FMath::Lerp(InitialTargetPos, FinalTargetPos, Alpha);
        TargetActor->SetActorLocation(CurrentTargetPos);
        
        Camera->UpdateCamera(1.0f / 60.0f);
        CameraPositions.Add(Camera->GetComponentLocation());
    }
    
    // Assert - 滑らかな動きの確認（急激な変化がないこと）
    for (int32 i = 1; i < CameraPositions.Num(); ++i)
    {
        float FrameDistance = FVector::Dist(CameraPositions[i], CameraPositions[i-1]);
        EXPECT_LT(FrameDistance, 50.0f); // 1フレーム間の移動量制限
    }
    
    World->DestroyWorld(true);
}
```

### TC-303-003: 先読み機能テスト

#### TC-303-003-01: 速度ベース先読み
```cpp
TEST(SideViewCameraTest, VelocityBasedLookAhead)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    AActor* TargetActor = World->SpawnActor<AActor>();
    Camera->SetTargetActor(TargetActor);
    
    // 初期位置設定と数フレーム更新
    TargetActor->SetActorLocation(FVector(0, 0, 0));
    for (int32 i = 0; i < 5; ++i)
    {
        Camera->UpdateCamera(1.0f / 60.0f);
    }
    
    FVector InitialCameraPos = Camera->GetComponentLocation();
    
    // Act - ターゲットを高速で右方向に移動
    for (int32 Frame = 0; Frame < 10; ++Frame)
    {
        FVector NewPos = FVector(Frame * 100.0f, 0, 0); // 高速移動
        TargetActor->SetActorLocation(NewPos);
        Camera->UpdateCamera(1.0f / 60.0f);
    }
    
    // Assert - 先読みによりカメラがターゲットより前方に位置
    FVector FinalCameraPos = Camera->GetComponentLocation();
    FVector FinalTargetPos = TargetActor->GetActorLocation();
    
    // カメラのX座標がターゲットより進行方向にオフセットされている
    EXPECT_GT(FinalCameraPos.X, FinalTargetPos.X - Camera->GetFollowDistance());
    
    World->DestroyWorld(true);
}
```

#### TC-303-003-02: 先読み強度調整
```cpp
TEST(SideViewCameraTest, LookAheadStrengthAdjustment)
{
    // Arrange
    auto [Camera1, World] = CreateTestSideViewCamera();
    USideViewCameraComponent* Camera2 = NewObject<USideViewCameraComponent>(World->SpawnActor<AActor>());
    Camera2->InitializeCamera();
    
    AActor* Target1 = World->SpawnActor<AActor>();
    AActor* Target2 = World->SpawnActor<AActor>();
    
    Camera1->SetTargetActor(Target1);
    Camera2->SetTargetActor(Target2);
    
    // 異なる先読み強度設定
    Camera1->SetLookAheadStrength(0.5f);
    Camera2->SetLookAheadStrength(2.0f);
    
    // Act - 同じ動きをするターゲットを設定
    FVector MovementPattern[] = {
        FVector(0, 0, 0),
        FVector(100, 0, 0),
        FVector(200, 0, 0),
        FVector(300, 0, 0)
    };
    
    TArray<FVector> Camera1Positions;
    TArray<FVector> Camera2Positions;
    
    for (const FVector& TargetPos : MovementPattern)
    {
        Target1->SetActorLocation(TargetPos);
        Target2->SetActorLocation(TargetPos);
        
        Camera1->UpdateCamera(1.0f / 60.0f);
        Camera2->UpdateCamera(1.0f / 60.0f);
        
        Camera1Positions.Add(Camera1->GetComponentLocation());
        Camera2Positions.Add(Camera2->GetComponentLocation());
    }
    
    // Assert - 高い先読み強度の方がより前方に位置
    for (int32 i = 1; i < Camera1Positions.Num(); ++i)
    {
        EXPECT_LT(Camera1Positions[i].X, Camera2Positions[i].X);
    }
    
    World->DestroyWorld(true);
}
```

### TC-303-004: 境界制限テスト

#### TC-303-004-01: 境界内制限
```cpp
TEST(SideViewCameraTest, BoundaryLimitation)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    AActor* TargetActor = World->SpawnActor<AActor>();
    Camera->SetTargetActor(TargetActor);
    
    // 境界設定
    FVector MinBounds(-500, -500, -100);
    FVector MaxBounds(500, 500, 100);
    Camera->SetBoundaryLimits(MinBounds, MaxBounds);
    
    // Act - ターゲットを境界外に移動
    FVector OutOfBoundsPosition(1000, 0, 0); // X境界超過
    TargetActor->SetActorLocation(OutOfBoundsPosition);
    
    for (int32 i = 0; i < 30; ++i) // 0.5秒間更新
    {
        Camera->UpdateCamera(1.0f / 60.0f);
    }
    
    // Assert - カメラ位置が境界内に制限されている
    FVector CameraPos = Camera->GetComponentLocation();
    EXPECT_GE(CameraPos.X, MinBounds.X);
    EXPECT_LE(CameraPos.X, MaxBounds.X);
    EXPECT_GE(CameraPos.Y, MinBounds.Y);
    EXPECT_LE(CameraPos.Y, MaxBounds.Y);
    EXPECT_GE(CameraPos.Z, MinBounds.Z);
    EXPECT_LE(CameraPos.Z, MaxBounds.Z);
    
    World->DestroyWorld(true);
}
```

#### TC-303-004-02: 境界クランプ機能
```cpp
TEST(SideViewCameraTest, BoundaryClampFunction)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    
    FVector MinBounds(-100, -200, -50);
    FVector MaxBounds(100, 200, 50);
    Camera->SetBoundaryLimits(MinBounds, MaxBounds);
    
    // Act & Assert - 様々な位置のクランプテスト
    FVector TestPositions[] = {
        FVector(-150, 0, 0),    // X最小値超過
        FVector(150, 0, 0),     // X最大値超過
        FVector(0, -250, 0),    // Y最小値超過
        FVector(0, 250, 0),     // Y最大値超過
        FVector(0, 0, -100),    // Z最小値超過
        FVector(0, 0, 100),     // Z最大値超過
        FVector(50, 100, 25),   // 境界内
    };
    
    FVector ExpectedResults[] = {
        FVector(-100, 0, 0),    // X最小値にクランプ
        FVector(100, 0, 0),     // X最大値にクランプ
        FVector(0, -200, 0),    // Y最小値にクランプ
        FVector(0, 200, 0),     // Y最大値にクランプ
        FVector(0, 0, -50),     // Z最小値にクランプ
        FVector(0, 0, 50),      // Z最大値にクランプ
        FVector(50, 100, 25),   // そのまま
    };
    
    for (int32 i = 0; i < sizeof(TestPositions) / sizeof(FVector); ++i)
    {
        FVector ClampedPos = Camera->ClampToBoundary(TestPositions[i]);
        EXPECT_VECTOR_NEAR(ClampedPos, ExpectedResults[i], 1.0f);
    }
    
    World->DestroyWorld(true);
}
```

### TC-303-005: 画面内保持テスト

#### TC-303-005-01: 画面内保持機能
```cpp
TEST(SideViewCameraTest, ScreenKeepFunction)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    AActor* TargetActor = World->SpawnActor<AActor>();
    Camera->SetTargetActor(TargetActor);
    
    // 画面マージン設定
    auto& Settings = const_cast<FSideViewCameraSettings&>(Camera->GetSettings());
    Settings.ScreenMargin = 0.1f; // 10%マージン
    
    // Act - ターゲットを様々な位置に移動してカメラ位置確認
    FVector TestTargetPositions[] = {
        FVector(0, 0, 0),
        FVector(1000, 0, 0),    // 遠距離
        FVector(-1000, 0, 0),   // 遠距離反対
        FVector(0, 0, 500),     // 高高度
        FVector(0, 0, -200),    // 低高度
    };
    
    for (const FVector& TargetPos : TestTargetPositions)
    {
        TargetActor->SetActorLocation(TargetPos);
        
        // 数フレーム更新してカメラ位置安定化
        for (int32 i = 0; i < 30; ++i)
        {
            Camera->UpdateCamera(1.0f / 60.0f);
        }
        
        FVector CameraPos = Camera->GetComponentLocation();
        
        // Assert - ターゲットがカメラから適切な距離内に収まっている
        float DistanceToTarget = FVector::Dist(CameraPos, TargetPos);
        EXPECT_GE(DistanceToTarget, Settings.MinFollowDistance);
        EXPECT_LE(DistanceToTarget, Settings.MaxFollowDistance);
    }
    
    World->DestroyWorld(true);
}
```

## 統合テストケース

### TC-303-101: BirdCharacter統合テスト

#### TC-303-101-01: BirdCharacterとの統合
```cpp
TEST(SideViewCameraIntegrationTest, BirdCharacterIntegration)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* Bird = World->SpawnActor<ABirdCharacter>();
    
    // Act
    USideViewCameraComponent* SideCamera = Bird->GetSideViewCamera();
    
    // Assert
    EXPECT_TRUE(SideCamera != nullptr);
    EXPECT_EQ(SideCamera->GetTargetActor(), Bird);
    
    World->DestroyWorld(true);
}
```

#### TC-303-101-02: 飛行動作との連携
```cpp
TEST(SideViewCameraIntegrationTest, FlightMotionTracking)
{
    // Arrange
    auto [Bird, World] = CreateTestBirdCharacter();
    USideViewCameraComponent* SideCamera = Bird->GetSideViewCamera();
    
    // Act - 鳥を複雑な飛行パターンで移動
    FVector FlightPath[] = {
        FVector(0, 0, 100),
        FVector(200, 0, 200),
        FVector(500, 0, 150),
        FVector(800, 0, 300),
        FVector(1000, 0, 100),
    };
    
    TArray<FVector> CameraTrajectory;
    
    for (int32 i = 0; i < sizeof(FlightPath) / sizeof(FVector); ++i)
    {
        // 鳥を目標位置に向けて移動
        FVector CurrentPos = Bird->GetActorLocation();
        FVector TargetPos = FlightPath[i];
        
        for (int32 Frame = 0; Frame < 60; ++Frame) // 1秒かけて移動
        {
            float Alpha = Frame / 59.0f;
            FVector NewPos = FMath::Lerp(CurrentPos, TargetPos, Alpha);
            Bird->SetActorLocation(NewPos);
            Bird->Tick(1.0f / 60.0f);
            
            CameraTrajectory.Add(SideCamera->GetComponentLocation());
        }
    }
    
    // Assert - カメラが鳥の飛行に適切に追従
    for (int32 i = 1; i < CameraTrajectory.Num(); ++i)
    {
        float CameraMovement = FVector::Dist(CameraTrajectory[i], CameraTrajectory[i-1]);
        EXPECT_LT(CameraMovement, 100.0f); // 滑らかな追従
    }
    
    World->DestroyWorld(true);
}
```

### TC-303-102: SplitScreenManager統合テスト

#### TC-303-102-01: 分割スクリーン統合
```cpp
TEST(SideViewCameraIntegrationTest, SplitScreenIntegration)
{
    // Arrange
    auto [Bird, World] = CreateTestBirdCharacter();
    USplitScreenManager* SplitScreen = CreateTestSplitScreenManager(World);
    USideViewCameraComponent* SideCamera = Bird->GetSideViewCamera();
    
    // Act - サイドビューカメラをビューポート1（下部/右側）に割り当て
    bool bAssignSuccess = SplitScreen->AssignCamera(1, SideCamera->GetCameraComponent());
    
    // Assert
    EXPECT_TRUE(bAssignSuccess);
    EXPECT_EQ(SplitScreen->GetAssignedCamera(1), SideCamera->GetCameraComponent());
    
    World->DestroyWorld(true);
}
```

## パフォーマンステストケース

### TC-303-201: 追従処理性能テスト

#### TC-303-201-01: 更新処理負荷測定
```cpp
TEST(SideViewCameraPerformanceTest, UpdatePerformance)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    AActor* TargetActor = World->SpawnActor<AActor>();
    Camera->SetTargetActor(TargetActor);
    
    FPerformanceProfiler Profiler;
    const int32 TestFrames = 3600; // 60秒分 at 60fps
    
    // Act - 継続的な追従処理
    Profiler.StartProfiling();
    
    for (int32 Frame = 0; Frame < TestFrames; ++Frame)
    {
        // ランダムな位置にターゲット移動
        FVector RandomPos = FVector(
            FMath::RandRange(-1000.0f, 1000.0f),
            FMath::RandRange(-1000.0f, 1000.0f),
            FMath::RandRange(-200.0f, 500.0f)
        );
        TargetActor->SetActorLocation(RandomPos);
        
        Camera->UpdateCamera(1.0f / 60.0f);
    }
    
    Profiler.StopProfiling();
    
    // Assert - 1フレームあたりの処理時間
    float AverageFrameTime = Profiler.GetAverageFrameTime();
    EXPECT_LT(AverageFrameTime, 0.05f); // 0.05ms以下
    
    World->DestroyWorld(true);
}
```

#### TC-303-201-02: メモリ使用量テスト
```cpp
TEST(SideViewCameraPerformanceTest, MemoryUsage)
{
    // Arrange
    size_t BaselineMemory = GetCurrentMemoryUsage();
    
    // Act - 複数カメラの作成・破棄
    TArray<USideViewCameraComponent*> Cameras;
    
    for (int32 i = 0; i < 50; ++i)
    {
        UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
        auto Camera = CreateTestSideViewCamera(World);
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
    size_t PerCameraMemory = (PeakMemory - BaselineMemory) / 50;
    EXPECT_LT(PerCameraMemory, 5 * 1024 * 1024); // 5MB以下
    EXPECT_LE(FinalMemory, BaselineMemory * 1.1f); // メモリリーク検証
}
```

## 視覚テストケース

### TC-303-301: 追従品質テスト

#### 手動テスト項目
1. **追従スムーズネス**: カメラが鳥を滑らかに追従するか
2. **先読み自然さ**: 先読み機能が自然で予測可能か
3. **画面内保持**: 鳥が常に適切に画面内に収まるか
4. **境界処理**: 境界到達時の処理が自然か

#### 自動化テスト
```cpp
TEST(SideViewCameraVisualTest, FollowingSmoothness)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    AActor* TargetActor = World->SpawnActor<AActor>();
    Camera->SetTargetActor(TargetActor);
    
    TArray<FVector> CameraPositions;
    TArray<FVector> TargetPositions;
    
    // Act - 複雑な飛行パターンをシミュレート
    for (int32 Frame = 0; Frame < 300; ++Frame) // 5秒間
    {
        float Time = Frame / 60.0f;
        FVector TargetPos = FVector(
            Time * 200.0f + FMath::Sin(Time * 2.0f) * 100,
            0,
            FMath::Cos(Time * 1.5f) * 150 + 200
        );
        
        TargetActor->SetActorLocation(TargetPos);
        Camera->UpdateCamera(1.0f / 60.0f);
        
        CameraPositions.Add(Camera->GetComponentLocation());
        TargetPositions.Add(TargetPos);
    }
    
    // Assert - 追従品質と滑らかさ
    for (int32 i = 1; i < CameraPositions.Num(); ++i)
    {
        // 滑らかな動き（急激な変化なし）
        float FrameDistance = FVector::Dist(CameraPositions[i], CameraPositions[i-1]);
        EXPECT_LT(FrameDistance, 30.0f);
        
        // 適切な距離維持
        float DistanceToTarget = FVector::Dist(CameraPositions[i], TargetPositions[i]);
        EXPECT_GE(DistanceToTarget, 200.0f); // 最小距離
        EXPECT_LE(DistanceToTarget, 1000.0f); // 最大距離
    }
    
    World->DestroyWorld(true);
}
```

## エラーハンドリングテストケース

### TC-303-401: 異常系処理テスト

#### TC-303-401-01: ターゲット喪失時処理
```cpp
TEST(SideViewCameraErrorTest, TargetLossHandling)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    AActor* TargetActor = World->SpawnActor<AActor>();
    Camera->SetTargetActor(TargetActor);
    
    // 数フレーム正常動作
    for (int32 i = 0; i < 10; ++i)
    {
        Camera->UpdateCamera(1.0f / 60.0f);
    }
    
    FVector LastValidPosition = Camera->GetComponentLocation();
    
    // Act - ターゲットを破棄
    TargetActor->Destroy();
    TargetActor = nullptr;
    
    // Assert - ターゲット喪失後も安全に動作
    EXPECT_NO_THROW({
        for (int32 i = 0; i < 60; ++i)
        {
            Camera->UpdateCamera(1.0f / 60.0f);
        }
    });
    
    // カメラ位置が大きく変化しない
    FVector CurrentPosition = Camera->GetComponentLocation();
    float PositionDrift = FVector::Dist(LastValidPosition, CurrentPosition);
    EXPECT_LT(PositionDrift, 50.0f);
    
    World->DestroyWorld(true);
}
```

#### TC-303-401-02: 無効な設定値処理
```cpp
TEST(SideViewCameraErrorTest, InvalidSettingsHandling)
{
    // Arrange
    auto [Camera, World] = CreateTestSideViewCamera();
    
    // Act & Assert - 無効な追従距離
    EXPECT_NO_THROW(Camera->SetFollowDistance(-100.0f)); // 負の値
    EXPECT_NO_THROW(Camera->SetFollowDistance(10000.0f)); // 極大値
    
    // 値が適切な範囲にクランプされることを確認
    float ClampedDistance = Camera->GetFollowDistance();
    EXPECT_GE(ClampedDistance, 50.0f);   // 最小値
    EXPECT_LE(ClampedDistance, 2000.0f); // 最大値
    
    // Act & Assert - 無効な先読み強度
    EXPECT_NO_THROW(Camera->SetLookAheadStrength(-1.0f)); // 負の値
    EXPECT_NO_THROW(Camera->SetLookAheadStrength(10.0f));  // 極大値
    
    float ClampedStrength = Camera->GetLookAheadStrength();
    EXPECT_GE(ClampedStrength, 0.0f); // 最小値
    EXPECT_LE(ClampedStrength, 5.0f); // 最大値
    
    World->DestroyWorld(true);
}
```

## テスト実行手順

### 自動テスト実行
```bash
# 全単体テスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.SideViewCamera"

# 特定カテゴリテスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.SideViewCamera.Component"
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.SideViewCamera.Performance"

# 統合テスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.SideViewCamera.Integration"
```

### 手動テスト実行
1. BirdCharacterでテストマップ読み込み
2. サイドビューカメラモードに切り替え
3. 様々な飛行パターンでの追従テスト
4. 境界条件での動作確認
5. 分割スクリーン表示での確認

## 成功基準

### 自動テスト
- [ ] 全単体テストケースが通過 (100%)
- [ ] 統合テストケースが通過 (100%)
- [ ] パフォーマンステストが要件クリア (100%)
- [ ] エラーハンドリングテストが通過 (100%)

### 手動テスト
- [ ] カメラ追従が滑らかで自然
- [ ] 先読み機能が適切に動作
- [ ] 鳥が常に画面内に適切に表示
- [ ] 境界処理が自然で滑らか

### 品質基準
- [ ] 追従遅延1フレーム以内
- [ ] CPU負荷0.05ms以下
- [ ] メモリ使用量5MB以下
- [ ] コードカバレッジ90%以上

## テストデータ・ヘルパー

### テスト用ヘルパー関数
```cpp
// Test helper functions
std::pair<USideViewCameraComponent*, UWorld*> CreateTestSideViewCamera();
std::pair<ABirdCharacter*, UWorld*> CreateTestBirdCharacter();
USplitScreenManager* CreateTestSplitScreenManager(UWorld* World);
size_t GetCurrentMemoryUsage();

// Expectation helpers
#define EXPECT_VECTOR_NEAR(actual, expected, tolerance) \
    EXPECT_NEAR(actual.X, expected.X, tolerance); \
    EXPECT_NEAR(actual.Y, expected.Y, tolerance); \
    EXPECT_NEAR(actual.Z, expected.Z, tolerance)
```

### テストデータ
- 標準追従距離: 500.0f units
- 先読み強度: 1.0f (デフォルト)
- 境界範囲: ±5000 units (X,Y), ±1000 units (Z)
- テスト解像度: 1920x1080, 2560x1440
- フレームレート: 60fps基準

このテストスイートにより、TASK-303の完全な検証が可能になります。