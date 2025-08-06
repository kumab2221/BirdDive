# TASK-201: 鳥キャラクター実装 - テストケース

## テスト戦略

この包括的なテストスイートは、鳥キャラクターの機能性、パフォーマンス、ユーザビリティを保証します。Enhanced Input System、物理統合、状態管理、着地システムの全側面をカバーします。

## テストカテゴリ

### 1. ABirdCharacter 基本機能テスト

#### TEST-BC-001: キャラクター初期化テスト
```cpp
UTEST(BirdCharacter, Initialization)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    
    // Assert
    TestNotNull(TEXT("BirdCharacter should be created"), BirdCharacter);
    TestNotNull(TEXT("WindForceComponent should be initialized"), BirdCharacter->GetWindForceComponent());
    TestNotNull(TEXT("BirdMovementComponent should be initialized"), BirdCharacter->GetBirdMovementComponent());
    TestEqual(TEXT("Initial flight state should be Idle"), BirdCharacter->GetCurrentFlightState(), EFlightState::Idle);
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

#### TEST-BC-002: Enhanced Input System統合テスト
```cpp
UTEST(BirdCharacter, EnhancedInputIntegration)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    UInputComponent* InputComponent = NewObject<UInputComponent>();
    
    // Act
    BirdCharacter->SetupPlayerInputComponent(InputComponent);
    
    // Assert
    TestNotNull(TEXT("DefaultMappingContext should be set"), BirdCharacter->GetDefaultMappingContext());
    TestNotNull(TEXT("FlyAction should be set"), BirdCharacter->GetFlyAction());
    TestNotNull(TEXT("LookAction should be set"), BirdCharacter->GetLookAction());
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

#### TEST-BC-003: 飛行入力処理テスト
```cpp
UTEST(BirdCharacter, FlightInputHandling)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    FInputActionValue TestInputValue(FVector2D(1.0f, 0.5f));
    FVector InitialVelocity = BirdCharacter->GetVelocity();
    
    // Act
    BirdCharacter->HandleFlyInput(TestInputValue);
    BirdCharacter->Tick(0.016f); // 60FPS
    
    // Assert
    FVector NewVelocity = BirdCharacter->GetVelocity();
    TestTrue(TEXT("Velocity should change after flight input"), 
            !NewVelocity.Equals(InitialVelocity, 0.1f));
    TestTrue(TEXT("Horizontal velocity should increase"), 
            FMath::Abs(NewVelocity.X) > FMath::Abs(InitialVelocity.X));
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

#### TEST-BC-004: 視点入力処理テスト
```cpp
UTEST(BirdCharacter, LookInputHandling)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    FInputActionValue TestLookValue(FVector2D(45.0f, 30.0f));
    FRotator InitialRotation = BirdCharacter->GetActorRotation();
    
    // Act
    BirdCharacter->HandleLookInput(TestLookValue);
    BirdCharacter->Tick(0.016f);
    
    // Assert
    FRotator NewRotation = BirdCharacter->GetActorRotation();
    TestTrue(TEXT("Rotation should change after look input"),
            !NewRotation.Equals(InitialRotation, 0.1f));
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

### 2. UFlightStateManager テスト

#### TEST-FSM-001: 基本状態遷移テスト
```cpp
UTEST(FlightStateManager, BasicStateTransitions)
{
    // Arrange
    UFlightStateManager* StateManager = NewObject<UFlightStateManager>();
    TestEqual(TEXT("Initial state should be Idle"), StateManager->GetCurrentState(), EFlightState::Idle);
    
    // Act & Assert - Valid transitions
    TestTrue(TEXT("Should transition from Idle to Flying"), 
            StateManager->TransitionToState(EFlightState::Flying));
    TestEqual(TEXT("Current state should be Flying"), 
            StateManager->GetCurrentState(), EFlightState::Flying);
    
    TestTrue(TEXT("Should transition from Flying to Gliding"), 
            StateManager->TransitionToState(EFlightState::Gliding));
    TestEqual(TEXT("Current state should be Gliding"), 
            StateManager->GetCurrentState(), EFlightState::Gliding);
    
    TestTrue(TEXT("Should transition from Gliding to Landing"), 
            StateManager->TransitionToState(EFlightState::Landing));
    TestEqual(TEXT("Current state should be Landing"), 
            StateManager->GetCurrentState(), EFlightState::Landing);
    
    TestTrue(TEXT("Should transition from Landing to Landed"), 
            StateManager->TransitionToState(EFlightState::Landed));
    TestEqual(TEXT("Current state should be Landed"), 
            StateManager->GetCurrentState(), EFlightState::Landed);
}
```

