#include "BirdDiveGameMode.h"
#include "../../BirdDive.h"
#include "../GameStates/BirdDiveGameState.h"
#include "../../Characters/Birds/BirdCharacter.h"

ABirdDiveGameMode::ABirdDiveGameMode()
{
	// Set default classes - minimal implementation
	GameStateClass = ABirdDiveGameState::StaticClass();
	DefaultPawnClass = ABirdCharacter::StaticClass();
	
	// Initialize state
	bHasBegunPlay = false;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDiveGameMode constructed"));
}

void ABirdDiveGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	bHasBegunPlay = true;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDiveGameMode BeginPlay"));
}

void ABirdDiveGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bHasBegunPlay = false;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDiveGameMode EndPlay"));
	
	Super::EndPlay(EndPlayReason);
}

