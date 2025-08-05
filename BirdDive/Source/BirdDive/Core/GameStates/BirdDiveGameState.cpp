#include "BirdDiveGameState.h"
#include "../../BirdDive.h"

ABirdDiveGameState::ABirdDiveGameState()
{
	// Initialize default values
	CurrentScore = 0;
	PlayerCount = 0;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDiveGameState constructed"));
}

void ABirdDiveGameState::SetCurrentScore(int32 NewScore)
{
	CurrentScore = NewScore;
	
	// Broadcast score change event
	OnScoreChanged.Broadcast(CurrentScore);
	
	UE_LOG(LogBirdDive, Log, TEXT("Score set to: %d"), CurrentScore);
}

void ABirdDiveGameState::AddPlayer()
{
	PlayerCount++;
	
	UE_LOG(LogBirdDive, Log, TEXT("Player added. Total players: %d"), PlayerCount);
}

void ABirdDiveGameState::RemovePlayer()
{
	if (PlayerCount > 0)
	{
		PlayerCount--;
	}
	
	UE_LOG(LogBirdDive, Log, TEXT("Player removed. Total players: %d"), PlayerCount);
}