#### TEST-FSM-002: 無効な状態遷移拒否テスト
```cpp
UTEST(FlightStateManager, InvalidStateTransitionRejection)
{
    // Arrange
    UFlightStateManager* StateManager = NewObject<UFlightStateManager>();
    
    // Act & Assert - Invalid transitions should be rejected
    TestFalse(TEXT("Should not transition from Idle directly to Landed"), 
             StateManager->TransitionToState(EFlightState::Landed));
    TestEqual(TEXT("State should remain Idle after invalid transition"), 
             StateManager->GetCurrentState(), EFlightState::Idle);
    
    // Set to Flying state
    StateManager->TransitionToState(EFlightState::Flying);
    
    TestFalse(TEXT("Should not transition from Flying directly to Landed"), 
             StateManager->TransitionToState(EFlightState::Landed));
    TestEqual(TEXT("State should remain Flying after invalid transition"), 
             StateManager->GetCurrentState(), EFlightState::Flying);
}
```

#### TEST-FSM-003: 状態変更イベントテスト
```cpp
UTEST(FlightStateManager, StateChangeEvents)
{
    // Arrange
    UFlightStateManager* StateManager = NewObject<UFlightStateManager>();
    bool bEventFired = false;
    EFlightState EventOldState, EventNewState;
    
    StateManager->OnFlightStateChanged.AddLambda([&](EFlightState OldState, EFlightState NewState) {
        bEventFired = true;
        EventOldState = OldState;
        EventNewState = NewState;
    });
    
    // Act
    StateManager->TransitionToState(EFlightState::Flying);
    
    // Assert
    TestTrue(TEXT("State change event should fire"), bEventFired);
    TestEqual(TEXT("Event old state should be Idle"), EventOldState, EFlightState::Idle);
    TestEqual(TEXT("Event new state should be Flying"), EventNewState, EFlightState::Flying);
}
```

### 3. ULandingDetectionComponent テスト

