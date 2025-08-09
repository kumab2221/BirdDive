# TASK-401: ControlRigシステム実装 - Red Phase (失敗するテスト)

## 実装ステータス: RED Phase

この段階では、実装されていない機能に対するテストを作成し、それらが失敗することを確認します。

## 実装されたテストファイル

### 1. BirdControlRigComponentTest.cpp

まず、基本的なBirdControlRigComponent用のテストファイルを作成します。

**ファイル**: `BirdDive/Source/BirdDive/Tests/BirdControlRigComponentTest.cpp`

```cpp
#include "CoreMinimal.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

// Include our classes (these don't exist yet - that's why tests will fail)
#include "Systems/Animation/BirdControlRigComponent.h"
#include "Characters/Birds/BirdCharacter.h"
#include "Systems/Physics/WindForceComponent.h"

/**
 * BirdControlRigComponent基本機能テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdControlRigComponentBasicTest, 
    "BirdDive.ControlRig.Component.Basic", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdControlRigComponentBasicTest::RunTest(const FString& Parameters)
{
    // UT-401-001-A: コンポーネント初期化テスト
    TestTrue("UBirdControlRigComponent class should exist", 
             UBirdControlRigComponent::StaticClass() != nullptr);
    
    // Create a test component
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    TestNotNull("Component should be created", Component);
    
    // Test initial state (this will fail because component doesn't exist yet)
    TestFalse("Component should not be initialized initially", Component->IsInitialized());
    
    // Test initialization (this will fail because BeginPlay is not implemented)
    Component->BeginPlay();
    TestTrue("Component should be initialized after BeginPlay", Component->IsInitialized());
    
    // Test ControlRig instance access (this will fail - method doesn't exist)
    UControlRig* ControlRigInstance = Component->GetControlRigInstance();
    TestNotNull("ControlRig instance should exist after initialization", ControlRigInstance);
    
    return true;
}

/**
 * BirdControlRigComponent パラメータ設定テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdControlRigParameterTest, 
    "BirdDive.ControlRig.Component.Parameters", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdControlRigParameterTest::RunTest(const FString& Parameters)
{
    // UT-401-001-C: パラメータ設定テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Create test parameters (this struct doesn't exist yet)
    FBirdControlRigParams TestParams;
    TestParams.FlappingFrequency = 2.5f;
    TestParams.WingAngle = 45.0f;
    TestParams.WingSpread = 0.8f;
    TestParams.BodyRotation = FRotator(10.0f, 20.0f, 15.0f);
    TestParams.TailFanAngle = 30.0f;
    
    // Set parameters (this method doesn't exist yet)
    bool bSetSuccess = Component->SetControlParameters(TestParams);
    TestTrue("Parameter setting should succeed", bSetSuccess);
    
    // Get parameters back (this method doesn't exist yet)
    FBirdControlRigParams RetrievedParams = Component->GetControlParameters();
    TestEqual("FlappingFrequency should match", RetrievedParams.FlappingFrequency, 2.5f, 0.001f);
    TestEqual("WingAngle should match", RetrievedParams.WingAngle, 45.0f, 0.001f);
    TestEqual("WingSpread should match", RetrievedParams.WingSpread, 0.8f, 0.001f);
    
    return true;
}

/**
 * 羽ばたき制御テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdFlappingControlTest, 
    "BirdDive.ControlRig.Flapping.Control", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdFlappingControlTest::RunTest(const FString& Parameters)
{
    // UT-401-002-A: 基本羽ばたき頻度テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Set flight speed (this method doesn't exist yet)
    float FlightSpeed = 1000.0f;
    Component->SetFlightSpeed(FlightSpeed);
    
    // Update flapping (this method doesn't exist yet)
    Component->UpdateFlapping(1.0f);
    
    // Check that flapping frequency is set correctly
    float ExpectedFrequency = Component->CalculateFlappingFrequency(FlightSpeed);
    FBirdControlRigParams Params = Component->GetControlParameters();
    TestEqual("Flapping frequency should match calculated value", 
              Params.FlappingFrequency, ExpectedFrequency, 0.001f);
    
    return true;
}

/**
 * 速度連動羽ばたきテスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdSpeedLinkedFlappingTest, 
    "BirdDive.ControlRig.Flapping.SpeedLinked", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdSpeedLinkedFlappingTest::RunTest(const FString& Parameters)
{
    // UT-401-002-B: 速度連動羽ばたきテスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Test with low speed
    float LowSpeed = 500.0f;
    Component->SetFlightSpeed(LowSpeed);
    Component->UpdateFlapping(1.0f);
    float LowSpeedFrequency = Component->GetControlParameters().FlappingFrequency;
    
    // Test with high speed
    float HighSpeed = 2000.0f;
    Component->SetFlightSpeed(HighSpeed);
    Component->UpdateFlapping(1.0f);
    float HighSpeedFrequency = Component->GetControlParameters().FlappingFrequency;
    
    // High speed should have higher flapping frequency
    TestTrue("High speed should have higher flapping frequency", 
             HighSpeedFrequency > LowSpeedFrequency);
    TestTrue("High speed frequency should be significantly higher", 
             HighSpeedFrequency > 1.5f * LowSpeedFrequency);
    
    return true;
}

/**
 * 飛行状態別翼角度テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdFlightStateWingAngleTest, 
    "BirdDive.ControlRig.FlightState.WingAngle", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdFlightStateWingAngleTest::RunTest(const FString& Parameters)
{
    // UT-401-002-C: 飛行状態別翼角度テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Test ascending flight state
    Component->SetFlightState(EFlightState::Ascending);
    Component->UpdateWingPosition(1.0f);
    float AscendingAngle = Component->GetControlParameters().WingAngle;
    
    // Test descending flight state
    Component->SetFlightState(EFlightState::Descending);
    Component->UpdateWingPosition(1.0f);
    float DescendingAngle = Component->GetControlParameters().WingAngle;
    
    // Ascending should have positive angle, descending should have negative
    TestTrue("Ascending angle should be positive", AscendingAngle > 0.0f);
    TestTrue("Descending angle should be negative", DescendingAngle < 0.0f);
    TestTrue("Ascending angle should be greater than descending", 
             AscendingAngle > DescendingAngle);
    
    return true;
}

/**
 * WindForceComponent連携テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdWindForceIntegrationTest, 
    "BirdDive.ControlRig.Wind.Integration", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdWindForceIntegrationTest::RunTest(const FString& Parameters)
{
    // UT-401-003-A: WindForceComponent連携テスト
    
    // Create a test world
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test world should be created", TestWorld);
    
    // Create BirdCharacter (this will fail because integration doesn't exist yet)
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    TestNotNull("BirdCharacter should be created", BirdCharacter);
    
    // Get components (these methods don't exist yet)
    UWindForceComponent* WindComponent = BirdCharacter->GetWindForceComponent();
    UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
    
    TestNotNull("WindForceComponent should exist", WindComponent);
    TestNotNull("BirdControlRigComponent should exist", ControlRigComponent);
    
    // Set wind force and update
    FVector WindForce = FVector(100.0f, 0.0f, 50.0f);
    WindComponent->SetWindForce(WindForce);
    ControlRigComponent->UpdateWindResponse(1.0f);
    
    // Check that wind force affects control rig parameters
    FBirdControlRigParams Params = ControlRigComponent->GetControlParameters();
    TestTrue("Body rotation should be affected by wind", 
             Params.BodyRotation != FRotator::ZeroRotator);
    TestTrue("Wind influence should be greater than zero", 
             Params.WindInfluence.Size() > 0.0f);
    
    // Clean up
    TestWorld->DestroyWorld(false);
    
    return true;
}

/**
 * 風向き応答テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdWindDirectionResponseTest, 
    "BirdDive.ControlRig.Wind.Direction", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdWindDirectionResponseTest::RunTest(const FString& Parameters)
{
    // UT-401-003-B: 風向き応答テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Apply crosswind (this method doesn't exist yet)
    FVector CrossWind = FVector(0.0f, 1000.0f, 0.0f);
    Component->ApplyWindForce(CrossWind);
    Component->UpdateWindResponse(1.0f);
    
    // Check that bird tilts appropriately to crosswind
    FBirdControlRigParams Params = Component->GetControlParameters();
    TestTrue("Body should roll in response to crosswind", Params.BodyRotation.Roll != 0.0f);
    TestTrue("Roll should be significant", FMath::Abs(Params.BodyRotation.Roll) > 5.0f);
    
    return true;
}

/**
 * 風力安定化テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdWindStabilizationTest, 
    "BirdDive.ControlRig.Wind.Stabilization", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdWindStabilizationTest::RunTest(const FString& Parameters)
{
    // UT-401-003-C: 風力安定化テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Apply strong wind and stabilization (these methods don't exist yet)
    FVector StrongWind = FVector(1500.0f, 0.0f, 0.0f);
    Component->ApplyWindForce(StrongWind);
    Component->SetStabilizationStrength(1.0f);
    Component->UpdateWindResponse(1.0f);
    
    // Check that wings take stabilizing posture
    FBirdControlRigParams Params = Component->GetControlParameters();
    TestTrue("Stabilization strength should be high", Params.StabilizationStrength > 0.8f);
    TestTrue("Wing spread should be near maximum", Params.WingSpread > 0.9f);
    
    return true;
}

/**
 * 個別羽根制御テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdIndividualFeatherControlTest, 
    "BirdDive.ControlRig.Feather.Individual", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdIndividualFeatherControlTest::RunTest(const FString& Parameters)
{
    // UT-401-004-A: 個別羽根制御テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Set individual feather angles (this method doesn't exist yet)
    TArray<float> FeatherAngles = {10.0f, 15.0f, 8.0f, 12.0f, 20.0f};
    bool bSetSuccess = Component->SetIndividualFeatherAngles(FeatherAngles);
    TestTrue("Setting individual feather angles should succeed", bSetSuccess);
    
    // Get feather angles back (this method doesn't exist yet)
    TArray<float> RetrievedAngles = Component->GetIndividualFeatherAngles();
    TestEqual("Number of feather angles should match", RetrievedAngles.Num(), FeatherAngles.Num());
    
    for (int32 i = 0; i < FeatherAngles.Num(); i++)
    {
        TestEqual(FString::Printf(TEXT("Feather angle %d should match"), i), 
                  RetrievedAngles[i], FeatherAngles[i], 0.001f);
    }
    
    return true;
}

/**
 * 尾羽方向制御テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdTailFeatherControlTest, 
    "BirdDive.ControlRig.TailFeather.Direction", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdTailFeatherControlTest::RunTest(const FString& Parameters)
{
    // UT-401-004-B: 尾羽方向制御テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Set tail fan angle (this method doesn't exist yet)
    float TailFanAngle = 30.0f;
    Component->SetTailFanAngle(TailFanAngle);
    
    // Check that tail fan angle is set correctly
    FBirdControlRigParams Params = Component->GetControlParameters();
    TestEqual("Tail fan angle should be set correctly", Params.TailFanAngle, TailFanAngle, 0.001f);
    
    // Apply tail control for direction change (this method doesn't exist yet)
    FRotator TurnDirection = FRotator(0.0f, 45.0f, 0.0f);
    Component->ApplyTailControl(TurnDirection);
    
    // Check that tail responds to direction control
    FBirdControlRigParams UpdatedParams = Component->GetControlParameters();
    TestTrue("Tail fan angle should change in response to direction control", 
             UpdatedParams.TailFanAngle != TailFanAngle);
    
    return true;
}

/**
 * BirdCharacter統合テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdCharacterControlRigIntegrationTest, 
    "BirdDive.ControlRig.Integration.Character", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdCharacterControlRigIntegrationTest::RunTest(const FString& Parameters)
{
    // IT-401-001-A: コンポーネント統合テスト
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test world should be created", TestWorld);
    
    // Create fully configured bird character (this setup doesn't exist yet)
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    TestNotNull("BirdCharacter should be created", BirdCharacter);
    
    // Simulate BeginPlay
    BirdCharacter->BeginPlay();
    
    // Get ControlRig component (this integration doesn't exist yet)
    UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
    TestNotNull("BirdControlRigComponent should exist on character", ControlRigComponent);
    
    // Set flight state and velocity
    BirdCharacter->SetFlightState(EFlightState::Flying);
    BirdCharacter->GetCharacterMovement()->Velocity = FVector(1000.0f, 0.0f, 100.0f);
    
    // Simulate one tick
    BirdCharacter->Tick(1.0f / 60.0f);
    
    // Check that ControlRig is automatically updated
    FBirdControlRigParams Params = ControlRigComponent->GetControlParameters();
    TestTrue("Flapping frequency should be greater than zero", Params.FlappingFrequency > 0.0f);
    TestTrue("Wing angle should be non-zero", Params.WingAngle != 0.0f);
    
    // Clean up
    TestWorld->DestroyWorld(false);
    
    return true;
}

/**
 * エラーハンドリングテスト - 未初期化
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdControlRigErrorHandlingTest, 
    "BirdDive.ControlRig.ErrorHandling.Uninitialized", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdControlRigErrorHandlingTest::RunTest(const FString& Parameters)
{
    // ET-401-001-A: ControlRig未初期化テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    // Note: BeginPlay is NOT called, so component is uninitialized
    
    // Try to set parameters on uninitialized component
    FBirdControlRigParams Params;
    Params.FlappingFrequency = 2.0f;
    
    // This should fail gracefully (these methods don't exist yet)
    bool bSetSuccess = Component->SetControlParameters(Params);
    TestFalse("Setting parameters on uninitialized component should fail", bSetSuccess);
    
    // Check error message (this method doesn't exist yet)
    FString LastError = Component->GetLastError();
    TestTrue("Error message should indicate not initialized", 
             LastError.Contains(TEXT("Not initialized")) || LastError.Contains(TEXT("not initialized")));
    
    return true;
}

/**
 * エラーハンドリングテスト - 無効パラメータ
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdControlRigInvalidParametersTest, 
    "BirdDive.ControlRig.ErrorHandling.InvalidParameters", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBirdControlRigInvalidParametersTest::RunTest(const FString& Parameters)
{
    // ET-401-001-B: 無効パラメータテスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->BeginPlay();
    
    // Create invalid parameters
    FBirdControlRigParams InvalidParams;
    InvalidParams.FlappingFrequency = -1.0f; // Negative value should be invalid
    InvalidParams.WingAngle = 180.0f; // Out of valid range
    InvalidParams.WingSpread = -0.5f; // Negative value should be invalid
    
    // Try to set invalid parameters
    bool bSetSuccess = Component->SetControlParameters(InvalidParams);
    TestFalse("Setting invalid parameters should fail", bSetSuccess);
    
    // Check error message
    FString LastError = Component->GetLastError();
    TestTrue("Error message should indicate invalid parameter", 
             LastError.Contains(TEXT("Invalid parameter")) || LastError.Contains(TEXT("invalid parameter")));
    
    return true;
}
```

