#include "BirdAnimInstance.h"
#include "../../Characters/Birds/BirdCharacter.h"
#include "BirdControlRigComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBirdAnimInstance::UBirdAnimInstance()
{
    // 基本パラメータの初期化
    Speed = 0.0f;
    VerticalVelocity = 0.0f;
    bIsFlying = false;
    bIsLanding = false;
    
    InputX = 0.0f;
    InputY = 0.0f;
    RollInput = 0.0f;
    
    DistanceToGround = 1000.0f;
    
    FlappingFrequency = 0.0f;
    WingAngle = 0.0f;
    WindInfluence = FVector::ZeroVector;
    PhysicsBlendWeight = 0.0f;
    
    CurrentStateName = TEXT("Idle");
    TurnAnimationWeight = 0.0f;
    DirectionalBlendWeight = 0.0f;
    
    CachedBirdCharacter = nullptr;
    CachedControlRigComponent = nullptr;
    PreviousFlightState = EFlightState::Idle;
}

void UBirdAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    // キャラクター参照の取得
    CachedBirdCharacter = Cast<ABirdCharacter>(GetOwningActor());
    
    // ControlRigコンポーネントの取得
    if (CachedBirdCharacter)
    {
        CachedControlRigComponent = CachedBirdCharacter->GetBirdControlRigComponent();
    }
    
    // 状態マシンの初期化
    InitializeStateMachine();
}

void UBirdAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);
    
    // 各パラメータの更新
    UpdateMovementParameters();
    UpdateFlightStateParameters();
    UpdateInputParameters();
    UpdateControlRigParameters();
    UpdateBlendSpaceParameters();
}

UBirdControlRigComponent* UBirdAnimInstance::GetControlRigComponent() const
{
    return CachedControlRigComponent;
}

void UBirdAnimInstance::SetCurrentState(const FString& StateName)
{
    if (IsValidStateTransition(CurrentStateName, StateName))
    {
        CurrentStateName = StateName;
        LastError.Empty();
    }
    else
    {
        LastError = FString::Printf(TEXT("Invalid state transition from %s to %s"), *CurrentStateName, *StateName);
    }
}

bool UBirdAnimInstance::TryTransitionToState(const FString& StateName)
{
    if (IsValidStateTransition(CurrentStateName, StateName))
    {
        CurrentStateName = StateName;
        LastError.Empty();
        return true;
    }
    else
    {
        LastError = FString::Printf(TEXT("Invalid state transition from %s to %s"), *CurrentStateName, *StateName);
        return false;
    }
}

void UBirdAnimInstance::UpdateStateMachine(float DeltaTime)
{
    if (!ValidateRequiredReferences())
    {
        LogAnimationError(TEXT("Failed to validate required references for state machine update"));
        return;
    }
    
    const FString PreviousState = CurrentStateName;
    FString NewState = CurrentStateName;
    
    // Check for state transitions based on current state and conditions
    if (CurrentStateName == TEXT("Idle") && ShouldTransitionToFlying())
    {
        NewState = TEXT("Flying");
    }
    else if (CurrentStateName == TEXT("Flying") && ShouldTransitionToClimbing())
    {
        NewState = TEXT("Climbing");
    }
    else if ((CurrentStateName == TEXT("Flying") || CurrentStateName == TEXT("Climbing")) && ShouldTransitionToDiving())
    {
        NewState = TEXT("Diving");
    }
    else if ((CurrentStateName == TEXT("Flying") || CurrentStateName == TEXT("Diving")) && ShouldTransitionToLanding())
    {
        NewState = TEXT("Landing");
    }
    else if (CurrentStateName == TEXT("Landing") && ShouldTransitionToIdle())
    {
        NewState = TEXT("Idle");
    }
    
    // Perform state transition if needed
    if (NewState != CurrentStateName)
    {
        if (TryTransitionToState(NewState))
        {
            UE_LOG(LogAnimation, Log, TEXT("BirdAnimInstance: State transition %s -> %s"), *PreviousState, *NewState);
        }
    }
}

FVector UBirdAnimInstance::GetFlightBlendSpaceInput() const
{
    return FVector(Speed, VerticalVelocity, 0.0f);
}

FVector2D UBirdAnimInstance::GetDirectionalBlendSpaceInput() const
{
    return FVector2D(InputX, InputY);
}

TArray<float> UBirdAnimInstance::GetFlightBlendWeights() const
{
    return GetCachedBlendWeights();
}

