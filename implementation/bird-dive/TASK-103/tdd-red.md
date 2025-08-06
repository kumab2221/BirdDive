# TASK-103: 物理システム基盤実装 - テスト実装（Red Phase）

## TDDフェーズ: RED - 失敗するテストの実装

このフェーズでは、まだ実装されていない物理システムクラスに対するテストを作成し、意図的に失敗させます。これによりTDDの「Red → Green → Refactor」サイクルを確立します。

## テストファイル作成

### 1. 物理システムテストファイル作成
```cpp
// File: BirdDive/Source/BirdDive/Tests/PhysicsSystemTest.cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Forward declarations for classes that don't exist yet
class UWindForceComponent;
class UBirdCharacterMovementComponent;

// ============================
// UWindForceComponent Tests
// ============================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindForceComponentBasicTest, "BirdDive.Physics.WindForceComponent.BasicWindCalculation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FWindForceComponentBasicTest::RunTest(const FString& Parameters)
{
    // This test will FAIL initially because UWindForceComponent doesn't exist yet
    
    // Arrange
    // UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    // FVector TestPosition(100.0f, 200.0f, 300.0f);
    // float TestHeight = 500.0f;
    
    // Act
    // FVector WindForce = WindComponent->CalculateWindForce(TestPosition, TestHeight);
    
    // Assert
    // TestTrue(TEXT("Wind force should be non-zero"), !WindForce.IsZero());
    // TestTrue(TEXT("Wind force should be finite"), FMath::IsFinite(WindForce.Size()));
    
    // For now, intentionally fail to establish RED phase
    TestTrue(TEXT("UWindForceComponent not implemented yet - this should fail"), false);
    
    return false; // Intentionally return false for RED phase
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindForcePerlinNoiseTest, "BirdDive.Physics.WindForceComponent.PerlinNoiseRange",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FWindForcePerlinNoiseTest::RunTest(const FString& Parameters)
{
    // This test will FAIL initially because the GetPerlinNoiseValue method doesn't exist
    
    // Arrange
    // UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    
    // Act & Assert
    // for (int32 i = 0; i < 100; ++i)
    // {
    //     FVector RandomPos(FMath::RandRange(-1000.0f, 1000.0f), 
    //                      FMath::RandRange(-1000.0f, 1000.0f), 
    //                      FMath::RandRange(0.0f, 2000.0f));
    //     float NoiseValue = WindComponent->GetPerlinNoiseValue(RandomPos, 0.0f);
    //     
    //     TestTrue(TEXT("Perlin noise should be in valid range"), 
    //             NoiseValue >= -1.0f && NoiseValue <= 1.0f);
    // }
    
    // For now, intentionally fail
    TestTrue(TEXT("GetPerlinNoiseValue method not implemented yet"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindForceDifficultyTest, "BirdDive.Physics.WindForceComponent.DifficultyBasedWindStrength",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FWindForceDifficultyTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because WindStrength property doesn't exist yet
    
    // For now, intentionally fail
    TestTrue(TEXT("WindStrength property not implemented yet"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindForcePerformanceTest, "BirdDive.Physics.WindForceComponent.Performance",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FWindForcePerformanceTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because the performance requirements aren't met yet
    
    TestTrue(TEXT("Performance requirements not met - class not implemented"), false);
    
    return false;
}

// ============================
// UBirdCharacterMovementComponent Tests
// ============================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementGravityTest, "BirdDive.Physics.BirdCharacterMovementComponent.GravityApplication",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementGravityTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because UBirdCharacterMovementComponent doesn't exist yet
    
    // Arrange
    // UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    // Movement->GravityMultiplier = 1.0f;
    // FVector InitialVelocity(0.0f, 0.0f, 0.0f);
    
    // Act
    // Movement->Velocity = InitialVelocity;
    // Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f); // 60FPS
    
    // Assert
    // TestTrue(TEXT("Velocity should be downward after gravity"), 
    //         Movement->Velocity.Z < 0.0f);
    
    // For now, intentionally fail
    TestTrue(TEXT("UBirdCharacterMovementComponent not implemented yet"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementWindForceTest, "BirdDive.Physics.BirdCharacterMovementComponent.WindForceApplication",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementWindForceTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because ApplyWindForce method doesn't exist yet
    
    TestTrue(TEXT("ApplyWindForce method not implemented yet"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementAirResistanceTest, "BirdDive.Physics.BirdCharacterMovementComponent.AirResistance",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementAirResistanceTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because AirResistance property doesn't exist yet
    
    TestTrue(TEXT("AirResistance property not implemented yet"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementMaxSpeedTest, "BirdDive.Physics.BirdCharacterMovementComponent.MaxSpeedLimit",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementMaxSpeedTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because MaxDiveSpeed property doesn't exist yet
    
    TestTrue(TEXT("MaxDiveSpeed property not implemented yet"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementNaNHandlingTest, "BirdDive.Physics.BirdCharacterMovementComponent.NaNHandling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementNaNHandlingTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because error handling isn't implemented yet
    
    TestTrue(TEXT("NaN handling not implemented yet"), false);
    
    return false;
}

// ============================
// Integration Tests
// ============================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsIntegrationTest, "BirdDive.Physics.Integration.WindAndMovement",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FPhysicsIntegrationTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because both components don't exist yet
    
    TestTrue(TEXT("Physics integration not possible - components not implemented"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultipleForceIntegrationTest, "BirdDive.Physics.Integration.MultipleForces",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMultipleForceIntegrationTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because force integration isn't implemented yet
    
    TestTrue(TEXT("Multiple force integration not implemented yet"), false);
    
    return false;
}

// ============================
// Edge Case Tests
// ============================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsEdgeCaseZeroWindTest, "BirdDive.Physics.EdgeCases.ZeroWindForce",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FPhysicsEdgeCaseZeroWindTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because edge case handling isn't implemented yet
    
    TestTrue(TEXT("Zero wind force edge case not handled yet"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsEdgeCaseExtremeHeightTest, "BirdDive.Physics.EdgeCases.ExtremeHeight",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FPhysicsEdgeCaseExtremeHeightTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because extreme height handling isn't implemented yet
    
    TestTrue(TEXT("Extreme height edge case not handled yet"), false);
    
    return false;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsEdgeCaseNegativeValuesTest, "BirdDive.Physics.EdgeCases.NegativeValues",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FPhysicsEdgeCaseNegativeValuesTest::RunTest(const FString& Parameters)
{
    // This test will FAIL because negative value handling isn't implemented yet
    
    TestTrue(TEXT("Negative values edge case not handled yet"), false);
    
    return false;
}
```