### 2. データ構造定義（失敗の原因となるヘッダーファイル）

**ファイル**: `BirdDive/Source/BirdDive/Systems/Animation/BirdControlRigTypes.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BirdControlRigTypes.generated.h"

/**
 * ControlRig制御パラメータ構造体
 * （この構造体は存在しないため、テストが失敗する）
 */
USTRUCT(BlueprintType)
struct BIRDDIVE_API FBirdControlRigParams
{
    GENERATED_BODY()

    // 羽ばたき関連パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flapping")
    float FlappingFrequency = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flapping")
    float WingAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flapping")
    float WingSpread = 1.0f;

    // 姿勢制御パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Posture")
    FRotator BodyRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Posture")
    float TailFanAngle = 0.0f;

    // 風力応答パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    FVector WindInfluence = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    float StabilizationStrength = 0.0f;

    // コンストラクタ
    FBirdControlRigParams()
    {
        FlappingFrequency = 1.0f;
        WingAngle = 0.0f;
        WingSpread = 1.0f;
        BodyRotation = FRotator::ZeroRotator;
        TailFanAngle = 0.0f;
        WindInfluence = FVector::ZeroVector;
        StabilizationStrength = 0.0f;
    }
};
```

### 3. BirdControlRigComponent クラス宣言（存在しないため失敗）