#### TEST-LDC-001: 地面検出テスト
```cpp
UTEST(LandingDetectionComponent, GroundDetection)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    AActor* TestActor = TestWorld->SpawnActor<AActor>();
    ULandingDetectionComponent* LandingComponent = NewObject<ULandingDetectionComponent>(TestActor);
    
    // Create a ground plane
    AStaticMeshActor* GroundPlane = TestWorld->SpawnActor<AStaticMeshActor>();
    // Set up ground mesh and position...
    
    // Position actor above ground
    TestActor->SetActorLocation(FVector(0, 0, 500));
    
    // Act
    float DistanceToGround;
    bool bIsNearGround = LandingComponent->IsNearGround(DistanceToGround);
    
    // Assert
    TestTrue(TEXT("Should detect ground"), bIsNearGround);
    TestTrue(TEXT("Distance should be positive"), DistanceToGround > 0.0f);
    TestTrue(TEXT("Distance should be reasonable"), DistanceToGround < 1000.0f);
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

#### TEST-LDC-002: 着地品質計算テスト
```cpp
UTEST(LandingDetectionComponent, LandingQualityCalculation)
{
    // Arrange
    ULandingDetectionComponent* LandingComponent = NewObject<ULandingDetectionComponent>();
    
    // Perfect landing scenario
    FHitResult PerfectHit;
    PerfectHit.Distance = 0.0f; // Exact center
    float PerfectSpeed = 50.0f; // Slow landing
    
    // Poor landing scenario
    FHitResult PoorHit;
    PoorHit.Distance = 200.0f; // Off-center
    float PoorSpeed = 300.0f; // Fast landing
    
    // Act
    ELandingQuality PerfectQuality = LandingComponent->CalculateLandingQuality(PerfectHit, PerfectSpeed);
    ELandingQuality PoorQuality = LandingComponent->CalculateLandingQuality(PoorHit, PoorSpeed);
    
    // Assert
    TestEqual(TEXT("Perfect landing should yield Perfect quality"), 
             PerfectQuality, ELandingQuality::Perfect);
    TestTrue(TEXT("Poor landing should yield lower quality"), 
            static_cast<uint8>(PoorQuality) < static_cast<uint8>(ELandingQuality::Perfect));
}
```

#### TEST-LDC-003: スコア計算テスト
```cpp
UTEST(LandingDetectionComponent, ScoreCalculation)
{
    // Arrange
    ULandingDetectionComponent* LandingComponent = NewObject<ULandingDetectionComponent>();
    
    // Act
    int32 PerfectScore = LandingComponent->CalculateBaseScore(ELandingQuality::Perfect, 1.0f);
    int32 GoodScore = LandingComponent->CalculateBaseScore(ELandingQuality::Good, 0.8f);
    int32 PoorScore = LandingComponent->CalculateBaseScore(ELandingQuality::Poor, 0.5f);
    int32 MissScore = LandingComponent->CalculateBaseScore(ELandingQuality::Miss, 0.0f);
    
    // Assert
    TestTrue(TEXT("Perfect score should be highest"), PerfectScore > GoodScore);
    TestTrue(TEXT("Good score should be higher than Poor"), GoodScore > PoorScore);
    TestTrue(TEXT("Poor score should be higher than Miss"), PoorScore > MissScore);
    TestEqual(TEXT("Miss should yield zero score"), MissScore, 0);
    
    // Score ranges validation
    TestTrue(TEXT("Perfect score should be in valid range"), PerfectScore >= 1000 && PerfectScore <= 5000);
    TestTrue(TEXT("All scores should be non-negative"), 
            PerfectScore >= 0 && GoodScore >= 0 && PoorScore >= 0 && MissScore >= 0);
}
```

### 4. 物理システム統合テスト

#### TEST-PSI-001: WindForceComponent統合テスト
```cpp
UTEST(BirdCharacterPhysics, WindForceIntegration)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    UWindForceComponent* WindComponent = BirdCharacter->GetWindForceComponent();
    
    // Set up wind parameters
    WindComponent->SetWindStrength(2.0f);
    WindComponent->SetBaseWindDirection(FVector(1.0f, 0.0f, 0.0f));
    
    FVector InitialVelocity = BirdCharacter->GetVelocity();
    FVector BirdPosition = BirdCharacter->GetActorLocation();
    
    // Act
    for (int32 i = 0; i < 60; ++i) // 1 second at 60 FPS
    {
        BirdCharacter->Tick(0.016f);
    }
    
    // Assert
    FVector FinalVelocity = BirdCharacter->GetVelocity();
    TestTrue(TEXT("Bird should be affected by wind"), 
            !FinalVelocity.Equals(InitialVelocity, 0.1f));
    TestTrue(TEXT("Bird should move in wind direction"), 
            FinalVelocity.X > InitialVelocity.X);
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

