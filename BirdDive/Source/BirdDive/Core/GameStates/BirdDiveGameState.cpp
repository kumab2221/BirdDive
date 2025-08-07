#include "BirdDiveGameState.h"
#include "../../BirdDive.h"
#include "../GameModes/BirdDiveGameMode.h"

ABirdDiveGameState::ABirdDiveGameState()
{
	// Initialize default values
	CurrentScore = 0;
	FinalScore = 0;
	PlayerCount = 0;
	
	// Initialize timer values
	ElapsedTime = 0.0f;
	TimeLimit = 60.0f; // デフォルト1分
	bTimerActive = false;
	
	// Initialize game state
	CurrentGameState = EGameState::Menu;
	
	// Enable ticking
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_DuringPhysics;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDiveGameState constructed"));
}

void ABirdDiveGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// タイマーがアクティブの場合のみ時間を進める
	if (bTimerActive)
	{
		ElapsedTime += DeltaTime;
		CheckTimeLimit();
	}
}

void ABirdDiveGameState::SetCurrentScore(int32 NewScore)
{
	CurrentScore = NewScore;
	
	// Broadcast score change event
	OnScoreChanged.Broadcast(CurrentScore);
	
	UE_LOG(LogBirdDive, Log, TEXT("Score set to: %d"), CurrentScore);
}

void ABirdDiveGameState::SetFinalScore(int32 NewFinalScore)
{
	FinalScore = NewFinalScore;
	
	UE_LOG(LogBirdDive, Log, TEXT("Final score set to: %d"), FinalScore);
}

void ABirdDiveGameState::StartGameTimer()
{
	bTimerActive = true;
	
	UE_LOG(LogBirdDive, Log, TEXT("Game timer started"));
}

void ABirdDiveGameState::StopGameTimer()
{
	bTimerActive = false;
	
	UE_LOG(LogBirdDive, Log, TEXT("Game timer stopped at %.2f seconds"), ElapsedTime);
}

void ABirdDiveGameState::ResetGameTimer()
{
	ElapsedTime = 0.0f;
	bTimerActive = false;
	
	UE_LOG(LogBirdDive, Log, TEXT("Game timer reset"));
}

void ABirdDiveGameState::SetTimeLimit(float NewTimeLimit)
{
	TimeLimit = FMath::Max(0.0f, NewTimeLimit);
	
	UE_LOG(LogBirdDive, Log, TEXT("Time limit set to %.2f seconds"), TimeLimit);
}

void ABirdDiveGameState::SetCurrentGameState(EGameState NewState)
{
	EGameState OldState = CurrentGameState;
	CurrentGameState = NewState;
	
	// Broadcast state change event (using parameter name from existing delegate)
	OnGameStateChanged.Broadcast(NewState);
	
	UE_LOG(LogBirdDive, Log, TEXT("Game state changed from %d to %d"), (int32)OldState, (int32)NewState);
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

void ABirdDiveGameState::CheckTimeLimit()
{
	if (ElapsedTime >= TimeLimit && CurrentGameState == EGameState::Playing)
	{
		// 制限時間到達時に自動でゲーム終了
		StopGameTimer();
		SetCurrentGameState(EGameState::GameOver);
		
		// GameModeに終了を通知
		if (ABirdDiveGameMode* BirdGameMode = Cast<ABirdDiveGameMode>(GetWorld()->GetAuthGameMode()))
		{
			// タイムアップによる終了結果を作成
			FLandingResult TimeUpResult;
			TimeUpResult.Quality = ELandingQuality::Miss;
			TimeUpResult.DistanceFromCenter = 999.0f; // 大きな値でMiss判定
			TimeUpResult.LandingSpeed = 0.0f;
			
			BirdGameMode->EndGame(TimeUpResult);
		}
		
		UE_LOG(LogBirdDive, Log, TEXT("Game ended due to time limit"));
	}
}