#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Engine.h"
#include "../../BirdDiveTypes.h"
#include "FlightStateManager.generated.h"

// Delegate for flight state changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFlightStateChanged, EFlightState, OldState, EFlightState, NewState);

/**
 * Flight State Manager Component
 * 
 * Manages flight state transitions and validates state changes for bird characters.
 * Provides events for state changes and ensures valid state machine behavior.
 */
UCLASS(ClassGroup=(Flight), meta=(BlueprintSpawnableComponent))
class BIRDDIVE_API UFlightStateManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UFlightStateManager();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Transition to a new flight state
     * @param NewState - Target flight state
     * @return True if transition was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Flight State")
    bool TransitionToState(EFlightState NewState);

    /**
     * Check if transition to target state is valid
     * @param TargetState - State to check transition to
     * @return True if transition is allowed
     */
    UFUNCTION(BlueprintCallable, Category = "Flight State")
    bool CanTransitionTo(EFlightState TargetState) const;

    /**
     * Get current flight state
     * @return Current flight state
     */
    UFUNCTION(BlueprintCallable, Category = "Flight State")
    EFlightState GetCurrentState() const { return CurrentState; }

    /**
     * Get previous flight state
     * @return Previous flight state
     */
    UFUNCTION(BlueprintCallable, Category = "Flight State")
    EFlightState GetPreviousState() const { return PreviousState; }

    /**
     * Force set state (for emergency situations)
     * @param NewState - State to force
     */
    UFUNCTION(BlueprintCallable, Category = "Flight State")
    void ForceSetState(EFlightState NewState);

    // Event fired when flight state changes
    UPROPERTY(BlueprintAssignable, Category = "Flight State")
    FOnFlightStateChanged OnFlightStateChanged;

protected:
    /**
     * Validate state transition
     * @param From - Current state
     * @param To - Target state
     * @return True if transition is valid
     */
    bool ValidateStateTransition(EFlightState From, EFlightState To) const;

    /**
     * Process state entry logic
     * @param State - State being entered
     */
    void ProcessStateEntry(EFlightState State);

    /**
     * Process state exit logic
     * @param State - State being exited
     */
    void ProcessStateExit(EFlightState State);

private:
    // Current flight state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight State", meta = (AllowPrivateAccess = "true"))
    EFlightState CurrentState = EFlightState::Idle;

    // Previous flight state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight State", meta = (AllowPrivateAccess = "true"))
    EFlightState PreviousState = EFlightState::Idle;

    // State transition history for debugging
    TArray<EFlightState> StateHistory;

    // Maximum history size
    static constexpr int32 MAX_STATE_HISTORY = 10;
};