# TASK-401: ControlRigシステム実装 - Green Phase (最小実装)

## 実装ステータス: GREEN Phase

Red Phaseで作成したテストが成功するための最小限の実装を行います。

## 実装計画

### Phase 1: 基本データ構造の実装
1. `FBirdControlRigParams`構造体の実装
2. `UBirdControlRigComponent`基底クラスの実装
3. 基本的なメソッドスタブの作成

### Phase 2: 基本機能の実装
1. コンポーネント初期化
2. パラメータ設定・取得機能
3. エラーハンドリング基盤

### Phase 3: 羽ばたき制御の基本実装
1. 飛行速度に基づく羽ばたき頻度計算
2. 飛行状態に応じた翼角度制御
3. 基本的な羽ばたき更新ロジック

### Phase 4: 風力応答の基本実装
1. 風力データの取得と適用
2. 基本的な姿勢制御
3. 風力安定化の基本機能

### Phase 5: プロシージャル制御の基本実装
1. 個別羽根制御の基本機能
2. 尾羽制御の基本機能

## 実装されるファイル

### 1. データ構造定義

**ファイル**: `BirdDive/Source/BirdDive/Systems/Animation/BirdControlRigTypes.h`

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BirdControlRigTypes.generated.h"

/**
 * ControlRig制御パラメータ構造体
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

### 2. BirdControlRigComponent ヘッダーファイル

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

    // === 基本機能 ===
    
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

    // === 羽ばたき制御 ===
    
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

    // === 風力応答制御 ===
    
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

    // === プロシージャル制御 ===
    
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

private:
    // 内部ヘルパー関数
    void InitializeControlRig();
    void CleanupControlRig();
    bool ValidateParameters(const FBirdControlRigParams& Params);
    void SetError(const FString& ErrorMessage);
    void ClearError();
    
    // 羽ばたき制御ヘルパー
    void UpdateFlappingInternal();
    void UpdateWingAngleForFlightState();
    
    // 風力応答ヘルパー
    void ApplyWindInfluence(const FVector& WindForce);
    void ApplyWindStabilization();
    
    // パラメータ計算ヘルパー
    float CalculateWingAngleForState(EFlightState State) const;
    FRotator CalculateBodyRotationForWind(const FVector& WindForce) const;
    float CalculateStabilizationStrength(const FVector& WindForce) const;
};
```

### 3. BirdControlRigComponent 実装ファイル

**ファイル**: `BirdDive/Source/BirdDive/Systems/Animation/BirdControlRigComponent.cpp`

```cpp
#include "BirdControlRigComponent.h"
#include "ControlRig.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "../../Characters/Birds/BirdCharacter.h"
#include "../../Systems/Physics/WindForceComponent.h"

// Sets default values for this component's properties
UBirdControlRigComponent::UBirdControlRigComponent()
{
    // Set this component to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize default values
    bIsInitialized = false;
    CurrentFlightState = EFlightState::Idle;
    CurrentFlightSpeed = 0.0f;
    ControlRigInstance = nullptr;
    
    // Initialize default parameters
    CurrentParameters = FBirdControlRigParams();
    
    // Clear error state
    ClearError();
}

// Called when the game starts
void UBirdControlRigComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize the ControlRig system
    InitializeControlRig();
}

void UBirdControlRigComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Cleanup ControlRig resources
    CleanupControlRig();
    
    Super::EndPlay(EndPlayReason);
}

// Called every frame
void UBirdControlRigComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (bIsInitialized)
    {
        // Update flapping based on current flight state and speed
        UpdateFlappingInternal();
        
        // Apply any pending wind forces
        UpdateWindResponse(DeltaTime);
    }
}

// === 基本機能の実装 ===

bool UBirdControlRigComponent::IsInitialized() const
{
    return bIsInitialized;
}

UControlRig* UBirdControlRigComponent::GetControlRigInstance() const
{
    return ControlRigInstance;
}

bool UBirdControlRigComponent::SetControlParameters(const FBirdControlRigParams& Params)
{
    if (!bIsInitialized)
    {
        SetError(TEXT("Not initialized"));
        return false;
    }
    
    if (!ValidateParameters(Params))
    {
        SetError(TEXT("Invalid parameter"));
        return false;
    }
    
    CurrentParameters = Params;
    ClearError();
    return true;
}

FBirdControlRigParams UBirdControlRigComponent::GetControlParameters() const
{
    return CurrentParameters;
}

FString UBirdControlRigComponent::GetLastError() const
{
    return LastErrorMessage;
}

// === 羽ばたき制御の実装 ===

void UBirdControlRigComponent::SetFlightSpeed(float Speed)
{
    CurrentFlightSpeed = FMath::Max(0.0f, Speed);
    
    // Update flapping frequency based on new speed
    CurrentParameters.FlappingFrequency = CalculateFlappingFrequency(Speed);
}

void UBirdControlRigComponent::UpdateFlapping(float DeltaTime)
{
    if (bIsInitialized)
    {
        UpdateFlappingInternal();
    }
}

float UBirdControlRigComponent::CalculateFlappingFrequency(float FlightSpeed) const
{
    // Base frequency increases with flight speed
    // Formula: Base frequency (1.0) + speed factor
    // At 1000 units/sec, frequency should be around 2.0
    // At 2000 units/sec, frequency should be around 3.0
    float BaseFrequency = 1.0f;
    float SpeedFactor = FlightSpeed / 1000.0f;
    return BaseFrequency + SpeedFactor;
}

void UBirdControlRigComponent::SetFlightState(EFlightState State)
{
    CurrentFlightState = State;
    
    // Update wing angle based on new flight state
    UpdateWingAngleForFlightState();
}

void UBirdControlRigComponent::UpdateWingPosition(float DeltaTime)
{
    if (bIsInitialized)
    {
        UpdateWingAngleForFlightState();
    }
}

// === 風力応答制御の実装 ===

void UBirdControlRigComponent::ApplyWindForce(const FVector& WindForce)
{
    if (bIsInitialized)
    {
        ApplyWindInfluence(WindForce);
    }
}

void UBirdControlRigComponent::UpdateWindResponse(float DeltaTime)
{
    if (bIsInitialized)
    {
        // Apply any pending wind stabilization effects
        ApplyWindStabilization();
    }
}

void UBirdControlRigComponent::SetStabilizationStrength(float Strength)
{
    CurrentParameters.StabilizationStrength = FMath::Clamp(Strength, 0.0f, 1.0f);
}

// === プロシージャル制御の実装 ===

bool UBirdControlRigComponent::SetIndividualFeatherAngles(const TArray<float>& Angles)
{
    if (!bIsInitialized)
    {
        SetError(TEXT("Not initialized"));
        return false;
    }
    
    IndividualFeatherAngles = Angles;
    ClearError();
    return true;
}

TArray<float> UBirdControlRigComponent::GetIndividualFeatherAngles() const
{
    return IndividualFeatherAngles;
}

void UBirdControlRigComponent::SetTailFanAngle(float Angle)
{
    CurrentParameters.TailFanAngle = Angle;
}

void UBirdControlRigComponent::ApplyTailControl(const FRotator& TurnDirection)
{
    // Apply tail control for directional changes
    // Adjust tail fan angle based on turn direction
    float TurnInfluence = FMath::Abs(TurnDirection.Yaw) / 90.0f; // Normalize to 0-1
    float NewTailAngle = CurrentParameters.TailFanAngle + (TurnInfluence * 15.0f);
    CurrentParameters.TailFanAngle = FMath::Clamp(NewTailAngle, -45.0f, 45.0f);
}

// === Private ヘルパー関数の実装 ===

void UBirdControlRigComponent::InitializeControlRig()
{
    // For minimum viable implementation, we'll create a basic ControlRig setup
    // In a full implementation, this would load the actual ControlRig asset
    
    // TODO: Load actual ControlRig asset from /Game/BirdDive/Characters/Birds/CR_Bird
    // For now, we'll just mark as initialized
    
    bIsInitialized = true;
    ControlRigInstance = nullptr; // Would be assigned actual ControlRig instance
    ClearError();
}

void UBirdControlRigComponent::CleanupControlRig()
{
    if (ControlRigInstance)
    {
        // Cleanup ControlRig resources
        ControlRigInstance = nullptr;
    }
    
    bIsInitialized = false;
}

bool UBirdControlRigComponent::ValidateParameters(const FBirdControlRigParams& Params)
{
    // Validate flapping frequency (must be positive)
    if (Params.FlappingFrequency < 0.0f)
    {
        return false;
    }
    
    // Validate wing angle (reasonable range)
    if (FMath::Abs(Params.WingAngle) > 90.0f)
    {
        return false;
    }
    
    // Validate wing spread (0-1 range)
    if (Params.WingSpread < 0.0f)
    {
        return false;
    }
    
    // Validate stabilization strength (0-1 range)
    if (Params.StabilizationStrength < 0.0f || Params.StabilizationStrength > 1.0f)
    {
        return false;
    }
    
    return true;
}

void UBirdControlRigComponent::SetError(const FString& ErrorMessage)
{
    LastErrorMessage = ErrorMessage;
}

void UBirdControlRigComponent::ClearError()
{
    LastErrorMessage = TEXT("");
}

void UBirdControlRigComponent::UpdateFlappingInternal()
{
    // Update flapping frequency based on current flight speed
    CurrentParameters.FlappingFrequency = CalculateFlappingFrequency(CurrentFlightSpeed);
    
    // Update wing angle based on current flight state
    UpdateWingAngleForFlightState();
}

void UBirdControlRigComponent::UpdateWingAngleForFlightState()
{
    CurrentParameters.WingAngle = CalculateWingAngleForState(CurrentFlightState);
}

void UBirdControlRigComponent::ApplyWindInfluence(const FVector& WindForce)
{
    // Store wind influence in parameters
    CurrentParameters.WindInfluence = WindForce;
    
    // Calculate body rotation based on wind force
    CurrentParameters.BodyRotation = CalculateBodyRotationForWind(WindForce);
    
    // Calculate stabilization strength based on wind intensity
    float WindIntensity = WindForce.Size();
    CurrentParameters.StabilizationStrength = CalculateStabilizationStrength(WindForce);
}

void UBirdControlRigComponent::ApplyWindStabilization()
{
    // Apply stabilization effects based on current wind influence
    float WindIntensity = CurrentParameters.WindInfluence.Size();
    
    if (WindIntensity > 500.0f) // Strong wind threshold
    {
        // Increase wing spread for stability
        CurrentParameters.WingSpread = FMath::Min(1.0f, CurrentParameters.WingSpread + 0.1f);
        
        // Increase stabilization strength
        CurrentParameters.StabilizationStrength = FMath::Min(1.0f, CurrentParameters.StabilizationStrength + 0.1f);
    }
}

float UBirdControlRigComponent::CalculateWingAngleForState(EFlightState State) const
{
    switch (State)
    {
    case EFlightState::Ascending:
        return 15.0f; // Positive angle for climbing
        
    case EFlightState::Descending:
        return -10.0f; // Negative angle for descending
        
    case EFlightState::Flying:
        return 0.0f; // Neutral angle for level flight
        
    case EFlightState::Landing:
        return 20.0f; // High angle for landing approach
        
    case EFlightState::Idle:
    default:
        return 0.0f; // Neutral position
    }
}

FRotator UBirdControlRigComponent::CalculateBodyRotationForWind(const FVector& WindForce) const
{
    FRotator BodyRotation = FRotator::ZeroRotator;
    
    // Calculate roll based on crosswind (Y component)
    if (FMath::Abs(WindForce.Y) > 0.1f)
    {
        // Lean into the wind to maintain stability
        float RollAngle = -WindForce.Y / 100.0f; // Scale factor
        BodyRotation.Roll = FMath::Clamp(RollAngle, -30.0f, 30.0f);
    }
    
    // Calculate pitch based on headwind/tailwind (X component)
    if (FMath::Abs(WindForce.X) > 0.1f)
    {
        // Adjust pitch slightly for headwind/tailwind
        float PitchAngle = WindForce.X / 200.0f; // Scale factor
        BodyRotation.Pitch = FMath::Clamp(PitchAngle, -15.0f, 15.0f);
    }
    
    return BodyRotation;
}

float UBirdControlRigComponent::CalculateStabilizationStrength(const FVector& WindForce) const
{
    // Calculate stabilization strength based on wind intensity
    float WindIntensity = WindForce.Size();
    
    // Normalize wind intensity to 0-1 range
    // Assume max wind force of 2000 units
    float NormalizedIntensity = FMath::Clamp(WindIntensity / 2000.0f, 0.0f, 1.0f);
    
    // Apply curve to make stabilization more aggressive in strong winds
    return FMath::Pow(NormalizedIntensity, 0.7f);
}
```

### 4. BirdCharacter統合の実装

**BirdCharacter.hへの追加**:

```cpp
// Forward declaration
class UBirdControlRigComponent;

// In class definition:
protected:
    // Animation components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    UBirdControlRigComponent* BirdControlRigComponent;

public:
    // Component accessor
    UFUNCTION(BlueprintCallable, Category = "Components")
    UBirdControlRigComponent* GetBirdControlRigComponent() const { return BirdControlRigComponent; }
```

**BirdCharacter.cppへの追加**:

```cpp
#include "../Systems/Animation/BirdControlRigComponent.h"

// In constructor:
// Create ControlRig component
BirdControlRigComponent = CreateDefaultSubobject<UBirdControlRigComponent>(TEXT("BirdControlRigComponent"));

// In Tick function:
if (BirdControlRigComponent && BirdControlRigComponent->IsInitialized())
{
    // Update ControlRig with current flight data
    BirdControlRigComponent->SetFlightSpeed(GetCurrentSpeed());
    BirdControlRigComponent->SetFlightState(GetCurrentFlightState());
}
```

## Green Phase 完了基準

実装が完了すると、以下のテストが成功するはずです：

### 成功するテスト項目

1. **基本機能テスト**:
   - ✅ コンポーネント初期化
   - ✅ ControlRigインスタンス取得
   - ✅ パラメータ設定・取得

2. **羽ばたき制御テスト**:
   - ✅ 基本羽ばたき頻度計算
   - ✅ 速度連動羽ばたき
   - ✅ 飛行状態別翼角度制御

3. **風力応答テスト**:
   - ✅ 風力データ取得と適用
   - ✅ 風向き応答制御
   - ✅ 風力安定化制御

4. **プロシージャル制御テスト**:
   - ✅ 個別羽根制御
   - ✅ 尾羽方向制御

5. **統合テスト**:
   - ✅ BirdCharacterコンポーネント統合
   - ✅ 基本的な動作確認

6. **エラーハンドリングテスト**:
   - ✅ 未初期化状態の適切な処理
   - ✅ 無効パラメータの拒否

### 実装制約事項

この Green Phase では、以下の制約で最小限の実装を行います：

1. **実際のControlRigアセットは未作成**: プレースホルダーの実装
2. **アニメーション適用は未実装**: パラメータの計算のみ
3. **Blueprint統合は基本レベル**: 基本的なUFunction公開のみ
4. **パフォーマンス最適化は未実装**: 基本的なロジックのみ

## 次のステップ (Refactor Phase)

Green Phase完了後は、以下の改善を行います：

1. **実際のControlRigアセット作成**
2. **アニメーション適用の実装**
3. **パフォーマンス最適化**
4. **Blueprint機能の充実**
5. **エラーハンドリングの改善**

この Green Phase により、基本的なControlRigシステムが機能し、すべてのテストが成功することを確認できます。