void UBirdAnimInstance::UpdatePhysicsAnimation(float DeltaTime)
{
    // Validate references first
    if (!ValidateRequiredReferences())
    {
        LogAnimationWarning(TEXT("Cannot update physics animation - missing required references"));
        return;
    }
    
    // Update ControlRig parameters
    if (CachedControlRigComponent)
    {
        const FBirdControlRigParams& ControlParams = CachedControlRigComponent->GetCurrentParameters();
        FlappingFrequency = ControlParams.FlappingFrequency;
        WingAngle = ControlParams.WingAngle;
    }
    
    // Update feather physics weights with optimized memory usage
    const float WindMagnitude = WindInfluence.Size();
    const float NormalizedWeight = FMath::Clamp(WindMagnitude / WindInfluenceScale, 0.0f, 1.0f);
    
    CachedFeatherPhysicsWeights.Empty(FeatherCount);
    CachedFeatherPhysicsWeights.Reserve(FeatherCount);
    
    for (int32 i = 0; i < FeatherCount; i++)
    {
        CachedFeatherPhysicsWeights.Add(NormalizedWeight);
    }
    
    // Update wing bone transform using helper method
    UpdateWingBoneTransform(WindMagnitude);
}

void UBirdAnimInstance::SetWindInfluence(const FVector& Influence)
{
    WindInfluence = Influence;
}

void UBirdAnimInstance::SetPhysicsBlendWeight(float Weight)
{
    PhysicsBlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);
}

TArray<float> UBirdAnimInstance::GetFeatherPhysicsWeights() const
{
    return CachedFeatherPhysicsWeights;
}

FTransform UBirdAnimInstance::GetWingBoneTransform() const
{
    return CachedWingBoneTransform;
}

void UBirdAnimInstance::ApplySuddenWindForce(const FVector& WindForce)
{
    // 突発的な風力を段階的に適用
    FVector TargetInfluence = WindForce * 0.5f;
    WindInfluence = FMath::VInterpTo(WindInfluence, TargetInfluence, 1.0f / 60.0f, 2.0f);
}

// プライベートメソッドの実装
void UBirdAnimInstance::UpdateMovementParameters()
{
    if (!CachedBirdCharacter)
        return;
    
    // 速度の取得と更新
    if (UCharacterMovementComponent* MovementComp = CachedBirdCharacter->GetCharacterMovement())
    {
        FVector Velocity = MovementComp->Velocity;
        Speed = Velocity.Size();
        VerticalVelocity = Velocity.Z;
    }
}

void UBirdAnimInstance::UpdateFlightStateParameters()
{
    if (!CachedBirdCharacter)
        return;
    
    // 飛行状態の取得
    EFlightState CurrentFlightState = CachedBirdCharacter->GetFlightState();
    
    bIsFlying = (CurrentFlightState == EFlightState::Flying || 
                CurrentFlightState == EFlightState::Gliding ||
                CurrentFlightState == EFlightState::Climbing ||
                CurrentFlightState == EFlightState::Diving);
    
    bIsLanding = (CurrentFlightState == EFlightState::Landing);
    
    PreviousFlightState = CurrentFlightState;
}

void UBirdAnimInstance::UpdateInputParameters()
{
    if (!CachedBirdCharacter)
        return;
    
    // 入力パラメータの取得（BirdCharacterから）
    FVector2D FlightInput = CachedBirdCharacter->GetFlightInput();
    InputX = FlightInput.X;
    InputY = FlightInput.Y;
    
    // ロール入力の取得
    RollInput = CachedBirdCharacter->GetRollInput();
}

void UBirdAnimInstance::UpdateControlRigParameters()
{
    if (!CachedControlRigComponent)
        return;
    
    // ControlRigパラメータの取得
    FBirdControlRigParams ControlParams = CachedControlRigComponent->GetCurrentParameters();
    FlappingFrequency = ControlParams.FlappingFrequency;
    WingAngle = ControlParams.WingAngle;
}

void UBirdAnimInstance::UpdateBlendSpaceParameters()
{
    // 旋回アニメーションウェイトの計算
    float InputMagnitude = FVector2D(InputX, InputY).Size();
    TurnAnimationWeight = FMath::Clamp(InputMagnitude, 0.0f, 1.0f);
    
    // 方向ブレンドウェイトの計算
    DirectionalBlendWeight = TurnAnimationWeight * DirectionalBlendScale;
    
    // Invalidate blend weight cache when parameters change
    InvalidateBlendWeightCache();
}

