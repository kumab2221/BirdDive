# TASK-301: 分割スクリーンシステム実装 - テストケース定義

## テストケース概要

### テスト戦略
- **単体テスト**: 分割スクリーンマネージャーの個別機能確認
- **統合テスト**: カメラシステム・レンダリングとの連携確認
- **パフォーマンステスト**: レンダリング負荷・フレームレート測定
- **視覚テスト**: 表示品質・分割精度の確認

### テスト環境
- **ハードウェア**: NVIDIA RTX系、AMD RX系GPU対応
- **解像度**: 1920x1080, 2560x1440, 3840x2160
- **プラットフォーム**: Windows 11
- **UEバージョン**: 5.6

## 単体テストケース

### TC-301-001: SplitScreenManager基盤テスト

#### TC-301-001-01: コンポーネント初期化
```cpp
TEST(SplitScreenManagerTest, ComponentInitialization)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    AActor* TestActor = World->SpawnActor<AActor>();
    USplitScreenManager* Manager = TestActor->CreateDefaultSubobject<USplitScreenManager>(TEXT("SplitScreenManager"));
    
    // Act
    TestActor->BeginPlay();
    
    // Assert
    EXPECT_TRUE(Manager != nullptr);
    EXPECT_EQ(Manager->GetViewportCount(), 2);
    EXPECT_EQ(Manager->GetSplitRatio(), 0.5f);
    EXPECT_EQ(Manager->GetSplitType(), ESplitScreenType::Horizontal);
    
    World->DestroyWorld(true);
}
```

#### TC-301-001-02: ライフサイクル管理
```cpp
TEST(SplitScreenManagerTest, LifecycleManagement)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    AActor* TestActor = World->SpawnActor<AActor>();
    USplitScreenManager* Manager = TestActor->CreateDefaultSubobject<USplitScreenManager>(TEXT("SplitScreenManager"));
    
    // Act - BeginPlay
    TestActor->BeginPlay();
    bool bInitialized = Manager->IsInitialized();
    
    // Act - EndPlay
    TestActor->EndPlay(EEndPlayReason::Destroyed);
    bool bCleanedUp = Manager->IsCleanedUp();
    
    // Assert
    EXPECT_TRUE(bInitialized);
    EXPECT_TRUE(bCleanedUp);
    
    World->DestroyWorld(true);
}
```

### TC-301-002: 分割比率管理テスト

#### TC-301-002-01: 分割比率設定
```cpp
TEST(SplitScreenManagerTest, SplitRatioSetting)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    
    // Act & Assert - Valid range
    EXPECT_TRUE(Manager->SetSplitRatio(0.3f));
    EXPECT_FLOAT_EQ(Manager->GetSplitRatio(), 0.3f);
    
    EXPECT_TRUE(Manager->SetSplitRatio(0.7f));
    EXPECT_FLOAT_EQ(Manager->GetSplitRatio(), 0.7f);
    
    // Act & Assert - Invalid range clamping
    EXPECT_TRUE(Manager->SetSplitRatio(0.1f)); // Below minimum
    EXPECT_FLOAT_EQ(Manager->GetSplitRatio(), 0.2f); // Clamped
    
    EXPECT_TRUE(Manager->SetSplitRatio(0.9f)); // Above maximum  
    EXPECT_FLOAT_EQ(Manager->GetSplitRatio(), 0.8f); // Clamped
    
    World->DestroyWorld(true);
}
```

#### TC-301-002-02: 分割タイプ切り替え
```cpp
TEST(SplitScreenManagerTest, SplitTypeSwitch)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    
    // Act - Horizontal to Vertical
    Manager->SetSplitType(ESplitScreenType::Vertical);
    
    // Assert
    EXPECT_EQ(Manager->GetSplitType(), ESplitScreenType::Vertical);
    FViewportInfo Viewport0 = Manager->GetViewportInfo(0);
    FViewportInfo Viewport1 = Manager->GetViewportInfo(1);
    
    // Vertical split should have viewports side by side
    EXPECT_LT(Viewport0.ViewportRect.Max.X, Viewport1.ViewportRect.Min.X);
    
    // Act - Vertical to Horizontal
    Manager->SetSplitType(ESplitScreenType::Horizontal);
    
    // Assert
    EXPECT_EQ(Manager->GetSplitType(), ESplitScreenType::Horizontal);
    Viewport0 = Manager->GetViewportInfo(0);
    Viewport1 = Manager->GetViewportInfo(1);
    
    // Horizontal split should have viewports stacked
    EXPECT_LT(Viewport0.ViewportRect.Max.Y, Viewport1.ViewportRect.Min.Y);
    
    World->DestroyWorld(true);
}
```

