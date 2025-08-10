# TASK-402: アニメーションブループリント実装 - Red Phase (失敗テスト実装)

## 概要

TDDのRed Phaseとして、まず失敗するテストを実装し、必要な機能が未実装であることを確認する。

## テスト環境セットアップ

### 必要なヘッダーファイル作成

#### UBirdAnimInstance クラス定義

**ファイル**: `Source/BirdDive/Systems/Animation/BirdAnimInstance.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../../BirdDiveTypes.h"
#include "BirdControlRigTypes.h"
#include "BirdAnimInstance.generated.h"

// Forward declarations
class UBirdControlRigComponent;
class ABirdCharacter;

/**
 * Animation Instance for Bird Character
 * 
 * Manages animation parameters, state machine, and blend spaces
 * for natural bird flight animation system.
 */
UCLASS()
class BIRDDIVE_API UBirdAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // Constructor
    UBirdAnimInstance();

protected:
    // Animation lifecycle
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTimeX) override;

public:
    // Core animation parameters
    UPROPERTY(BlueprintReadOnly, Category = "Animation Parameters")
    float Speed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Animation Parameters")
    float VerticalVelocity = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Animation Parameters")
    bool bIsFlying = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation Parameters")
    bool bIsLanding = false;

    // Input parameters
    UPROPERTY(BlueprintReadOnly, Category = "Input Parameters")
    float InputX = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Input Parameters")
    float InputY = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Input Parameters")
    float RollInput = 0.0f;

    // Distance to ground (for landing detection)
    UPROPERTY(BlueprintReadOnly, Category = "Environment")
    float DistanceToGround = 1000.0f;

    // ControlRig integration parameters
    UPROPERTY(BlueprintReadOnly, Category = "ControlRig")
    float FlappingFrequency = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "ControlRig")
    float WingAngle = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Physics")
    FVector WindInfluence = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Physics")
    float PhysicsBlendWeight = 0.0f;

    // State machine support
    UPROPERTY(BlueprintReadOnly, Category = "State Machine")
    FString CurrentStateName = "Idle";

    // Blend space support
    UPROPERTY(BlueprintReadOnly, Category = "Blend Space")
    float TurnAnimationWeight = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Blend Space")
    float DirectionalBlendWeight = 0.0f;

public:
    // Core parameter accessors
    UFUNCTION(BlueprintCallable, Category = "Animation")
    float GetSpeed() const { return Speed; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    float GetVerticalVelocity() const { return VerticalVelocity; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    bool GetIsFlying() const { return bIsFlying; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    bool GetIsLanding() const { return bIsLanding; }

    // Input accessors
    UFUNCTION(BlueprintCallable, Category = "Animation")
    float GetInputX() const { return InputX; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    float GetInputY() const { return InputY; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    float GetRollInput() const { return RollInput; }

    // ControlRig accessors
    UFUNCTION(BlueprintCallable, Category = "Animation")
    float GetFlappingFrequency() const { return FlappingFrequency; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    float GetWingAngle() const { return WingAngle; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    FVector GetWindInfluence() const { return WindInfluence; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    float GetPhysicsBlendWeight() const { return PhysicsBlendWeight; }

    // State machine methods
    UFUNCTION(BlueprintCallable, Category = "State Machine")
    FString GetCurrentStateName() const { return CurrentStateName; }

    UFUNCTION(BlueprintCallable, Category = "State Machine")
    void SetCurrentState(const FString& StateName);

    UFUNCTION(BlueprintCallable, Category = "State Machine")
    bool TryTransitionToState(const FString& StateName);

    UFUNCTION(BlueprintCallable, Category = "State Machine")
    void UpdateStateMachine(float DeltaTime);

    // Blend space methods
    UFUNCTION(BlueprintCallable, Category = "Blend Space")
    FVector GetFlightBlendSpaceInput() const;

    UFUNCTION(BlueprintCallable, Category = "Blend Space")
    FVector2D GetDirectionalBlendSpaceInput() const;

    UFUNCTION(BlueprintCallable, Category = "Blend Space")
    TArray<float> GetFlightBlendWeights() const;

    UFUNCTION(BlueprintCallable, Category = "Blend Space")
    float GetTurnAnimationWeight() const { return TurnAnimationWeight; }

    UFUNCTION(BlueprintCallable, Category = "Blend Space")
    float GetDirectionalBlendWeight() const { return DirectionalBlendWeight; }

    // Physics animation methods
    UFUNCTION(BlueprintCallable, Category = "Physics Animation")
    void UpdatePhysicsAnimation(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Physics Animation")
    void SetWindInfluence(const FVector& Influence);

    UFUNCTION(BlueprintCallable, Category = "Physics Animation")
    void SetPhysicsBlendWeight(float Weight);

    UFUNCTION(BlueprintCallable, Category = "Physics Animation")
    void ApplySuddenWindForce(const FVector& WindForce);

    UFUNCTION(BlueprintCallable, Category = "Physics Animation")
    TArray<float> GetFeatherPhysicsWeights() const;

    UFUNCTION(BlueprintCallable, Category = "Physics Animation")
    FTransform GetWingBoneTransform() const;

    // ControlRig component access
    UFUNCTION(BlueprintCallable, Category = "Components")
    UBirdControlRigComponent* GetControlRigComponent() const;

    // Test support methods
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetSpeed(float NewSpeed) { Speed = NewSpeed; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetVerticalVelocity(float NewVelocity) { VerticalVelocity = NewVelocity; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetIsFlying(bool NewFlying) { bIsFlying = NewFlying; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetIsLanding(bool NewLanding) { bIsLanding = NewLanding; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void SetDistanceToGround(float Distance) { DistanceToGround = Distance; }

    // Error handling
    UFUNCTION(BlueprintCallable, Category = "Error Handling")
    FString GetLastError() const { return LastError; }

protected:
    // Character reference
    UPROPERTY()
    ABirdCharacter* CachedBirdCharacter = nullptr;

    // ControlRig component reference
    UPROPERTY()
    UBirdControlRigComponent* CachedControlRigComponent = nullptr;

    // Internal state tracking
    EFlightState PreviousFlightState = EFlightState::Idle;
    
    // Error handling
    FString LastError;

    // Internal update methods
    void UpdateMovementParameters();
    void UpdateFlightStateParameters();
    void UpdateInputParameters();
    void UpdateControlRigParameters();
    void UpdateBlendSpaceParameters();

private:
    // State machine data
    TMap<FString, bool> ValidStates;
    TMap<FString, TArray<FString>> StateTransitions;
    
    // Blend space data
    TArray<float> CachedFlightBlendWeights;
    TArray<float> CachedFeatherPhysicsWeights;

    // Physics animation data
    FTransform CachedWingBoneTransform;

    // Initialize state machine
    void InitializeStateMachine();
    
    // Validate state transition
    bool IsValidStateTransition(const FString& FromState, const FString& ToState) const;
};
```

