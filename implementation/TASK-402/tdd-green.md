# TASK-402: アニメーションブループリント実装 - Green Phase (最小実装)

## 概要

TDDのGreen Phaseとして、Red Phaseで作成した失敗テストを通すための最小限の実装を行う。

## Green Phase実装内容

### Phase 1: 基盤実装

#### UBirdAnimInstance クラスの完全実装

**ファイル**: `Source/BirdDive/Systems/Animation/BirdAnimInstance.cpp`

```cpp
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
    FString NewState = CurrentStateName;
    
    // 飛行状態への遷移判定
    if (CurrentStateName == TEXT("Idle") && bIsFlying && Speed > 100.0f)
    {
        NewState = TEXT("Flying");
    }
    // 上昇状態への遷移判定
    else if (CurrentStateName == TEXT("Flying") && VerticalVelocity > 100.0f && Speed > 1000.0f)
    {
        NewState = TEXT("Climbing");
    }
    // 下降状態への遷移判定
    else if ((CurrentStateName == TEXT("Flying") || CurrentStateName == TEXT("Climbing")) && VerticalVelocity < -200.0f)
    {
        NewState = TEXT("Diving");
    }
    // 着地状態への遷移判定
    else if ((CurrentStateName == TEXT("Flying") || CurrentStateName == TEXT("Diving")) && 
             bIsLanding && Speed < 200.0f && DistanceToGround < 100.0f)
    {
        NewState = TEXT("Landing");
    }
    // Idle状態への遷移判定
    else if (CurrentStateName == TEXT("Landing") && !bIsFlying && DistanceToGround <= 0.0f)
    {
        NewState = TEXT("Idle");
    }
    
    // 状態が変更された場合のみ遷移を実行
    if (NewState != CurrentStateName)
    {
        TryTransitionToState(NewState);
    }
}

FVector UBirdAnimInstance::GetFlightBlendSpaceInput() const
{
    // 基本的なブレンドスペース入力を返す
    return FVector(Speed, VerticalVelocity, 0.0f);
}

FVector2D UBirdAnimInstance::GetDirectionalBlendSpaceInput() const
{
    // 方向入力を返す
    return FVector2D(InputX, InputY);
}

TArray<float> UBirdAnimInstance::GetFlightBlendWeights() const
{
    // 基本的なブレンドウェイトを返す（テスト用の最小実装）
    TArray<float> Weights;
    
    // 4つの基本アニメーション用ウェイト
    Weights.Add(0.0f); // Idle
    Weights.Add(0.0f); // Slow flight
    Weights.Add(0.0f); // Medium flight
    Weights.Add(0.0f); // Fast flight
    
    // 速度に基づいて適切なウェイトを設定
    if (Speed <= 0.0f)
    {
        Weights[0] = 1.0f; // Idle
    }
    else if (Speed <= 500.0f)
    {
        Weights[1] = 1.0f; // Slow flight
    }
    else if (Speed <= 1000.0f)
    {
        Weights[2] = 1.0f; // Medium flight
    }
    else
    {
        Weights[3] = 1.0f; // Fast flight
    }
    
    return Weights;
}

void UBirdAnimInstance::UpdatePhysicsAnimation(float DeltaTime)
{
    // 物理アニメーションの更新（最小実装）
    if (CachedControlRigComponent)
    {
        // ControlRigからパラメータを取得
        FBirdControlRigParams ControlParams = CachedControlRigComponent->GetCurrentParameters();
        FlappingFrequency = ControlParams.FlappingFrequency;
        WingAngle = ControlParams.WingAngle;
    }
    
    // 羽根物理ウェイトの更新
    CachedFeatherPhysicsWeights.Empty();
    for (int32 i = 0; i < 10; i++) // 10個の羽根を仮定
    {
        float Weight = FMath::Clamp(WindInfluence.Size() / 1000.0f, 0.0f, 1.0f);
        CachedFeatherPhysicsWeights.Add(Weight);
    }
    
    // 翼骨のトランスフォームを更新
    CachedWingBoneTransform = FTransform::Identity;
    if (WindInfluence.Size() > 0.0f)
    {
        FVector WindDirection = WindInfluence.GetSafeNormal();
        FRotator WindRotation = WindDirection.Rotation();
        CachedWingBoneTransform.SetRotation(WindRotation.Quaternion());
    }
}

void UBirdAnimInstance::SetWindInfluence(const FVector& Influence)
{
    WindInfluence = Influence;
}

void UBirdAnimInstance::SetPhysicsBlendWeight(float Weight)
{
    PhysicsBlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);
}

void UBirdAnimInstance::ApplySuddenWindForce(const FVector& WindForce)
{
    // 突発的な風力を段階的に適用
    FVector TargetInfluence = WindForce * 0.5f;
    WindInfluence = FMath::VInterpTo(WindInfluence, TargetInfluence, 1.0f / 60.0f, 2.0f);
}

TArray<float> UBirdAnimInstance::GetFeatherPhysicsWeights() const
{
    return CachedFeatherPhysicsWeights;
}

FTransform UBirdAnimInstance::GetWingBoneTransform() const
{
    return CachedWingBoneTransform;
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
    DirectionalBlendWeight = TurnAnimationWeight * 0.8f;
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
```