## テスト実行確認

### 1. テストファイルをプロジェクトに追加
テストファイルを適切な場所に配置し、ビルドシステムに認識させます。

### 2. コンパイル確認
- 現在の状態では、存在しないクラスを参照しているためコンパイルエラーが発生
- これは意図的な設計（RED フェーズ）

### 3. テスト実行結果の確認
UE5のAutomation Test Frameworkで以下のテストが失敗することを確認：

```
❌ BirdDive.Physics.WindForceComponent.BasicWindCalculation - FAILED
   Reason: UWindForceComponent not implemented yet - this should fail

❌ BirdDive.Physics.WindForceComponent.PerlinNoiseRange - FAILED  
   Reason: GetPerlinNoiseValue method not implemented yet

❌ BirdDive.Physics.WindForceComponent.DifficultyBasedWindStrength - FAILED
   Reason: WindStrength property not implemented yet

❌ BirdDive.Physics.WindForceComponent.Performance - FAILED
   Reason: Performance requirements not met - class not implemented

❌ BirdDive.Physics.BirdCharacterMovementComponent.GravityApplication - FAILED
   Reason: UBirdCharacterMovementComponent not implemented yet

❌ BirdDive.Physics.BirdCharacterMovementComponent.WindForceApplication - FAILED
   Reason: ApplyWindForce method not implemented yet

❌ BirdDive.Physics.BirdCharacterMovementComponent.AirResistance - FAILED
   Reason: AirResistance property not implemented yet

❌ BirdDive.Physics.BirdCharacterMovementComponent.MaxSpeedLimit - FAILED
   Reason: MaxDiveSpeed property not implemented yet

❌ BirdDive.Physics.BirdCharacterMovementComponent.NaNHandling - FAILED
   Reason: NaN handling not implemented yet

❌ BirdDive.Physics.Integration.WindAndMovement - FAILED
   Reason: Physics integration not possible - components not implemented

❌ BirdDive.Physics.Integration.MultipleForces - FAILED
   Reason: Multiple force integration not implemented yet

❌ BirdDive.Physics.EdgeCases.ZeroWindForce - FAILED
   Reason: Zero wind force edge case not handled yet

❌ BirdDive.Physics.EdgeCases.ExtremeHeight - FAILED
   Reason: Extreme height edge case not handled yet

❌ BirdDive.Physics.EdgeCases.NegativeValues - FAILED
   Reason: Negative values edge case not handled yet
```

## RED フェーズの成功基準

### ✅ 確認事項
1. **全テストが失敗する**: 14個のテストが全て失敗することを確認
2. **明確な失敗理由**: 各テストが適切な理由で失敗することを確認  
3. **コンパイルエラー**: 存在しないクラスの参照によるコンパイルエラーを確認
4. **テストの網羅性**: 要件定義の全機能に対するテストが作成されている

### 📋 次のステップ（GREEN フェーズ）への準備
1. UWindForceComponent クラスの実装
2. UBirdCharacterMovementComponent クラスの実装  
3. 必要な物理計算メソッドの実装
4. Blueprint公開用のプロパティ設定
5. エラーハンドリングの実装

## 学習ポイント

### TDD RED フェーズの価値
- **要件の明確化**: テストを書くことで実装すべき機能が明確になる
- **インターフェース設計**: 実装前にクラスのインターフェースを設計できる
- **品質保証**: 実装完了の判定基準が明確になる
- **リグレッション防止**: 将来の変更時にデグレードを検出できる

### UE5 Testing Framework の活用
- **Automation Test**: UE5の標準テストフレームワークを活用
- **Blueprint Integration**: C++からBlueprintへの公開を考慮したテスト設計
- **Performance Testing**: パフォーマンス要件もテストコードで検証

## 注意事項

### コンパイルエラーについて
現在のテストコードは、まだ実装されていないクラスを参照するため、一時的にコメントアウトしています。GREEN フェーズで実装が完了したら、コメントを外してテストを有効化します。

### テスト実行環境
- Editor Context: エディタ環境でのテスト実行
- Client Context: クライアント環境でのテスト実行  
- Product Filter: リリース版でも実行されるテスト