**ファイル**: `BirdDive/Source/BirdDive/Systems/Animation/BirdControlRigComponent.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimInstance.h"
#include "ControlRig.h"
#include "BirdControlRigTypes.h"
#include "../../BirdDiveTypes.h"
#include "BirdControlRigComponent.generated.h"

// Forward declarations
class UControlRig;
class ABirdCharacter;
class UWindForceComponent;

/**
 * BirdControlRigComponent
 * 
 * 鳥キャラクターのControlRig制御コンポーネント
 * このクラスは現在存在しないため、すべてのテストが失敗する
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BIRDDIVE_API UBirdControlRigComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Constructor
    UBirdControlRigComponent();

protected:
    // Lifecycle
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // Tick function
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                              FActorComponentTickFunction* ThisTickFunction) override;

    // === 基本機能（これらのメソッドは実装されていない） ===
    
    /**
     * コンポーネントが初期化されているかチェック
     */
    UFUNCTION(BlueprintCallable, Category = "ControlRig")
    bool IsInitialized() const;

    /**
     * ControlRigインスタンスを取得
     */
    UFUNCTION(BlueprintCallable, Category = "ControlRig")
    UControlRig* GetControlRigInstance() const;

    /**
     * 制御パラメータを設定
     */
    UFUNCTION(BlueprintCallable, Category = "ControlRig")
    bool SetControlParameters(const FBirdControlRigParams& Params);

    /**
     * 制御パラメータを取得
     */
    UFUNCTION(BlueprintCallable, Category = "ControlRig")
    FBirdControlRigParams GetControlParameters() const;

    /**
     * 最後のエラーメッセージを取得
     */
    UFUNCTION(BlueprintCallable, Category = "ControlRig")
    FString GetLastError() const;

    // === 羽ばたき制御（これらのメソッドは実装されていない） ===
    
    /**
     * 飛行速度を設定
     */
    UFUNCTION(BlueprintCallable, Category = "Flapping")
    void SetFlightSpeed(float Speed);

    /**
     * 羽ばたきを更新
     */
    UFUNCTION(BlueprintCallable, Category = "Flapping")
    void UpdateFlapping(float DeltaTime);

    /**
     * 羽ばたき頻度を計算
     */
    UFUNCTION(BlueprintCallable, Category = "Flapping")
    float CalculateFlappingFrequency(float FlightSpeed) const;

    /**
     * 飛行状態を設定
     */
    UFUNCTION(BlueprintCallable, Category = "Flapping")
    void SetFlightState(EFlightState State);

    /**
     * 翼位置を更新
     */
    UFUNCTION(BlueprintCallable, Category = "Flapping")
    void UpdateWingPosition(float DeltaTime);

    // === 風力応答制御（これらのメソッドは実装されていない） ===
    
    /**
     * 風力を適用
     */
    UFUNCTION(BlueprintCallable, Category = "Wind")
    void ApplyWindForce(const FVector& WindForce);

    /**
     * 風力応答を更新
     */
    UFUNCTION(BlueprintCallable, Category = "Wind")
    void UpdateWindResponse(float DeltaTime);

    /**
     * 安定化強度を設定
     */
    UFUNCTION(BlueprintCallable, Category = "Wind")
    void SetStabilizationStrength(float Strength);

    // === プロシージャル制御（これらのメソッドは実装されていない） ===
    
    /**
     * 個別羽根角度を設定
     */
    UFUNCTION(BlueprintCallable, Category = "Feathers")
    bool SetIndividualFeatherAngles(const TArray<float>& Angles);

    /**
     * 個別羽根角度を取得
     */
    UFUNCTION(BlueprintCallable, Category = "Feathers")
    TArray<float> GetIndividualFeatherAngles() const;

    /**
     * 尾羽角度を設定
     */
    UFUNCTION(BlueprintCallable, Category = "Feathers")
    void SetTailFanAngle(float Angle);

    /**
     * 尾羽制御を適用
     */
    UFUNCTION(BlueprintCallable, Category = "Feathers")
    void ApplyTailControl(const FRotator& TurnDirection);

protected:
    // ControlRig関連
    UPROPERTY()
    UControlRig* ControlRigInstance;

    // 制御パラメータ
    UPROPERTY()
    FBirdControlRigParams CurrentParameters;

    // 状態管理
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    EFlightState CurrentFlightState;

    UPROPERTY()
    float CurrentFlightSpeed;

    // エラー管理
    UPROPERTY()
    FString LastErrorMessage;

    // 個別羽根制御
    UPROPERTY()
    TArray<float> IndividualFeatherAngles;
};
```