#### アニメーション関連型定義

**ファイル**: `Source/BirdDive/Systems/Animation/BirdAnimTypes.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BirdAnimTypes.generated.h"

/**
 * Animation state enumeration
 */
UENUM(BlueprintType)
enum class EBirdAnimState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Flying      UMETA(DisplayName = "Flying"),
    Landing     UMETA(DisplayName = "Landing"),
    Gliding     UMETA(DisplayName = "Gliding"),
    Climbing    UMETA(DisplayName = "Climbing"),
    Diving      UMETA(DisplayName = "Diving")
};

/**
 * Blend space parameters structure
 */
USTRUCT(BlueprintType)
struct BIRDDIVE_API FBirdBlendSpaceParams
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Space")
    float Speed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Space")
    float VerticalVelocity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Space")
    float InputX = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Space")
    float InputY = 0.0f;

    FBirdBlendSpaceParams()
    {
        Speed = 0.0f;
        VerticalVelocity = 0.0f;
        InputX = 0.0f;
        InputY = 0.0f;
    }
};

/**
 * Animation transition rule structure
 */
USTRUCT(BlueprintType)
struct BIRDDIVE_API FAnimTransitionRule
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    FString FromState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    FString ToState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float MinSpeed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float MaxSpeed = 10000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    bool bRequireFlying = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    bool bRequireLanding = false;

    FAnimTransitionRule()
    {
        FromState = "";
        ToState = "";
        MinSpeed = 0.0f;
        MaxSpeed = 10000.0f;
        bRequireFlying = false;
        bRequireLanding = false;
    }
};
```

