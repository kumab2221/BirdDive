#include "BirdDivePlayerController.h"
#include "../../BirdDive.h"
#include "Components/InputComponent.h"

ABirdDivePlayerController::ABirdDivePlayerController()
{
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController constructed"));
}

void ABirdDivePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController BeginPlay"));
}

void ABirdDivePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController EndPlay"));
	
	Super::EndPlay(EndPlayReason);
}

void ABirdDivePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Minimal input setup - just ensure InputComponent exists
	if (InputComponent)
	{
		UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController input setup complete"));
	}
}

void ABirdDivePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController possessed pawn: %s"), 
		InPawn ? *InPawn->GetName() : TEXT("null"));
}

void ABirdDivePlayerController::OnUnPossess()
{
	UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController unpossessing pawn"));
	
	Super::OnUnPossess();
}