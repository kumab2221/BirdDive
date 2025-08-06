# TASK-103: 物理システム基盤実装 - テストケース

## テスト戦略

このテストスイートは、物理システムの信頼性とパフォーマンスを保証します。単体テスト、統合テスト、パフォーマンステストを含み、TDD開発サイクルをサポートします。

## テストカテゴリ

### 1. UWindForceComponent 単体テスト

#### TEST-WF-001: 基本風力計算テスト
```cpp
UTEST(WindForceComponent, BasicWindCalculation)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    FVector TestPosition(100.0f, 200.0f, 300.0f);
    float TestHeight = 500.0f;
    
    // Act
    FVector WindForce = WindComponent->CalculateWindForce(TestPosition, TestHeight);
    
    // Assert
    TestTrue(TEXT("Wind force should be non-zero"), !WindForce.IsZero());
    TestTrue(TEXT("Wind force should be finite"), FMath::IsFinite(WindForce.Size()));
}
```

#### TEST-WF-002: Perlinノイズ値域テスト
```cpp
UTEST(WindForceComponent, PerlinNoiseRange)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    
    // Act & Assert
    for (int32 i = 0; i < 100; ++i)
    {
        FVector RandomPos(FMath::RandRange(-1000.0f, 1000.0f), 
                         FMath::RandRange(-1000.0f, 1000.0f), 
                         FMath::RandRange(0.0f, 2000.0f));
        float NoiseValue = WindComponent->GetPerlinNoiseValue(RandomPos, 0.0f);
        
        TestTrue(TEXT("Perlin noise should be in valid range"), 
                NoiseValue >= -1.0f && NoiseValue <= 1.0f);
    }
}
```

#### TEST-WF-003: 難易度別風力強度テスト
```cpp
UTEST(WindForceComponent, DifficultyBasedWindStrength)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    FVector TestPosition(0.0f, 0.0f, 1000.0f);
    
    // Easy difficulty
    WindComponent->WindStrength = 0.5f;
    FVector EasyWind = WindComponent->CalculateWindForce(TestPosition, 1000.0f);
    
    // Hard difficulty  
    WindComponent->WindStrength = 2.0f;
    FVector HardWind = WindComponent->CalculateWindForce(TestPosition, 1000.0f);
    
    // Assert
    TestTrue(TEXT("Hard wind should be stronger than easy wind"), 
            HardWind.Size() > EasyWind.Size());
}
```

#### TEST-WF-004: 位置による風力変化テスト
```cpp
UTEST(WindForceComponent, PositionalWindVariation)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    FVector Position1(0.0f, 0.0f, 1000.0f);
    FVector Position2(1000.0f, 1000.0f, 1000.0f);
    
    // Act
    FVector Wind1 = WindComponent->CalculateWindForce(Position1, 1000.0f);
    FVector Wind2 = WindComponent->CalculateWindForce(Position2, 1000.0f);
    
    // Assert
    TestTrue(TEXT("Wind should vary by position"), 
            !Wind1.Equals(Wind2, 0.1f));
}
```

#### TEST-WF-005: パフォーマンステスト
```cpp
UTEST(WindForceComponent, PerformanceTest)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    FVector TestPositions[1000];
    for (int32 i = 0; i < 1000; ++i)
    {
        TestPositions[i] = FVector(FMath::RandRange(-5000.0f, 5000.0f),
                                  FMath::RandRange(-5000.0f, 5000.0f),
                                  FMath::RandRange(0.0f, 2000.0f));
    }
    
    // Act
    double StartTime = FPlatformTime::Seconds();
    for (int32 i = 0; i < 1000; ++i)
    {
        WindComponent->CalculateWindForce(TestPositions[i], TestPositions[i].Z);
    }
    double EndTime = FPlatformTime::Seconds();
    
    // Assert
    double ElapsedTime = EndTime - StartTime;
    TestTrue(TEXT("1000 wind calculations should complete within 1ms"), 
            ElapsedTime < 0.001);
}
```

### 2. UBirdCharacterMovementComponent 単体テスト

#### TEST-BCM-001: 重力適用テスト
```cpp
UTEST(BirdCharacterMovementComponent, GravityApplication)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    Movement->GravityMultiplier = 1.0f;
    FVector InitialVelocity(0.0f, 0.0f, 0.0f);
    
    // Act
    Movement->Velocity = InitialVelocity;
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f); // 60FPS
    
    // Assert
    TestTrue(TEXT("Velocity should be downward after gravity"), 
            Movement->Velocity.Z < 0.0f);
}
```

#### TEST-BCM-002: 風力適用テスト
```cpp
UTEST(BirdCharacterMovementComponent, WindForceApplication)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    FVector WindForce(100.0f, 50.0f, 0.0f);
    FVector InitialVelocity = Movement->Velocity;
    
    // Act
    Movement->ApplyWindForce(WindForce, 0.016f);
    
    // Assert
    TestTrue(TEXT("Velocity should change due to wind force"), 
            !Movement->Velocity.Equals(InitialVelocity, 0.1f));
    TestTrue(TEXT("Velocity should increase in wind direction"),
            Movement->Velocity.X > InitialVelocity.X);
}
```

