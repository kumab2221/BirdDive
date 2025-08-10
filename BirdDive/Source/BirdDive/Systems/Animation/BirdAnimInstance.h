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

    // Configuration constants - exposed to Blueprint for easy tweaking
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float FlightStartSpeedThreshold = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float ClimbingSpeedThreshold = 1000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float ClimbingVerticalVelocityThreshold = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float DivingVerticalVelocityThreshold = -200.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float LandingSpeedThreshold = 200.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Machine Config")
    float LandingDistanceThreshold = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Space Config")
    float SlowFlightSpeedThreshold = 500.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Space Config")
    float MediumFlightSpeedThreshold = 1000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Config")
    int32 FeatherCount = 10;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Config")
    float WindInfluenceScale = 1000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Config")
    float DirectionalBlendScale = 0.8f;

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
    FString GetCurrentAnimStateName() const { return CurrentStateName; }
    
    // Non-UFUNCTION version for tests
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

    // Test helper for initialization
    UFUNCTION(BlueprintCallable, Category = "Testing")
    void InitializeForTest() { NativeInitializeAnimation(); }

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
    
    // Performance optimization
    mutable bool bCachedBlendWeightsValid = false;
    mutable TArray<float> OptimizedBlendWeights;

    // Physics animation data
    FTransform CachedWingBoneTransform;

    // Initialize state machine
    void InitializeStateMachine();
    
    // Validate state transition
    bool IsValidStateTransition(const FString& FromState, const FString& ToState) const;
    
    // State transition condition checkers
    bool ShouldTransitionToFlying() const;
    bool ShouldTransitionToClimbing() const;
    bool ShouldTransitionToDiving() const;
    bool ShouldTransitionToLanding() const;
    bool ShouldTransitionToIdle() const;
    
    // Error handling utilities
    bool ValidateRequiredReferences() const;
    void LogAnimationWarning(const FString& Message) const;
    void LogAnimationError(const FString& Message) const;
    
    // Performance optimization helpers
    void InvalidateBlendWeightCache();
    const TArray<float>& GetCachedBlendWeights() const;
    void UpdateWingBoneTransform(float WindMagnitude);
};