### TC-301-003: ビューポート管理テスト

#### TC-301-003-01: ビューポート分割精度
```cpp
TEST(SplitScreenManagerTest, ViewportSplitPrecision)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    FIntPoint ScreenSize(1920, 1080);
    Manager->SetScreenSize(ScreenSize);
    Manager->SetSplitRatio(0.6f);
    
    // Act - Horizontal split
    Manager->SetSplitType(ESplitScreenType::Horizontal);
    FViewportInfo Viewport0 = Manager->GetViewportInfo(0);
    FViewportInfo Viewport1 = Manager->GetViewportInfo(1);
    
    // Assert - Precision within ±1 pixel
    int32 ExpectedSplitY = static_cast<int32>(ScreenSize.Y * 0.6f);
    EXPECT_NEAR(Viewport0.ViewportRect.Max.Y, ExpectedSplitY, 1);
    EXPECT_NEAR(Viewport1.ViewportRect.Min.Y, ExpectedSplitY, 1);
    
    // Assert - No gap or overlap
    EXPECT_LE(abs(Viewport0.ViewportRect.Max.Y - Viewport1.ViewportRect.Min.Y), 1);
    
    World->DestroyWorld(true);
}
```

#### TC-301-003-02: アスペクト比計算
```cpp
TEST(SplitScreenManagerTest, AspectRatioCalculation)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    FIntPoint ScreenSize(1920, 1080); // 16:9 aspect ratio
    Manager->SetScreenSize(ScreenSize);
    
    // Act - Horizontal split with 50:50
    Manager->SetSplitRatio(0.5f);
    Manager->SetSplitType(ESplitScreenType::Horizontal);
    
    // Assert - Each viewport should maintain proper aspect ratio
    FViewportInfo Viewport0 = Manager->GetViewportInfo(0);
    FViewportInfo Viewport1 = Manager->GetViewportInfo(1);
    
    float ExpectedAspectRatio = (float)ScreenSize.X / (float)(ScreenSize.Y * 0.5f);
    EXPECT_NEAR(Viewport0.AspectRatio, ExpectedAspectRatio, 0.01f);
    EXPECT_NEAR(Viewport1.AspectRatio, ExpectedAspectRatio, 0.01f);
    
    World->DestroyWorld(true);
}
```

### TC-301-004: カメラ管理テスト

#### TC-301-004-01: カメラ割り当て
```cpp
TEST(SplitScreenManagerTest, CameraAssignment)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    ACameraActor* Camera1 = World->SpawnActor<ACameraActor>();
    ACameraActor* Camera2 = World->SpawnActor<ACameraActor>();
    
    // Act
    bool bAssigned1 = Manager->AssignCamera(0, Camera1);
    bool bAssigned2 = Manager->AssignCamera(1, Camera2);
    
    // Assert
    EXPECT_TRUE(bAssigned1);
    EXPECT_TRUE(bAssigned2);
    EXPECT_EQ(Manager->GetAssignedCamera(0), Camera1);
    EXPECT_EQ(Manager->GetAssignedCamera(1), Camera2);
    
    // Act - Invalid viewport index
    bool bInvalidAssignment = Manager->AssignCamera(2, Camera1);
    
    // Assert
    EXPECT_FALSE(bInvalidAssignment);
    
    World->DestroyWorld(true);
}
```

#### TC-301-004-02: カメラ再割り当て
```cpp
TEST(SplitScreenManagerTest, CameraReassignment)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    ACameraActor* Camera1 = World->SpawnActor<ACameraActor>();
    ACameraActor* Camera2 = World->SpawnActor<ACameraActor>();
    ACameraActor* Camera3 = World->SpawnActor<ACameraActor>();
    
    Manager->AssignCamera(0, Camera1);
    Manager->AssignCamera(1, Camera2);
    
    // Act - Reassign camera
    bool bReassigned = Manager->AssignCamera(0, Camera3);
    
    // Assert
    EXPECT_TRUE(bReassigned);
    EXPECT_EQ(Manager->GetAssignedCamera(0), Camera3);
    EXPECT_EQ(Manager->GetAssignedCamera(1), Camera2); // Unchanged
    
    World->DestroyWorld(true);
}
```

### TC-301-005: レンダーターゲット管理テスト

#### TC-301-005-01: レンダーターゲット作成
```cpp
TEST(SplitScreenManagerTest, RenderTargetCreation)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    
    // Act
    Manager->InitializeSplitScreen();
    
    // Assert
    FViewportInfo Viewport0 = Manager->GetViewportInfo(0);
    FViewportInfo Viewport1 = Manager->GetViewportInfo(1);
    
    EXPECT_TRUE(Viewport0.RenderTarget != nullptr);
    EXPECT_TRUE(Viewport1.RenderTarget != nullptr);
    EXPECT_NE(Viewport0.RenderTarget, Viewport1.RenderTarget); // Separate targets
    
    World->DestroyWorld(true);
}
```