void UBirdAnimInstance::InitializeStateMachine()
{
    // 有効な状態の定義
    ValidStates.Empty();
    ValidStates.Add(TEXT("Idle"), true);
    ValidStates.Add(TEXT("Flying"), true);
    ValidStates.Add(TEXT("Landing"), true);
    ValidStates.Add(TEXT("Gliding"), true);
    ValidStates.Add(TEXT("Climbing"), true);
    ValidStates.Add(TEXT("Diving"), true);
    
    // 状態遷移の定義
    StateTransitions.Empty();
    
    // Idleからの遷移
    TArray<FString> IdleTransitions;
    IdleTransitions.Add(TEXT("Flying"));
    StateTransitions.Add(TEXT("Idle"), IdleTransitions);
    
    // Flyingからの遷移
    TArray<FString> FlyingTransitions;
    FlyingTransitions.Add(TEXT("Idle"));
    FlyingTransitions.Add(TEXT("Landing"));
    FlyingTransitions.Add(TEXT("Climbing"));
    FlyingTransitions.Add(TEXT("Diving"));
    FlyingTransitions.Add(TEXT("Gliding"));
    StateTransitions.Add(TEXT("Flying"), FlyingTransitions);
    
    // Landingからの遷移
    TArray<FString> LandingTransitions;
    LandingTransitions.Add(TEXT("Idle"));
    LandingTransitions.Add(TEXT("Flying"));
    StateTransitions.Add(TEXT("Landing"), LandingTransitions);
    
    // Climbingからの遷移
    TArray<FString> ClimbingTransitions;
    ClimbingTransitions.Add(TEXT("Flying"));
    ClimbingTransitions.Add(TEXT("Diving"));
    ClimbingTransitions.Add(TEXT("Gliding"));
    StateTransitions.Add(TEXT("Climbing"), ClimbingTransitions);
    
    // Divingからの遷移
    TArray<FString> DivingTransitions;
    DivingTransitions.Add(TEXT("Flying"));
    DivingTransitions.Add(TEXT("Landing"));
    DivingTransitions.Add(TEXT("Gliding"));
    StateTransitions.Add(TEXT("Diving"), DivingTransitions);
    
    // Glidingからの遷移
    TArray<FString> GlidingTransitions;
    GlidingTransitions.Add(TEXT("Flying"));
    GlidingTransitions.Add(TEXT("Landing"));
    GlidingTransitions.Add(TEXT("Climbing"));
    GlidingTransitions.Add(TEXT("Diving"));
    StateTransitions.Add(TEXT("Gliding"), GlidingTransitions);
}

bool UBirdAnimInstance::IsValidStateTransition(const FString& FromState, const FString& ToState) const
{
    // 同じ状態への遷移は常に有効
    if (FromState == ToState)
        return true;
    
    // 存在しない状態への遷移は無効
    if (!ValidStates.Contains(ToState))
    {
        return false;
    }
    
    // 遷移テーブルをチェック
    if (const TArray<FString>* AllowedTransitions = StateTransitions.Find(FromState))
    {
        return AllowedTransitions->Contains(ToState);
    }
    
    return false;
}

// State transition condition checkers
bool UBirdAnimInstance::ShouldTransitionToFlying() const
{
    return bIsFlying && Speed > FlightStartSpeedThreshold;
}

bool UBirdAnimInstance::ShouldTransitionToClimbing() const
{
    return VerticalVelocity > ClimbingVerticalVelocityThreshold && Speed > ClimbingSpeedThreshold;
}

bool UBirdAnimInstance::ShouldTransitionToDiving() const
{
    return VerticalVelocity < DivingVerticalVelocityThreshold;
}

bool UBirdAnimInstance::ShouldTransitionToLanding() const
{
    return bIsLanding && Speed < LandingSpeedThreshold && DistanceToGround < LandingDistanceThreshold;
}

bool UBirdAnimInstance::ShouldTransitionToIdle() const
{
    return !bIsFlying && DistanceToGround <= 0.0f;
}

// Error handling utilities
bool UBirdAnimInstance::ValidateRequiredReferences() const
{
    if (!CachedBirdCharacter)
    {
        return false;
    }
    return true;
}

void UBirdAnimInstance::LogAnimationWarning(const FString& Message) const
{
    UE_LOG(LogAnimation, Warning, TEXT("BirdAnimInstance: %s"), *Message);
}

void UBirdAnimInstance::LogAnimationError(const FString& Message) const
{
    UE_LOG(LogAnimation, Error, TEXT("BirdAnimInstance: %s"), *Message);
    const_cast<UBirdAnimInstance*>(this)->LastError = Message;
}

// Performance optimization helpers
void UBirdAnimInstance::InvalidateBlendWeightCache()
{
    bCachedBlendWeightsValid = false;
}

const TArray<float>& UBirdAnimInstance::GetCachedBlendWeights() const
{
    if (!bCachedBlendWeightsValid)
    {
        OptimizedBlendWeights.Empty();
        OptimizedBlendWeights.AddZeroed(4);
        
        // Determine which weight to set based on speed
        if (Speed <= 0.0f)
            OptimizedBlendWeights[0] = 1.0f; // Idle
        else if (Speed <= SlowFlightSpeedThreshold)
            OptimizedBlendWeights[1] = 1.0f; // Slow flight
        else if (Speed <= MediumFlightSpeedThreshold)
            OptimizedBlendWeights[2] = 1.0f; // Medium flight
        else
            OptimizedBlendWeights[3] = 1.0f; // Fast flight
        
        bCachedBlendWeightsValid = true;
    }
    
    return OptimizedBlendWeights;
}

void UBirdAnimInstance::UpdateWingBoneTransform(float WindMagnitude)
{
    CachedWingBoneTransform = FTransform::Identity;
    if (WindMagnitude > 0.0f)
    {
        FVector WindDirection = WindInfluence.GetSafeNormal();
        FRotator WindRotation = WindDirection.Rotation();
        CachedWingBoneTransform.SetRotation(WindRotation.Quaternion());
    }
}