## 失敗テストの実装

### テストファイル作成

**ファイル**: `Source/BirdDive/Tests/BirdAnimInstanceTest.cpp`

```cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "../Characters/Birds/BirdCharacter.h"
#include "../Systems/Animation/BirdAnimInstance.h"
#include "../Systems/Animation/BirdControlRigComponent.h"

// Test helper macros
#define BIRD_ANIM_TEST_CLASS(TestName) \
    IMPLEMENT_SIMPLE_AUTOMATION_TEST(F##TestName, "BirdDive.Animation.BirdAnimInstance." #TestName, \
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

// Test utilities
namespace BirdAnimTestUtils
{
    UBirdAnimInstance* CreateAnimInstance()
    {
        // This will fail initially as UBirdAnimInstance doesn't exist
        return NewObject<UBirdAnimInstance>();
    }

    UBirdAnimInstance* CreateInitializedAnimInstance()
    {
        UBirdAnimInstance* AnimInstance = CreateAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->NativeInitializeAnimation();
        }
        return AnimInstance;
    }

    ABirdCharacter* CreateBirdCharacterWithAnimBP()
    {
        // This will fail initially as integration doesn't exist
        ABirdCharacter* BirdCharacter = NewObject<ABirdCharacter>();
        // Animation BP assignment would happen here
        return BirdCharacter;
    }

    ABirdCharacter* CreateBirdCharacterWithControlRig()
    {
        ABirdCharacter* BirdCharacter = CreateBirdCharacterWithAnimBP();
        // ControlRig component should be added automatically
        return BirdCharacter;
    }

    UBirdAnimInstance* GetAnimInstance(ABirdCharacter* BirdCharacter)
    {
        if (!BirdCharacter || !BirdCharacter->GetMesh())
        {
            return nullptr;
        }
        return Cast<UBirdAnimInstance>(BirdCharacter->GetMesh()->GetAnimInstance());
    }

    void Tick(float DeltaTime)
    {
        // Simulate world tick
        if (GEngine && GEngine->GetWorld())
        {
            GEngine->GetWorld()->Tick(LEVELTICK_All, DeltaTime);
        }
    }
}

// UT-402-001: BirdAnimInstance基本機能テスト
BIRD_ANIM_TEST_CLASS(AnimInstanceInitialization)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: 新しいBirdAnimInstance
        UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::CreateAnimInstance();
        
        // When: AnimInstanceを初期化
        if (AnimInstance)
        {
            AnimInstance->NativeInitializeAnimation();
        }
        
        // Then: 正常に初期化される (これは失敗するはず)
        TestTrue("AnimInstance should be valid", IsValid(AnimInstance));
        TestNotNull("AnimInstance should not be null", AnimInstance);
        
        if (AnimInstance)
        {
            TestTrue("AnimInstance should be valid low level", AnimInstance->IsValidLowLevel());
            // 以下は初期実装では失敗するはず
            TestNotNull("OwningActor should not be null", AnimInstance->GetOwningActor());
            TestNotNull("SkelMeshComponent should not be null", AnimInstance->GetSkelMeshComponent());
        }

        return true;
    }
};

BIRD_ANIM_TEST_CLASS(DefaultParameterValues)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: 初期化済みAnimInstance
        UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::CreateInitializedAnimInstance();
        
        // When: 初期パラメータを確認
        // Then: デフォルト値が設定されている (初期実装では失敗)
        TestNotNull("AnimInstance should exist", AnimInstance);
        
        if (AnimInstance)
        {
            TestEqual("Speed should be 0.0f", AnimInstance->GetSpeed(), 0.0f);
            TestEqual("VerticalVelocity should be 0.0f", AnimInstance->GetVerticalVelocity(), 0.0f);
            TestFalse("IsFlying should be false", AnimInstance->GetIsFlying());
            TestFalse("IsLanding should be false", AnimInstance->GetIsLanding());
            TestNull("ControlRigComponent should be null initially", AnimInstance->GetControlRigComponent());
        }

        return true;
    }
};

BIRD_ANIM_TEST_CLASS(ControlRigComponentAccess)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: ControlRigComponentを持つBirdCharacter
        ABirdCharacter* BirdCharacter = BirdAnimTestUtils::CreateBirdCharacterWithControlRig();
        
        TestNotNull("BirdCharacter should exist", BirdCharacter);
        
        if (BirdCharacter)
        {
            UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::GetAnimInstance(BirdCharacter);
            
            // When: ControlRigComponentを取得
            UBirdControlRigComponent* ControlRigComponent = nullptr;
            if (AnimInstance)
            {
                ControlRigComponent = AnimInstance->GetControlRigComponent();
            }
            
            // Then: 有効なコンポーネントが取得される (初期実装では失敗)
            TestNotNull("AnimInstance should exist", AnimInstance);
            TestNotNull("ControlRigComponent should exist", ControlRigComponent);
            
            if (ControlRigComponent && BirdCharacter)
            {
                TestEqual("ControlRigComponent should match BirdCharacter's component", 
                    ControlRigComponent, BirdCharacter->GetBirdControlRigComponent());
            }
        }

        return true;
    }
};

// UT-402-002: パラメータ更新テスト
BIRD_ANIM_TEST_CLASS(SpeedParameterUpdate)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: BirdCharacterとAnimInstance
        ABirdCharacter* BirdCharacter = BirdAnimTestUtils::CreateBirdCharacterWithAnimBP();
        
        TestNotNull("BirdCharacter should exist", BirdCharacter);
        
        if (BirdCharacter)
        {
            UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::GetAnimInstance(BirdCharacter);
            TestNotNull("AnimInstance should exist", AnimInstance);
            
            if (AnimInstance)
            {
                // When: キャラクターの速度を設定
                FVector Velocity = FVector(1000.0f, 500.0f, 200.0f);
                BirdCharacter->GetCharacterMovement()->Velocity = Velocity;
                
                // AnimInstance更新
                AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
                
                // Then: 速度パラメータが更新される (初期実装では失敗)
                float ExpectedSpeed = Velocity.Size();
                TestEqual("Speed should be updated", AnimInstance->GetSpeed(), ExpectedSpeed);
                TestEqual("VerticalVelocity should be updated", AnimInstance->GetVerticalVelocity(), Velocity.Z);
            }
        }

        return true;
    }
};

BIRD_ANIM_TEST_CLASS(FlightStateParameterUpdate)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: BirdCharacterとAnimInstance
        ABirdCharacter* BirdCharacter = BirdAnimTestUtils::CreateBirdCharacterWithAnimBP();
        
        TestNotNull("BirdCharacter should exist", BirdCharacter);
        
        if (BirdCharacter)
        {
            UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::GetAnimInstance(BirdCharacter);
            TestNotNull("AnimInstance should exist", AnimInstance);
            
            if (AnimInstance)
            {
                // When: 飛行状態を設定
                BirdCharacter->SetFlightState(EFlightState::Flying);
                AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
                
                // Then: 飛行フラグが更新される (初期実装では失敗)
                TestTrue("IsFlying should be true", AnimInstance->GetIsFlying());
                
                // When: 着地状態を設定
                BirdCharacter->SetFlightState(EFlightState::Landing);
                AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
                
                // Then: 着地フラグが更新される (初期実装では失敗)
                TestTrue("IsLanding should be true", AnimInstance->GetIsLanding());
                TestFalse("IsFlying should be false", AnimInstance->GetIsFlying());
            }
        }

        return true;
    }
};

// UT-402-003: 状態マシンテスト
BIRD_ANIM_TEST_CLASS(BasicStateTransitions)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: AnimInstanceと状態マシン
        UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::CreateInitializedAnimInstance();
        
        TestNotNull("AnimInstance should exist", AnimInstance);
        
        if (AnimInstance)
        {
            // When: Idle状態から開始
            AnimInstance->SetCurrentState("Idle");
            
            // 飛行状態への遷移条件を満たす
            AnimInstance->SetSpeed(500.0f);
            AnimInstance->SetIsFlying(true);
            AnimInstance->UpdateStateMachine(1.0f / 60.0f);
            
            // Then: Flying状態に遷移 (初期実装では失敗)
            FString CurrentState = AnimInstance->GetCurrentStateName();
            TestEqual("Should transition to Flying state", CurrentState, FString("Flying"));
        }

        return true;
    }
};

BIRD_ANIM_TEST_CLASS(SpeedBasedTransitions)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: Flying状態のAnimInstance
        UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::CreateInitializedAnimInstance();
        
        TestNotNull("AnimInstance should exist", AnimInstance);
        
        if (AnimInstance)
        {
            AnimInstance->SetCurrentState("Flying");
            AnimInstance->SetIsFlying(true);
            
            // When: 高速飛行条件を満たす
            AnimInstance->SetSpeed(1500.0f);
            AnimInstance->SetVerticalVelocity(200.0f);
            AnimInstance->UpdateStateMachine(1.0f / 60.0f);
            
            // Then: Climbing状態に遷移 (初期実装では失敗)
            FString CurrentState = AnimInstance->GetCurrentStateName();
            TestEqual("Should transition to Climbing state", CurrentState, FString("Climbing"));
            
            // When: 降下条件を満たす
            AnimInstance->SetVerticalVelocity(-300.0f);
            AnimInstance->UpdateStateMachine(1.0f / 60.0f);
            
            // Then: Diving状態に遷移 (初期実装では失敗)
            CurrentState = AnimInstance->GetCurrentStateName();
            TestEqual("Should transition to Diving state", CurrentState, FString("Diving"));
        }

        return true;
    }
};

// UT-402-004: ブレンドスペーステスト
BIRD_ANIM_TEST_CLASS(FlightBlendSpaceTest)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: ブレンドスペースを持つAnimInstance
        UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::CreateInitializedAnimInstance();
        
        TestNotNull("AnimInstance should exist", AnimInstance);
        
        if (AnimInstance)
        {
            // When: 異なる速度・垂直速度を設定
            AnimInstance->SetSpeed(1000.0f);
            AnimInstance->SetVerticalVelocity(500.0f);
            
            // ブレンドスペース評価を実行
            FVector BlendInput = AnimInstance->GetFlightBlendSpaceInput();
            
            // Then: 適切なブレンド入力値が計算される (初期実装では失敗)
            TestEqual("BlendInput.X should match speed", BlendInput.X, 1000.0f);
            TestEqual("BlendInput.Y should match vertical velocity", BlendInput.Y, 500.0f);
            
            // ブレンドウェイトが適切に計算される (初期実装では失敗)
            TArray<float> BlendWeights = AnimInstance->GetFlightBlendWeights();
            TestTrue("BlendWeights should not be empty", BlendWeights.Num() > 0);
            
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
};

// UT-402-005: 物理アニメーション統合テスト
BIRD_ANIM_TEST_CLASS(ControlRigIntegration)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: ControlRigとAnimInstanceを持つBirdCharacter
        ABirdCharacter* BirdCharacter = BirdAnimTestUtils::CreateBirdCharacterWithControlRig();
        
        TestNotNull("BirdCharacter should exist", BirdCharacter);
        
        if (BirdCharacter)
        {
            UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::GetAnimInstance(BirdCharacter);
            UBirdControlRigComponent* ControlRigComponent = BirdCharacter->GetBirdControlRigComponent();
            
            TestNotNull("AnimInstance should exist", AnimInstance);
            TestNotNull("ControlRigComponent should exist", ControlRigComponent);
            
            if (AnimInstance && ControlRigComponent)
            {
                // When: ControlRigパラメータを設定
                FBirdControlRigParams ControlRigParams;
                ControlRigParams.FlappingFrequency = 2.5f;
                ControlRigParams.WingAngle = 30.0f;
                ControlRigComponent->SetControlParameters(ControlRigParams);
                
                // AnimInstance更新
                AnimInstance->NativeUpdateAnimation(1.0f / 60.0f);
                
                // Then: AnimInstanceがControlRigパラメータを取得 (初期実装では失敗)
                float FlappingFrequency = AnimInstance->GetFlappingFrequency();
                float WingAngle = AnimInstance->GetWingAngle();
                
                TestEqual("FlappingFrequency should match", FlappingFrequency, 2.5f);
                TestEqual("WingAngle should match", WingAngle, 30.0f);
            }
        }

        return true;
    }
};

BIRD_ANIM_TEST_CLASS(PhysicsAnimationApplication)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: 物理アニメーション対応AnimInstance
        UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::CreateInitializedAnimInstance();
        
        TestNotNull("AnimInstance should exist", AnimInstance);
        
        if (AnimInstance)
        {
            // When: 風力パラメータを設定
            FVector WindInfluence = FVector(100.0f, 50.0f, 0.0f);
            AnimInstance->SetWindInfluence(WindInfluence);
            AnimInstance->SetPhysicsBlendWeight(0.7f);
            
            // 物理アニメーションを更新
            AnimInstance->UpdatePhysicsAnimation(1.0f / 60.0f);
            
            // Then: 風力が羽根アニメーションに影響 (初期実装では失敗)
            TArray<float> FeatherPhysicsWeights = AnimInstance->GetFeatherPhysicsWeights();
            TestTrue("FeatherPhysicsWeights should not be empty", FeatherPhysicsWeights.Num() > 0);
            
            for (float Weight : FeatherPhysicsWeights)
            {
                TestTrue("Weight should be > 0", Weight > 0.0f);
                TestTrue("Weight should be <= 1", Weight <= 1.0f);
            }
            
            // 物理ブレンドが適用される (初期実装では失敗)
            float PhysicsBlendWeight = AnimInstance->GetPhysicsBlendWeight();
            TestEqual("PhysicsBlendWeight should match", PhysicsBlendWeight, 0.7f);
        }

        return true;
    }
};

// エラーハンドリングテスト
BIRD_ANIM_TEST_CLASS(InvalidStateTransition)
{
    bool RunTest(const FString& Parameters) override
    {
        // Given: 特定状態のAnimInstance
        UBirdAnimInstance* AnimInstance = BirdAnimTestUtils::CreateInitializedAnimInstance();
        
        TestNotNull("AnimInstance should exist", AnimInstance);
        
        if (AnimInstance)
        {
            AnimInstance->SetCurrentState("Flying");
            
            // When: 無効な遷移を試行
            bool TransitionResult = AnimInstance->TryTransitionToState("InvalidState");
            
            // Then: 適切にエラーハンドリング (初期実装では失敗)
            TestFalse("Transition should fail", TransitionResult);
            TestEqual("State should remain Flying", AnimInstance->GetCurrentStateName(), FString("Flying"));
            
            FString LastError = AnimInstance->GetLastError();
            TestTrue("Error should contain 'Invalid state'", LastError.Contains("Invalid state"));
        }

        return true;
    }
};
```