#### TC-301-005-02: メモリ管理
```cpp
TEST(SplitScreenManagerTest, MemoryManagement)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    
    // Act - Initialize
    Manager->InitializeSplitScreen();
    
    // Capture initial memory state
    size_t InitialMemory = GetTextureMemoryUsage();
    
    // Act - Cleanup
    Manager->Shutdown();
    
    // Assert - Memory should be cleaned up
    size_t FinalMemory = GetTextureMemoryUsage();
    EXPECT_LE(FinalMemory, InitialMemory); // No memory leaks
    
    World->DestroyWorld(true);
}
```

## 統合テストケース

### TC-301-101: GameModeとの統合テスト

#### TC-301-101-01: GameMode統合
```cpp
TEST(SplitScreenManagerIntegrationTest, GameModeIntegration)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdDiveGameMode* GameMode = World->SpawnActor<ABirdDiveGameMode>();
    World->SetGameMode(GameMode);
    
    // Act
    USplitScreenManager* Manager = GameMode->GetSplitScreenManager();
    
    // Assert
    EXPECT_TRUE(Manager != nullptr);
    EXPECT_TRUE(Manager->IsInitialized());
    
    World->DestroyWorld(true);
}
```

### TC-301-102: PlayerControllerとの統合テスト

#### TC-301-102-01: PlayerController連携
```cpp
TEST(SplitScreenManagerIntegrationTest, PlayerControllerIntegration)
{
    // Arrange
    auto [Manager, World] = CreateTestManagerWithPlayerController();
    ABirdDivePlayerController* PC = GetTestPlayerController(World);
    
    // Act - UI command to change split ratio
    PC->ChangeSplitRatio(0.7f);
    
    // Assert
    EXPECT_FLOAT_EQ(Manager->GetSplitRatio(), 0.7f);
    
    World->DestroyWorld(true);
}
```

## パフォーマンステストケース

### TC-301-201: レンダリング負荷テスト

#### TC-301-201-01: フレームレート測定
```cpp
TEST(SplitScreenManagerPerformanceTest, FrameRateMeasurement)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    Manager->InitializeSplitScreen();
    
    FFrameRateCounter FrameCounter;
    const int32 TestDurationFrames = 300; // 5 seconds at 60fps
    
    // Act - Simulate rendering for test duration
    for (int32 Frame = 0; Frame < TestDurationFrames; ++Frame)
    {
        float DeltaTime = 1.0f / 60.0f; // Target 60fps
        Manager->UpdateSplitScreen(DeltaTime);
        FrameCounter.RecordFrame(DeltaTime);
    }
    
    // Assert
    float AverageFrameRate = FrameCounter.GetAverageFrameRate();
    EXPECT_GE(AverageFrameRate, 58.0f); // Allow 2fps tolerance
    EXPECT_LE(FrameCounter.GetWorstFrameTime(), 20.0f); // Max 20ms frame time
    
    World->DestroyWorld(true);
}
```

#### TC-301-201-02: GPU使用率測定
```cpp
TEST(SplitScreenManagerPerformanceTest, GPUUtilizationTest)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    Manager->InitializeSplitScreen();
    
    FGPUProfiler Profiler;
    
    // Act - Measure GPU usage during split screen rendering
    Profiler.StartProfiling();
    
    for (int32 Frame = 0; Frame < 120; ++Frame) // 2 seconds
    {
        Manager->UpdateSplitScreen(1.0f / 60.0f);
    }
    
    Profiler.StopProfiling();
    
    // Assert
    float GPUUtilization = Profiler.GetAverageGPUUtilization();
    EXPECT_LE(GPUUtilization, 70.0f); // Requirements: <70% GPU usage
    
    World->DestroyWorld(true);
}
```

### TC-301-202: メモリ使用量テスト

#### TC-301-202-01: レンダーターゲットメモリ
```cpp
TEST(SplitScreenManagerPerformanceTest, RenderTargetMemoryUsage)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    size_t BaselineMemory = GetTextureMemoryUsage();
    
    // Act
    Manager->InitializeSplitScreen();
    size_t AfterInitMemory = GetTextureMemoryUsage();
    
    // Assert
    size_t AdditionalMemory = AfterInitMemory - BaselineMemory;
    
    // Expected memory usage for 1920x1080 split screen
    size_t ExpectedMemory = CalculateExpectedRenderTargetMemory(1920, 1080);
    EXPECT_LE(AdditionalMemory, ExpectedMemory * 1.2f); // 20% tolerance
    
    World->DestroyWorld(true);
}
```