### 4. BirdCharacter統合メソッドの宣言（存在しないため失敗）

**BirdCharacter.h への追加**（これらのメソッドは存在しない）:

```cpp
// BirdCharacter.h に追加される予定のメソッド（現在は存在しない）

/**
 * BirdControlRigComponentを取得
 */
UFUNCTION(BlueprintCallable, Category = "Components")
UBirdControlRigComponent* GetBirdControlRigComponent() const { return BirdControlRigComponent; }

protected:
// 追加されるコンポーネント（現在は存在しない）
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
UBirdControlRigComponent* BirdControlRigComponent;
```

## テスト実行結果の予想

### 予想される失敗

1. **コンパイルエラー**: 
   - `UBirdControlRigComponent`クラスが存在しないため
   - `FBirdControlRigParams`構造体が存在しないため
   - 各種メソッドが存在しないため

2. **リンクエラー**:
   - 実装されていないメソッドの呼び出し

3. **ランタイムエラー**:
   - nullptrアクセス
   - 未実装メソッドの呼び出し

### テスト実行コマンド

```bash
# Unreal Engine Automation Tests を実行
# 現在の段階では、これらのテストは全て失敗するはず

# すべてのControlRigテストを実行
UnrealEditor.exe BirdDive -ExecCmds="Automation RunTests BirdDive.ControlRig" -unattended -NullRHI -nosplash

# 個別テストの実行
UnrealEditor.exe BirdDive -ExecCmds="Automation RunTests BirdDive.ControlRig.Component.Basic" -unattended -NullRHI -nosplash
```

### 期待される結果

- **全テスト失敗**: 実装されていないクラスとメソッドに対するテストのため
- **明確な失敗理由**: コンパイルエラー、リンクエラー、ランタイムエラー
- **次のステップの明確化**: Green Phaseで実装すべき内容の具体化

## Red Phase 完了基準

- [ ] すべてのテストファイルが作成されている
- [ ] テストがコンパイルエラーまたは実行時エラーで失敗する
- [ ] 失敗の理由が明確（未実装のクラス/メソッドのため）
- [ ] Green Phaseで実装すべき内容が明確になっている

## 次のステップ（Green Phase）

1. `UBirdControlRigComponent`クラスの基本実装
2. `FBirdControlRigParams`構造体の完全実装
3. 各テストが成功するための最小限の実装
4. BirdCharacterとの統合実装

この Red Phase により、実装すべき機能が明確になり、TDD のサイクルが正しく機能していることを確認できます。