## 実装ファイルの基本構造

**ファイル**: `Source/BirdDive/Systems/Animation/BirdAnimInstance.cpp`

```cpp
#include "BirdAnimInstance.h"
#include "../../Characters/Birds/BirdCharacter.h"
#include "BirdControlRigComponent.h"

UBirdAnimInstance::UBirdAnimInstance()
{
    // コンストラクタは空実装 - テストが失敗することを確認
}

void UBirdAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    // 基本実装なし - テストが失敗することを確認
    // CachedBirdCharacter = Cast<ABirdCharacter>(GetOwningActor());
    // InitializeStateMachine();
}

void UBirdAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);
    
    // 更新実装なし - テストが失敗することを確認
    // UpdateMovementParameters();
    // UpdateFlightStateParameters();
    // UpdateInputParameters();
    // UpdateControlRigParameters();
    // UpdateBlendSpaceParameters();
}

// その他のメソッドは空実装または基本的なダミー実装
// すべてのテストが失敗することを確認するため

UBirdControlRigComponent* UBirdAnimInstance::GetControlRigComponent() const
{
    return nullptr; // テストが失敗するように
}

void UBirdAnimInstance::SetCurrentState(const FString& StateName)
{
    // 実装なし - テストが失敗することを確認
}

bool UBirdAnimInstance::TryTransitionToState(const FString& StateName)
{
    return false; // テストが失敗するように
}

void UBirdAnimInstance::UpdateStateMachine(float DeltaTime)
{
    // 実装なし - テストが失敗することを確認
}

FVector UBirdAnimInstance::GetFlightBlendSpaceInput() const
{
    return FVector::ZeroVector; // テストが失敗するように
}

FVector2D UBirdAnimInstance::GetDirectionalBlendSpaceInput() const
{
    return FVector2D::ZeroVector; // テストが失敗するように
}

TArray<float> UBirdAnimInstance::GetFlightBlendWeights() const
{
    return TArray<float>(); // 空配列でテストが失敗するように
}

void UBirdAnimInstance::UpdatePhysicsAnimation(float DeltaTime)
{
    // 実装なし - テストが失敗することを確認
}

void UBirdAnimInstance::SetWindInfluence(const FVector& Influence)
{
    // 実装なし - テストが失敗することを確認
}

void UBirdAnimInstance::SetPhysicsBlendWeight(float Weight)
{
    // 実装なし - テストが失敗することを確認
}

TArray<float> UBirdAnimInstance::GetFeatherPhysicsWeights() const
{
    return TArray<float>(); // 空配列でテストが失敗するように
}

FTransform UBirdAnimInstance::GetWingBoneTransform() const
{
    return FTransform::Identity; // テストが失敗するように
}

void UBirdAnimInstance::ApplySuddenWindForce(const FVector& WindForce)
{
    // 実装なし - テストが失敗することを確認
}

// プライベートメソッドも空実装
void UBirdAnimInstance::UpdateMovementParameters()
{
    // 実装なし
}

void UBirdAnimInstance::UpdateFlightStateParameters()
{
    // 実装なし
}

void UBirdAnimInstance::UpdateInputParameters()
{
    // 実装なし
}

void UBirdAnimInstance::UpdateControlRigParameters()
{
    // 実装なし
}

void UBirdAnimInstance::UpdateBlendSpaceParameters()
{
    // 実装なし
}

void UBirdAnimInstance::InitializeStateMachine()
{
    // 実装なし
}

bool UBirdAnimInstance::IsValidStateTransition(const FString& FromState, const FString& ToState) const
{
    return false; // テストが失敗するように
}
```