#### TEST-PSI-002: MovementComponent統合テスト
```cpp
UTEST(BirdCharacterPhysics, MovementComponentIntegration)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    UBirdCharacterMovementComponent* MovementComponent = BirdCharacter->GetBirdMovementComponent();
    
    // Test gravity application
    MovementComponent->SetGravityMultiplier(2.0f);
    FVector InitialVelocity = BirdCharacter->GetVelocity();
    
    // Act
    BirdCharacter->Tick(0.016f);
    
    // Assert
    FVector NewVelocity = BirdCharacter->GetVelocity();
    TestTrue(TEXT("Gravity should affect vertical velocity"), 
            NewVelocity.Z < InitialVelocity.Z);
    
    // Test speed limits
    MovementComponent->SetMaxDiveSpeed(1000.0f);
    BirdCharacter->SetVelocity(FVector(0, 0, -2000.0f)); // Over limit
    BirdCharacter->Tick(0.016f);
    
    TestTrue(TEXT("Speed should be limited"), 
            FMath::Abs(BirdCharacter->GetVelocity().Z) <= 1000.0f + 0.1f);
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

### 5. パフォーマンステスト

#### TEST-PERF-001: Tick処理パフォーマンステスト
```cpp
UTEST(BirdCharacterPerformance, TickPerformance)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TArray<ABirdCharacter*> BirdCharacters;
    
    // Spawn multiple birds for stress test
    for (int32 i = 0; i < 100; ++i)
    {
        ABirdCharacter* Bird = TestWorld->SpawnActor<ABirdCharacter>();
        BirdCharacters.Add(Bird);
    }
    
    // Act - Measure tick performance
    double StartTime = FPlatformTime::Seconds();
    
    for (int32 Frame = 0; Frame < 60; ++Frame) // 1 second worth of ticks
    {
        for (ABirdCharacter* Bird : BirdCharacters)
        {
            Bird->Tick(0.016f);
        }
    }
    
    double EndTime = FPlatformTime::Seconds();
    double ElapsedTime = EndTime - StartTime;
    
    // Assert
    double AverageTickTime = ElapsedTime / (100 * 60); // Per bird per tick
    TestTrue(TEXT("Average tick time should be under 1ms"), 
            AverageTickTime < 0.001);
    
    UE_LOG(LogBirdDive, Log, TEXT("Bird character tick performance: %.6f ms per bird per tick"), 
           AverageTickTime * 1000.0);
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

#### TEST-PERF-002: 入力レスポンス時間テスト
```cpp
UTEST(BirdCharacterPerformance, InputResponseTime)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    FInputActionValue TestInput(FVector2D(1.0f, 1.0f));
    
    // Act - Measure input processing time
    double StartTime = FPlatformTime::Seconds();
    
    for (int32 i = 0; i < 1000; ++i)
    {
        BirdCharacter->HandleFlyInput(TestInput);
    }
    
    double EndTime = FPlatformTime::Seconds();
    double ElapsedTime = EndTime - StartTime;
    
    // Assert
    double AverageInputTime = ElapsedTime / 1000.0;
    TestTrue(TEXT("Input processing should be under 16ms (60 FPS target)"), 
            AverageInputTime < 0.016);
    
    UE_LOG(LogBirdDive, Log, TEXT("Input processing performance: %.6f ms per input"), 
           AverageInputTime * 1000.0);
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

### 6. エッジケーステスト

#### TEST-EDGE-001: 極限速度テスト
```cpp
UTEST(BirdCharacterEdgeCases, ExtremeSpeed)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    
    // Act - Set extreme velocity
    FVector ExtremeVelocity(50000.0f, 50000.0f, -50000.0f);
    BirdCharacter->SetVelocity(ExtremeVelocity);
    
    // Process several ticks
    for (int32 i = 0; i < 10; ++i)
    {
        BirdCharacter->Tick(0.016f);
    }
    
    // Assert
    FVector FinalVelocity = BirdCharacter->GetVelocity();
    TestTrue(TEXT("Velocity should remain finite"), FMath::IsFinite(FinalVelocity.Size()));
    TestTrue(TEXT("Velocity should be clamped to safe limits"), 
            FinalVelocity.Size() <= BirdCharacter->GetMaxSafeSpeed() + 100.0f);
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