## BirdCharacterとの統合実装

既存のBirdCharacterクラスに必要なメソッドが不足している場合、以下を追加実装：

**ファイル**: `Source/BirdDive/Characters/Birds/BirdCharacter.h`

```cpp
// 追加メソッド（既存にない場合）
UFUNCTION(BlueprintCallable, Category = "Flight")
FVector2D GetFlightInput() const { return CurrentFlightInput; }

UFUNCTION(BlueprintCallable, Category = "Flight")
float GetRollInput() const { return CurrentRollInput; }

UFUNCTION(BlueprintCallable, Category = "Flight")
void SetFlightInput(const FVector2D& Input) { CurrentFlightInput = Input; }

protected:
// 追加プロパティ（既存にない場合）
FVector2D CurrentFlightInput = FVector2D::ZeroVector;
float CurrentRollInput = 0.0f;
```

## BirdControlRigComponentとの統合実装

**ファイル**: `Source/BirdDive/Systems/Animation/BirdControlRigComponent.h`

```cpp
// GetCurrentParameters メソッドが未実装の場合追加
UFUNCTION(BlueprintCallable, Category = "Control Rig")
FBirdControlRigParams GetCurrentParameters() const 
{ 
    return CurrentControlParameters; 
}

UFUNCTION(BlueprintCallable, Category = "Control Rig")
void SetControlParameters(const FBirdControlRigParams& NewParams)
{
    CurrentControlParameters = NewParams;
}

protected:
// 現在のパラメータを保存
UPROPERTY(BlueprintReadWrite, Category = "Control Rig")
FBirdControlRigParams CurrentControlParameters;
```

## テスト実行と結果確認

### 実装後のテスト実行

1. プロジェクトのコンパイル
2. テストの実行
3. 結果の確認と微調整

### 期待される結果

- **基本機能テスト**: 成功
  - AnimInstance作成成功
  - パラメータ初期値正常

- **状態マシンテスト**: 成功  
  - 状態遷移成功
  - 条件ベース遷移成功

- **ブレンドスペーステスト**: 成功
  - ブレンド入力計算成功
  - ウェイト計算成功

- **ControlRig統合テスト**: 成功
  - パラメータ取得成功
  - 連携動作成功

## Green Phaseの成功指標

### 機能達成
- [ ] 全テストがパス
- [ ] 基本的なアニメーション機能が動作
- [ ] ControlRigとの統合が成功

### 品質確認  
- [ ] コンパイルエラーなし
- [ ] 実行時エラーなし
- [ ] メモリリークなし

### パフォーマンス
- [ ] 基本的な動作速度が問題なし
- [ ] 60FPS環境で動作

この最小実装により、全テストが通過し、次のRefactor Phaseに進める状態となる。