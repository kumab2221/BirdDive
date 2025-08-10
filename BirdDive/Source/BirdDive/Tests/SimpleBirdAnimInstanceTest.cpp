#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "../Systems/Animation/BirdAnimInstance.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdAnimInstanceBasicTest, "BirdDive.Animation.BirdAnimInstance.BasicTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdAnimInstanceBasicTest::RunTest(const FString& Parameters)
{
    // Given: Create a new BirdAnimInstance
    UBirdAnimInstance* AnimInstance = NewObject<UBirdAnimInstance>();
    
    // When: Test basic creation
    TestNotNull("AnimInstance should be created", AnimInstance);
    
    if (AnimInstance)
    {
        // Then: Test initial values
        TestEqual("Speed should be 0.0f", AnimInstance->GetSpeed(), 0.0f);
        TestEqual("VerticalVelocity should be 0.0f", AnimInstance->GetVerticalVelocity(), 0.0f);
        TestFalse("IsFlying should be false", AnimInstance->GetIsFlying());
        TestFalse("IsLanding should be false", AnimInstance->GetIsLanding());
        
        // Test state machine (these should fail initially)
        FString CurrentState = AnimInstance->GetCurrentAnimStateName();
        TestEqual("Current state should be Idle", CurrentState, FString("Idle"));
        
        // Test blend space - THIS SHOULD FAIL in RED phase
        AnimInstance->SetSpeed(1000.0f);
        AnimInstance->SetVerticalVelocity(500.0f);
        FVector BlendInput = AnimInstance->GetFlightBlendSpaceInput();
        TestEqual("BlendInput.X should be 1000.0", static_cast<float>(BlendInput.X), 1000.0f);
        TestEqual("BlendInput.Y should be 500.0", static_cast<float>(BlendInput.Y), 500.0f);
        
        // Test ControlRig integration - THIS SHOULD FAIL in RED phase
        UBirdControlRigComponent* ControlRig = AnimInstance->GetControlRigComponent();
        TestNotNull("ControlRig should exist", ControlRig);
    }
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdAnimInstanceStateTest, "BirdDive.Animation.BirdAnimInstance.StateTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdAnimInstanceStateTest::RunTest(const FString& Parameters)
{
    // Given: Create AnimInstance
    UBirdAnimInstance* AnimInstance = NewObject<UBirdAnimInstance>();
    TestNotNull("AnimInstance should be created", AnimInstance);
    
    if (AnimInstance)
    {
        AnimInstance->InitializeForTest();
        
        // Setup proper conditions for transition
        AnimInstance->SetSpeed(500.0f);
        AnimInstance->SetIsFlying(true);
        
        // When: Try state transition from Idle to Flying
        bool TransitionResult = AnimInstance->TryTransitionToState("Flying");
        
        // Then: Should succeed in GREEN phase (will fail in RED)
        TestTrue("Transition should succeed with proper conditions", TransitionResult);
        
        FString CurrentState = AnimInstance->GetCurrentStateName();
        TestEqual("State should change to Flying", CurrentState, FString("Flying"));
        
        // Test automatic state machine updates
        AnimInstance->SetSpeed(1500.0f);
        AnimInstance->SetVerticalVelocity(200.0f);
        AnimInstance->UpdateStateMachine(1.0f / 60.0f);
        
        CurrentState = AnimInstance->GetCurrentStateName();
        TestEqual("State should auto-transition to Climbing", CurrentState, FString("Climbing"));
    }
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdAnimInstanceBlendSpaceTest, "BirdDive.Animation.BirdAnimInstance.BlendSpaceTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdAnimInstanceBlendSpaceTest::RunTest(const FString& Parameters)
{
    // Given: Create AnimInstance
    UBirdAnimInstance* AnimInstance = NewObject<UBirdAnimInstance>();
    TestNotNull("AnimInstance should be created", AnimInstance);
    
    if (AnimInstance)
    {
        // When: Set speed parameters
        AnimInstance->SetSpeed(1000.0f);
        AnimInstance->SetVerticalVelocity(500.0f);
        
        // Then: Blend space should work (will fail initially)
        FVector BlendInput = AnimInstance->GetFlightBlendSpaceInput();
        TestEqual("BlendInput.X should match speed", static_cast<float>(BlendInput.X), 1000.0f);
        TestEqual("BlendInput.Y should match vertical velocity", static_cast<float>(BlendInput.Y), 500.0f);
        
        TArray<float> BlendWeights = AnimInstance->GetFlightBlendWeights();
        TestTrue("BlendWeights should not be empty", BlendWeights.Num() > 0);
        
        // Test weight normalization
        float TotalWeight = 0.0f;
        for (float Weight : BlendWeights)
        {
            TotalWeight += Weight;
            TestTrue("Weight should be >= 0", Weight >= 0.0f);
            TestTrue("Weight should be <= 1", Weight <= 1.0f);
        }
        TestEqual("Total weight should be 1.0", TotalWeight, 1.0f);
    }
    
    return true;
}

// Advanced blend space tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdAnimInstanceAdvancedBlendTest, "BirdDive.Animation.BirdAnimInstance.AdvancedBlend",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdAnimInstanceAdvancedBlendTest::RunTest(const FString& Parameters)
{
    // Given: Create AnimInstance
    UBirdAnimInstance* AnimInstance = NewObject<UBirdAnimInstance>();
    TestNotNull("AnimInstance should be created", AnimInstance);
    
    if (AnimInstance)
    {
        // Test directional blend space input calculation
        FVector2D DirectionalInput = AnimInstance->GetDirectionalBlendSpaceInput();
        TestEqual("DirectionalInput.X should be InputX", static_cast<float>(DirectionalInput.X), AnimInstance->GetInputX());
        TestEqual("DirectionalInput.Y should be InputY", static_cast<float>(DirectionalInput.Y), AnimInstance->GetInputY());
        
        // Test turn animation weight calculation
        float TurnWeight = AnimInstance->GetTurnAnimationWeight();
        TestTrue("TurnAnimationWeight should be valid", TurnWeight >= 0.0f && TurnWeight <= 1.0f);
        
        float DirectionalWeight = AnimInstance->GetDirectionalBlendWeight();
        TestTrue("DirectionalBlendWeight should be valid", DirectionalWeight >= 0.0f && DirectionalWeight <= 1.0f);
    }
    
    return true;
}

// Physics integration tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdAnimInstancePhysicsTest, "BirdDive.Animation.BirdAnimInstance.Physics",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdAnimInstancePhysicsTest::RunTest(const FString& Parameters)
{
    // Given: Create AnimInstance
    UBirdAnimInstance* AnimInstance = NewObject<UBirdAnimInstance>();
    TestNotNull("AnimInstance should be created", AnimInstance);
    
    if (AnimInstance)
    {
        // When: Set physics parameters
        FVector WindInfluence = FVector(100.0f, 50.0f, 0.0f);
        AnimInstance->SetWindInfluence(WindInfluence);
        AnimInstance->SetPhysicsBlendWeight(0.7f);
        
        // Update physics animation
        AnimInstance->UpdatePhysicsAnimation(1.0f / 60.0f);
        
        // Then: Physics parameters should be properly applied
        FVector CurrentWindInfluence = AnimInstance->GetWindInfluence();
        TestEqual("WindInfluence should be set correctly", CurrentWindInfluence, WindInfluence);
        
        float PhysicsBlendWeight = AnimInstance->GetPhysicsBlendWeight();
        TestEqual("PhysicsBlendWeight should be set correctly", PhysicsBlendWeight, 0.7f);
        
        // Test feather physics weights
        TArray<float> FeatherWeights = AnimInstance->GetFeatherPhysicsWeights();
        TestTrue("FeatherPhysicsWeights should not be empty", FeatherWeights.Num() > 0);
        
        for (float Weight : FeatherWeights)
        {
            TestTrue("Feather weight should be >= 0", Weight >= 0.0f);
            TestTrue("Feather weight should be <= 1", Weight <= 1.0f);
        }
        
        // Test wing bone transform
        FTransform WingTransform = AnimInstance->GetWingBoneTransform();
        TestFalse("WingTransform should not be identity when wind is applied", WingTransform.Equals(FTransform::Identity));
        
        // Test sudden wind force
        FVector SuddenWind = FVector(1000.0f, 0.0f, 0.0f);
        AnimInstance->ApplySuddenWindForce(SuddenWind);
        
        // Should gradually apply the wind force (test after a few updates)
        for (int32 i = 0; i < 5; i++)
        {
            AnimInstance->UpdatePhysicsAnimation(1.0f / 60.0f);
        }
        
        FVector NewWindInfluence = AnimInstance->GetWindInfluence();
        TestTrue("Wind influence should change after sudden wind", !NewWindInfluence.Equals(WindInfluence, 0.1f));
    }
    
    return true;
}

// Error handling tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdAnimInstanceErrorTest, "BirdDive.Animation.BirdAnimInstance.ErrorHandling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FBirdAnimInstanceErrorTest::RunTest(const FString& Parameters)
{
    // Given: Create AnimInstance
    UBirdAnimInstance* AnimInstance = NewObject<UBirdAnimInstance>();
    TestNotNull("AnimInstance should be created", AnimInstance);
    
    if (AnimInstance)
    {
        AnimInstance->InitializeForTest();
        
        // When: Try invalid state transition
        bool TransitionResult = AnimInstance->TryTransitionToState("InvalidState");
        
        // Then: Should fail gracefully
        TestFalse("Invalid transition should fail", TransitionResult);
        
        FString LastError = AnimInstance->GetLastError();
        TestTrue("Error message should contain info about invalid state", LastError.Contains("Invalid"));
        
        // State should remain unchanged
        FString CurrentState = AnimInstance->GetCurrentStateName();
        TestEqual("State should remain Idle after failed transition", CurrentState, FString("Idle"));
        
        // Test boundary values
        AnimInstance->SetPhysicsBlendWeight(-1.0f); // Invalid value
        float ClampedWeight = AnimInstance->GetPhysicsBlendWeight();
        TestTrue("Negative weight should be clamped to 0", ClampedWeight >= 0.0f);
        
        AnimInstance->SetPhysicsBlendWeight(2.0f); // Invalid value
        ClampedWeight = AnimInstance->GetPhysicsBlendWeight();
        TestTrue("Weight > 1 should be clamped to 1", ClampedWeight <= 1.0f);
    }
    
    return true;
}