#### TEST-EDGE-002: 無効入力処理テスト
```cpp
UTEST(BirdCharacterEdgeCases, InvalidInputHandling)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    FVector InitialVelocity = BirdCharacter->GetVelocity();
    
    // Act - Process invalid inputs
    FInputActionValue NaNInput(FVector2D(NAN, NAN));
    FInputActionValue InfiniteInput(FVector2D(INFINITY, -INFINITY));
    
    BirdCharacter->HandleFlyInput(NaNInput);
    BirdCharacter->HandleLookInput(InfiniteInput);
    BirdCharacter->Tick(0.016f);
    
    // Assert
    FVector FinalVelocity = BirdCharacter->GetVelocity();
    FRotator FinalRotation = BirdCharacter->GetActorRotation();
    
    TestTrue(TEXT("Velocity should remain finite after NaN input"), 
            FMath::IsFinite(FinalVelocity.Size()));
    TestTrue(TEXT("Rotation should remain finite after infinite input"), 
            FMath::IsFinite(FinalRotation.Yaw) && FMath::IsFinite(FinalRotation.Pitch));
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

### 7. E2Eテスト

#### TEST-E2E-001: 完全飛行サイクルテスト
```cpp
UTEST(BirdCharacterE2E, CompleteFightCycle)
{
    // Arrange
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    
    // Start above ground
    BirdCharacter->SetActorLocation(FVector(0, 0, 2000));
    
    // Act - Simulate complete flight cycle
    
    // 1. Take off (idle to flying)
    BirdCharacter->SetFlightState(EFlightState::Flying);
    TestEqual(TEXT("Should transition to flying"), 
             BirdCharacter->GetCurrentFlightState(), EFlightState::Flying);
    
    // 2. Flight input and physics simulation
    FInputActionValue FlightInput(FVector2D(0.5f, 0.0f));
    for (int32 i = 0; i < 300; ++i) // 5 seconds of flight
    {
        BirdCharacter->HandleFlyInput(FlightInput);
        BirdCharacter->Tick(0.016f);
        
        // Check for stable flight
        if (i % 60 == 0) // Every second
        {
            TestTrue(TEXT("Bird should maintain finite velocity during flight"), 
                    FMath::IsFinite(BirdCharacter->GetVelocity().Size()));
        }
    }
    
    // 3. Approach landing
    BirdCharacter->SetFlightState(EFlightState::Landing);
    
    // 4. Landing detection
    float DistanceToGround;
    bool bNearGround = BirdCharacter->GetLandingDetectionComponent()->IsNearGround(DistanceToGround);
    
    if (bNearGround && DistanceToGround < 50.0f)
    {
        FLandingResult LandingResult = BirdCharacter->PerformLandingCheck();
        TestTrue(TEXT("Landing result should be valid"), 
                LandingResult.Quality != ELandingQuality::Miss || LandingResult.BaseScore >= 0);
        
        BirdCharacter->SetFlightState(EFlightState::Landed);
    }
    
    // Assert final state
    TestTrue(TEXT("Should complete flight cycle successfully"), 
            BirdCharacter->GetCurrentFlightState() == EFlightState::Landed || 
            BirdCharacter->GetCurrentFlightState() == EFlightState::Landing);
    
    // Cleanup
    TestWorld->DestroyWorld(false);
}
```

## テスト実行指標

### カバレッジ目標
- **コードカバレッジ**: 90%以上
- **ブランチカバレッジ**: 85%以上  
- **機能カバレッジ**: 100%（全Public関数）

### パフォーマンス指標
- **Tick処理**: < 1ms per character per tick
- **入力処理**: < 16ms response time
- **状態遷移**: < 1ms processing time
- **着地検出**: < 5ms per check

### 品質指標
- **テスト成功率**: 100%
- **メモリリーク**: 0件
- **クラッシュ**: 0件
- **入力エラー**: 完全な処理

## 継続的テスト

### 自動化レベル
- **Unit Tests**: 完全自動化
- **Integration Tests**: 完全自動化
- **Performance Tests**: 定期実行
- **E2E Tests**: リリース前実行

### レポート生成
- **リアルタイム結果**: CI/CDパイプライン統合
- **パフォーマンス追跡**: 時系列グラフ
- **回帰検出**: 自動アラート
- **品質ダッシュボード**: 開発チーム向け

このテストスイートにより、鳥キャラクターシステムの全側面が検証され、高品質で信頼性の高い実装が保証されます。