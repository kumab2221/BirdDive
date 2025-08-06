#include "FlightStateManager.h"
#include "Engine/World.h"
#include "../../BirdDive.h"

UFlightStateManager::UFlightStateManager()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Initialize state
    CurrentState = EFlightState::Idle;
    PreviousState = EFlightState::Idle;
    StateHistory.Reserve(MAX_STATE_HISTORY);
}

void UFlightStateManager::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize state history
    StateHistory.Empty();
    StateHistory.Add(CurrentState);
}

bool UFlightStateManager::TransitionToState(EFlightState NewState)
{
    // Check if transition is valid
    if (!ValidateStateTransition(CurrentState, NewState))
    {
        UE_LOG(LogBirdDive, Warning, TEXT("Invalid state transition from %d to %d"), 
               static_cast<int32>(CurrentState), static_cast<int32>(NewState));
        return false;
    }

    // Same state - no transition needed
    if (CurrentState == NewState)
    {
        return true;
    }

    // Process state exit
    ProcessStateExit(CurrentState);

    // Update states
    PreviousState = CurrentState;
    CurrentState = NewState;

    // Update history
    StateHistory.Add(CurrentState);
    if (StateHistory.Num() > MAX_STATE_HISTORY)
    {
        StateHistory.RemoveAt(0);
    }

    // Process state entry
    ProcessStateEntry(CurrentState);

    // Fire event
    OnFlightStateChanged.Broadcast(PreviousState, CurrentState);

    UE_LOG(LogBirdDive, Log, TEXT("Flight state changed from %d to %d"), 
           static_cast<int32>(PreviousState), static_cast<int32>(CurrentState));

    return true;
}

bool UFlightStateManager::CanTransitionTo(EFlightState TargetState) const
{
    return ValidateStateTransition(CurrentState, TargetState);
}

void UFlightStateManager::ForceSetState(EFlightState NewState)
{
    UE_LOG(LogBirdDive, Warning, TEXT("Force setting flight state from %d to %d"), 
           static_cast<int32>(CurrentState), static_cast<int32>(NewState));

    // Process state exit
    ProcessStateExit(CurrentState);

    // Update states
    PreviousState = CurrentState;
    CurrentState = NewState;

    // Update history
    StateHistory.Add(CurrentState);
    if (StateHistory.Num() > MAX_STATE_HISTORY)
    {
        StateHistory.RemoveAt(0);
    }

    // Process state entry
    ProcessStateEntry(CurrentState);

    // Fire event
    OnFlightStateChanged.Broadcast(PreviousState, CurrentState);
}

bool UFlightStateManager::ValidateStateTransition(EFlightState From, EFlightState To) const
{
    // Allow same state
    if (From == To)
    {
        return true;
    }

    // Define valid transitions
    switch (From)
    {
        case EFlightState::Idle:
            return To == EFlightState::Flying;

        case EFlightState::Flying:
            return To == EFlightState::Gliding || 
                   To == EFlightState::Turning || 
                   To == EFlightState::Landing ||
                   To == EFlightState::Idle; // Emergency stop

        case EFlightState::Gliding:
            return To == EFlightState::Flying ||
                   To == EFlightState::Turning ||
                   To == EFlightState::Landing ||
                   To == EFlightState::Idle; // Emergency stop

        case EFlightState::Turning:
            return To == EFlightState::Flying ||
                   To == EFlightState::Gliding ||
                   To == EFlightState::Landing ||
                   To == EFlightState::Idle; // Emergency stop

        case EFlightState::Landing:
            return To == EFlightState::Landed ||
                   To == EFlightState::Flying || // Abort landing
                   To == EFlightState::Idle; // Emergency stop

        case EFlightState::Landed:
            return To == EFlightState::Idle ||
                   To == EFlightState::Flying; // Take off again

        default:
            return false;
    }
}

void UFlightStateManager::ProcessStateEntry(EFlightState State)
{
    switch (State)
    {
        case EFlightState::Idle:
            UE_LOG(LogBirdDive, Log, TEXT("Entered Idle state"));
            break;

        case EFlightState::Flying:
            UE_LOG(LogBirdDive, Log, TEXT("Entered Flying state"));
            break;

        case EFlightState::Gliding:
            UE_LOG(LogBirdDive, Log, TEXT("Entered Gliding state"));
            break;

        case EFlightState::Turning:
            UE_LOG(LogBirdDive, Log, TEXT("Entered Turning state"));
            break;

        case EFlightState::Landing:
            UE_LOG(LogBirdDive, Log, TEXT("Entered Landing state"));
            break;

        case EFlightState::Landed:
            UE_LOG(LogBirdDive, Log, TEXT("Entered Landed state"));
            break;
    }
}

void UFlightStateManager::ProcessStateExit(EFlightState State)
{
    switch (State)
    {
        case EFlightState::Idle:
            UE_LOG(LogBirdDive, VeryVerbose, TEXT("Exited Idle state"));
            break;

        case EFlightState::Flying:
            UE_LOG(LogBirdDive, VeryVerbose, TEXT("Exited Flying state"));
            break;

        case EFlightState::Gliding:
            UE_LOG(LogBirdDive, VeryVerbose, TEXT("Exited Gliding state"));
            break;

        case EFlightState::Turning:
            UE_LOG(LogBirdDive, VeryVerbose, TEXT("Exited Turning state"));
            break;

        case EFlightState::Landing:
            UE_LOG(LogBirdDive, VeryVerbose, TEXT("Exited Landing state"));
            break;

        case EFlightState::Landed:
            UE_LOG(LogBirdDive, VeryVerbose, TEXT("Exited Landed state"));
            break;
    }
}