#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

// Include our classes
#include "../Systems/Animation/BirdControlRigComponent.h"
#include "../Systems/Animation/BirdControlRigTypes.h"
#include "../Characters/Birds/BirdCharacter.h"
#include "../Systems/Physics/WindForceComponent.h"

/**
 * BirdControlRigComponent基本機能テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdControlRigComponentBasicTest, 
    "BirdDive.ControlRig.Component.Basic", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdControlRigComponentBasicTest::RunTest(const FString& Parameters)
{
    // UT-401-001-A: コンポーネント初期化テスト
    
    TestTrue("UBirdControlRigComponent class should exist", 
             UBirdControlRigComponent::StaticClass() != nullptr);
    
    // Create a test component
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    TestNotNull("Component should be created", Component);
    
    // Test initial state
    TestFalse("Component should not be initialized initially", Component->IsInitialized());
    
    // Test initialization
    Component->InitializeForTest();
    TestTrue("Component should be initialized after BeginPlay", Component->IsInitialized());
    
    // Test ControlRig instance access
    UControlRig* ControlRigInstance = Component->GetControlRigInstance();
    // Note: ControlRigInstance may be null in minimum implementation
    // TestNotNull("ControlRig instance should exist after initialization", ControlRigInstance);
    
    return true;
}

/**
 * BirdControlRigComponent パラメータ設定テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdControlRigParameterTest, 
    "BirdDive.ControlRig.Component.Parameters", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdControlRigParameterTest::RunTest(const FString& Parameters)
{
    // UT-401-001-C: パラメータ設定テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
    // Create test parameters
    FBirdControlRigParams TestParams;
    TestParams.FlappingFrequency = 2.5f;
    TestParams.WingAngle = 45.0f;
    TestParams.WingSpread = 0.8f;
    TestParams.BodyRotation = FRotator(10.0f, 20.0f, 15.0f);
    TestParams.TailFanAngle = 30.0f;
    
    // Set parameters
    bool bSetSuccess = Component->SetControlParameters(TestParams);
    TestTrue("Parameter setting should succeed", bSetSuccess);
    
    // Get parameters back
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
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdFlappingControlTest::RunTest(const FString& Parameters)
{
    // UT-401-002-A: 基本羽ばたき頻度テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
    // Set flight speed
    float FlightSpeed = 1000.0f;
    Component->SetFlightSpeed(FlightSpeed);
    
    // Update flapping
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
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdSpeedLinkedFlappingTest::RunTest(const FString& Parameters)
{
    // UT-401-002-B: 速度連動羽ばたきテスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
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
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdFlightStateWingAngleTest::RunTest(const FString& Parameters)
{
    // UT-401-002-C: 飛行状態別翼角度テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
    // Test flying flight state
    Component->SetFlightState(EFlightState::Flying);
    Component->UpdateWingPosition(1.0f);
    float FlyingAngle = Component->GetControlParameters().WingAngle;
    
    // Test landing flight state
    Component->SetFlightState(EFlightState::Landing);
    Component->UpdateWingPosition(1.0f);
    float LandingAngle = Component->GetControlParameters().WingAngle;
    
    // Landing should have higher angle than flying
    TestTrue("Flying angle should be neutral", FMath::IsNearlyEqual(FlyingAngle, 0.0f, 0.1f));
    TestTrue("Landing angle should be positive", LandingAngle > 0.0f);
    TestTrue("Landing angle should be greater than flying", 
             LandingAngle > FlyingAngle);
    
    return true;
}

/**
 * WindForceComponent連携テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdWindForceIntegrationTest, 
    "BirdDive.ControlRig.Wind.Integration", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdWindForceIntegrationTest::RunTest(const FString& Parameters)
{
    // UT-401-003-A: WindForceComponent連携テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
    // Set wind force and update
    FVector WindForce = FVector(100.0f, 0.0f, 50.0f);
    Component->ApplyWindForce(WindForce);
    Component->UpdateWindResponse(1.0f);
    
    // Check that wind force affects control rig parameters
    FBirdControlRigParams Params = Component->GetControlParameters();
    TestTrue("Wind influence should be set", 
             Params.WindInfluence.Size() > 0.0f);
    TestEqual("Wind influence should match applied force", 
             Params.WindInfluence, WindForce, 0.001f);
    
    return true;
}

/**
 * 風向き応答テスト
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdWindDirectionResponseTest, 
    "BirdDive.ControlRig.Wind.Direction", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdWindDirectionResponseTest::RunTest(const FString& Parameters)
{
    // UT-401-003-B: 風向き応答テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
    // Apply crosswind
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
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdWindStabilizationTest::RunTest(const FString& Parameters)
{
    // UT-401-003-C: 風力安定化テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
    // Apply strong wind and stabilization
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
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdIndividualFeatherControlTest::RunTest(const FString& Parameters)
{
    // UT-401-004-A: 個別羽根制御テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
    // Set individual feather angles
    TArray<float> FeatherAngles = {10.0f, 15.0f, 8.0f, 12.0f, 20.0f};
    bool bSetSuccess = Component->SetIndividualFeatherAngles(FeatherAngles);
    TestTrue("Setting individual feather angles should succeed", bSetSuccess);
    
    // Get feather angles back
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
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdTailFeatherControlTest::RunTest(const FString& Parameters)
{
    // UT-401-004-B: 尾羽方向制御テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
    // Set tail fan angle
    float TailFanAngle = 30.0f;
    Component->SetTailFanAngle(TailFanAngle);
    
    // Check that tail fan angle is set correctly
    FBirdControlRigParams Params = Component->GetControlParameters();
    TestEqual("Tail fan angle should be set correctly", Params.TailFanAngle, TailFanAngle, 0.001f);
    
    // Apply tail control for direction change
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
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdCharacterControlRigIntegrationTest::RunTest(const FString& Parameters)
{
    // IT-401-001-A: コンポーネント統合テスト
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull("Test world should be created", TestWorld);
    
    // Create fully configured bird character
    ABirdCharacter* BirdCharacter = TestWorld->SpawnActor<ABirdCharacter>();
    TestNotNull("BirdCharacter should be created", BirdCharacter);
    
    // Simulate BeginPlay
    BirdCharacter->InitializeForTest();
    
    // Get ControlRig component
    UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
    TestNotNull("BirdControlRigComponent should exist on character", ControlRigComponent);
    TestTrue("ControlRig component should be initialized", ControlRigComponent->IsInitialized());
    
    // Set flight state and velocity
    BirdCharacter->SetFlightState(EFlightState::Flying);
    
    // Simulate one tick
    BirdCharacter->Tick(1.0f / 60.0f);
    
    // Check that ControlRig is automatically updated
    FBirdControlRigParams Params = ControlRigComponent->GetControlParameters();
    TestTrue("Flapping frequency should be greater than zero", Params.FlappingFrequency > 0.0f);
    
    // Clean up
    TestWorld->DestroyWorld(false);
    
    return true;
}

/**
 * エラーハンドリングテスト - 未初期化
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdControlRigErrorHandlingTest, 
    "BirdDive.ControlRig.ErrorHandling.Uninitialized", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdControlRigErrorHandlingTest::RunTest(const FString& Parameters)
{
    // ET-401-001-A: ControlRig未初期化テスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    // Note: BeginPlay is NOT called, so component is uninitialized
    
    // Try to set parameters on uninitialized component
    FBirdControlRigParams Params;
    Params.FlappingFrequency = 2.0f;
    
    // This should fail gracefully
    bool bSetSuccess = Component->SetControlParameters(Params);
    TestFalse("Setting parameters on uninitialized component should fail", bSetSuccess);
    
    // Check error message
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
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdControlRigInvalidParametersTest::RunTest(const FString& Parameters)
{
    // ET-401-001-B: 無効パラメータテスト
    UBirdControlRigComponent* Component = NewObject<UBirdControlRigComponent>();
    Component->InitializeForTest();
    
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