#### TEST-BCM-003: 空気抵抗テスト
```cpp
UTEST(BirdCharacterMovementComponent, AirResistance)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    Movement->AirResistance = 0.02f;
    Movement->Velocity = FVector(1000.0f, 0.0f, -1000.0f);
    float InitialSpeed = Movement->Velocity.Size();
    
    // Act
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    float FinalSpeed = Movement->Velocity.Size();
    TestTrue(TEXT("Air resistance should reduce speed"), 
            FinalSpeed < InitialSpeed);
}
```

#### TEST-BCM-004: 最大速度制限テスト
```cpp
UTEST(BirdCharacterMovementComponent, MaxSpeedLimit)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    Movement->MaxDiveSpeed = 2000.0f;
    Movement->Velocity = FVector(0.0f, 0.0f, -3000.0f); // Over limit
    
    // Act
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    TestTrue(TEXT("Speed should be limited to MaxDiveSpeed"), 
            Movement->Velocity.Size() <= Movement->MaxDiveSpeed + 0.1f);
}
```

#### TEST-BCM-005: 異常値処理テスト
```cpp
UTEST(BirdCharacterMovementComponent, NaNHandling)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    
    // Act
    FVector NaNForce(NAN, NAN, NAN);
    Movement->ApplyWindForce(NaNForce, 0.016f);
    
    // Assert
    TestTrue(TEXT("Velocity should remain finite after NaN input"), 
            FMath::IsFinite(Movement->Velocity.Size()));
}
```

### 3. 統合テスト

#### TEST-INT-001: 風力とキャラクター移動の統合
```cpp
UTEST(PhysicsIntegration, WindAndMovementIntegration)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    FVector Position(0.0f, 0.0f, 1000.0f);
    
    // Act
    FVector WindForce = WindComponent->CalculateWindForce(Position, 1000.0f);
    FVector InitialVelocity = Movement->Velocity;
    Movement->ApplyWindForce(WindForce, 0.016f);
    
    // Assert
    TestTrue(TEXT("Movement should respond to wind force"), 
            !Movement->Velocity.Equals(InitialVelocity, 0.1f));
}
```

#### TEST-INT-002: 複数物理力の統合テスト
```cpp
UTEST(PhysicsIntegration, MultipleForceIntegration)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    FVector WindForce(100.0f, 0.0f, 0.0f);
    
    // Act - Apply wind and gravity simultaneously
    Movement->ApplyWindForce(WindForce, 0.016f);
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    TestTrue(TEXT("Should have horizontal component from wind"), 
            FMath::Abs(Movement->Velocity.X) > 0.1f);
    TestTrue(TEXT("Should have vertical component from gravity"), 
            Movement->Velocity.Z < 0.0f);
}
```

### 4. エッジケーステスト

#### TEST-EDGE-001: ゼロ風力テスト
```cpp
UTEST(PhysicsEdgeCases, ZeroWindForce)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    WindComponent->WindStrength = 0.0f;
    
    // Act
    FVector WindForce = WindComponent->CalculateWindForce(FVector::ZeroVector, 1000.0f);
    
    // Assert
    TestTrue(TEXT("Zero wind strength should produce zero force"), 
            WindForce.IsNearlyZero(0.01f));
}
```

#### TEST-EDGE-002: 極限高度テスト
```cpp
UTEST(PhysicsEdgeCases, ExtremeHeight)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    
    // Act & Assert
    FVector WindAtZero = WindComponent->CalculateWindForce(FVector::ZeroVector, 0.0f);
    FVector WindAtMax = WindComponent->CalculateWindForce(FVector::ZeroVector, 100000.0f);
    
    TestTrue(TEXT("Wind at zero height should be finite"), 
            FMath::IsFinite(WindAtZero.Size()));
    TestTrue(TEXT("Wind at extreme height should be finite"), 
            FMath::IsFinite(WindAtMax.Size()));
}
```

#### TEST-EDGE-003: 負の値テスト
```cpp
UTEST(PhysicsEdgeCases, NegativeValues)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    
    // Act
    Movement->ApplyWindForce(FVector(-1000.0f, -1000.0f, -1000.0f), 0.016f);
    
    // Assert
    TestTrue(TEXT("Should handle negative wind forces"), 
            FMath::IsFinite(Movement->Velocity.Size()));
}
```

## テスト実行指標

### カバレッジ目標
- コードカバレッジ: 90%以上
- ブランチカバレッジ: 85%以上
- 関数カバレッジ: 100%

### パフォーマンス指標
- 風力計算: < 1ms per 1000 calculations
- 移動計算: < 0.1ms per frame
- メモリ使用量: < 1MB additional

### 品質指標
- テスト成功率: 100%
- 異常値処理: 全ケース対応
- エラーハンドリング: 全例外キャッチ

## テスト環境要件

### ハードウェア
- CPU: テスト実行用の最低性能
- メモリ: 8GB以上
- GPU: DirectX 11対応

### ソフトウェア
- UE5.6 Engine with Testing Framework
- Visual Studio 2022
- Git (バージョン管理)

## 継続的テスト

### 自動化
- ビルド時の自動テスト実行
- プルリクエスト時のテストスイート実行
- パフォーマンス回帰テスト

### レポート
- テスト結果のHTMLレポート生成
- カバレッジレポートの自動生成
- パフォーマンスメトリクスの追跡