## 視覚テストケース

### TC-301-301: 表示品質テスト

#### 手動テスト項目
1. **分割境界の明確さ**: 境界線がクリアに表示されるか
2. **色精度**: 両ビューポートで同じ色が同じに表示されるか  
3. **解像度維持**: 各ビューポートで適切な解像度が保たれるか
4. **アンチエイリアシング**: AA効果が両方で適用されるか

#### 自動化テスト
```cpp
TEST(SplitScreenManagerVisualTest, SplitBoundaryAccuracy)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    Manager->SetSplitRatio(0.6f);
    
    // Act - Render test frame
    UTextureRenderTarget2D* TestTarget = CaptureFrame(Manager);
    
    // Assert - Check split boundary location
    FIntPoint SplitPoint = FindSplitBoundary(TestTarget);
    FIntPoint ExpectedSplit = CalculateExpectedSplitPoint(0.6f, TestTarget->SizeX, TestTarget->SizeY);
    
    EXPECT_NEAR(SplitPoint.X, ExpectedSplit.X, 1);
    EXPECT_NEAR(SplitPoint.Y, ExpectedSplit.Y, 1);
    
    World->DestroyWorld(true);
}
```

## エラーハンドリングテストケース

### TC-301-401: 異常系処理テスト

#### TC-301-401-01: カメラ削除時処理
```cpp
TEST(SplitScreenManagerErrorTest, CameraDeletionHandling)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    ACameraActor* Camera = World->SpawnActor<ACameraActor>();
    Manager->AssignCamera(0, Camera);
    
    // Act - Delete assigned camera
    World->DestroyActor(Camera);
    
    // Assert - Should handle gracefully without crash
    EXPECT_NO_THROW(Manager->UpdateSplitScreen(1.0f / 60.0f));
    EXPECT_EQ(Manager->GetAssignedCamera(0), nullptr);
    
    World->DestroyWorld(true);
}
```

#### TC-301-401-02: 無効解像度処理
```cpp
TEST(SplitScreenManagerErrorTest, InvalidResolutionHandling)
{
    // Arrange
    auto [Manager, World] = CreateTestManager();
    
    // Act & Assert - Invalid resolutions
    EXPECT_NO_THROW(Manager->SetScreenSize(FIntPoint(0, 0)));
    EXPECT_NO_THROW(Manager->SetScreenSize(FIntPoint(-100, -100)));
    
    // Should clamp to minimum valid size
    FIntPoint ActualSize = Manager->GetScreenSize();
    EXPECT_GT(ActualSize.X, 0);
    EXPECT_GT(ActualSize.Y, 0);
    
    World->DestroyWorld(true);
}
```

## テスト実行手順

### 自動テスト実行
```bash
# 全単体テスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.SplitScreen"

# 特定カテゴリテスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.SplitScreen.Manager"
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.SplitScreen.Performance"

# 視覚テスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.SplitScreen.Visual"
```

### 手動テスト実行
1. テストマップ読み込み
2. SplitScreenManagerを持つGameMode設定
3. PIE開始
4. 分割比率・タイプの動的変更確認
5. 複数解像度での動作確認

## 成功基準

### 自動テスト
- [ ] 全単体テストケースが通過 (100%)
- [ ] 統合テストケースが通過 (100%)
- [ ] パフォーマンステストが要件クリア (100%)
- [ ] エラーハンドリングテストが通過 (100%)

### 手動テスト
- [ ] 視覚品質が要件を満たす
- [ ] UI操作が直感的
- [ ] 複数解像度で正常動作
- [ ] フレームレート要件クリア

### 品質基準
- [ ] クラッシュ・フリーズなし
- [ ] メモリリークなし  
- [ ] GPU使用率70%以下
- [ ] コードカバレッジ90%以上

## テストデータ・ヘルパー

### テスト用ヘルパー関数
```cpp
// Test helper functions
std::pair<USplitScreenManager*, UWorld*> CreateTestManager();
ABirdDivePlayerController* GetTestPlayerController(UWorld* World);
size_t GetTextureMemoryUsage();
FIntPoint FindSplitBoundary(UTextureRenderTarget2D* RenderTarget);
size_t CalculateExpectedRenderTargetMemory(int32 Width, int32 Height);
```

### テストデータ
- 標準解像度: 1920x1080, 2560x1440, 3840x2160
- テスト分割比率: 0.2, 0.3, 0.5, 0.7, 0.8
- フレームレート目標: 60fps, 30fps (最低要件)

このテストスイートにより、TASK-301の完全な検証が可能になります。