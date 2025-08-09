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

    // Test helper functions
    UFUNCTION(BlueprintCallable, Category = "Test", CallInEditor)
    void InitializeForTest() { BeginPlay(); }

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