## ビルドファイル更新

**ファイル更新**: `Source/BirdDive/BirdDive.Build.cs`

```csharp
// Animation moduleを追加
PublicDependencyModuleNames.AddRange(new string[] { 
    "Core", 
    "CoreUObject", 
    "Engine", 
    "InputCore",
    "EnhancedInput",
    "AIModule",
    "Niagara",
    "ControlRig",
    "RigVM",
    "AnimationCore",
    "AnimGraph",
    "AnimGraphRuntime"  // 追加
});
```

## テスト実行

### コンパイルテスト

現時点では以下のエラーが予想される：
1. `UBirdAnimInstance` クラスが未定義
2. アニメーション関連のヘッダーが不足
3. ControlRig統合が未実装

### 期待される失敗結果

1. **基本機能テスト**: すべて失敗
   - AnimInstance の初期化が不完全
   - パラメータ更新が未実装

2. **状態マシンテスト**: すべて失敗
   - 状態遷移ロジックが未実装
   - 状態管理が未実装

3. **ブレンドスペーステスト**: すべて失敗
   - ブレンド計算が未実装
   - ウェイト計算が未実装

4. **物理統合テスト**: すべて失敗
   - ControlRig統合が未実装
   - 物理アニメーションが未実装

5. **エラーハンドリングテスト**: 部分的失敗
   - エラー検出は動作するが適切な処理が未実装

## 次のステップ

Red Phase完了後、Green Phaseで：
1. BirdAnimInstance クラスの完全実装
2. 状態マシンロジック実装
3. ブレンドスペースサポート実装
4. ControlRig統合実装
5. エラーハンドリング強化

このRed Phaseにより、すべての必要機能が未実装であることが確認され、TDDサイクルの基盤が確立される。