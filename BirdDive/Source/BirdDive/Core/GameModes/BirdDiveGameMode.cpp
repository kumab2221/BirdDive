#include "BirdDiveGameMode.h"
#include "../../BirdDive.h"
#include "../GameStates/BirdDiveGameState.h"
#include "../../Characters/Birds/BirdCharacter.h"
#include "../../Systems/Scoring/BirdDiveScoreCalculator.h"

ABirdDiveGameMode::ABirdDiveGameMode()
{
	// Set default classes
	GameStateClass = ABirdDiveGameState::StaticClass();
	DefaultPawnClass = ABirdCharacter::StaticClass();
	
	// Initialize state
	bHasBegunPlay = false;
	CurrentGameState = EGameState::Menu;
	CurrentDifficulty = EDifficultyLevel::Normal;
	ScoreCalculator = nullptr;
	
	// Initialize difficulty settings
	InitializeDifficultySettings();
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDiveGameMode constructed"));
}

void ABirdDiveGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	bHasBegunPlay = true;
	
	// Create score calculator
	ScoreCalculator = NewObject<UBirdDiveScoreCalculator>(this);
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDiveGameMode BeginPlay"));
}

void ABirdDiveGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bHasBegunPlay = false;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdDiveGameMode EndPlay"));
	
	Super::EndPlay(EndPlayReason);
}

void ABirdDiveGameMode::SetDifficulty(EDifficultyLevel NewDifficulty)
{
	CurrentDifficulty = NewDifficulty;
	DifficultySettings = CreateDifficultySettings(NewDifficulty);
	
	UE_LOG(LogBirdDive, Log, TEXT("Difficulty set to %d"), (int32)NewDifficulty);
}

FDifficultySettings ABirdDiveGameMode::GetCurrentDifficultySettings() const
{
	return DifficultySettings;
}

void ABirdDiveGameMode::ApplyDifficultySettings()
{
	// 物理システム等への設定適用は統合テストで実装予定
	UE_LOG(LogBirdDive, Log, TEXT("Difficulty settings applied"));
}

void ABirdDiveGameMode::StartGame(EDifficultyLevel Difficulty)
{
	// 難易度設定
	SetDifficulty(Difficulty);
	ApplyDifficultySettings();
	
	// ゲーム状態をPlayingに変更
	SetCurrentGameState(EGameState::Playing);
	
	// ゲーム状態にタイマー開始を通知
	if (ABirdDiveGameState* BirdGameState = Cast<ABirdDiveGameState>(GameState))
	{
		BirdGameState->SetCurrentScore(0); // スコアリセット
		BirdGameState->SetTimeLimit(DifficultySettings.TimeLimit); // 制限時間設定
		BirdGameState->ResetGameTimer(); // タイマーリセット
		BirdGameState->StartGameTimer(); // タイマー開始
	}
	
	UE_LOG(LogBirdDive, Log, TEXT("Game started with difficulty %d"), (int32)Difficulty);
}

void ABirdDiveGameMode::EndGame(const FLandingResult& LandingResult)
{
	// ゲーム状態をGameOverに変更
	SetCurrentGameState(EGameState::GameOver);
	
	// タイマー停止
	if (ABirdDiveGameState* BirdGameState = Cast<ABirdDiveGameState>(GameState))
	{
		BirdGameState->StopGameTimer();
	}
	
	// 最終スコア計算
	if (ScoreCalculator && IsValid(ScoreCalculator))
	{
		int32 FinalScore = ScoreCalculator->CalculateFinalScore(LandingResult, CurrentDifficulty);
		
		if (ABirdDiveGameState* BirdGameState = Cast<ABirdDiveGameState>(GameState))
		{
			BirdGameState->SetCurrentScore(FinalScore);
			BirdGameState->SetFinalScore(FinalScore);
		}
		
		UE_LOG(LogBirdDive, Log, TEXT("Game ended with final score: %d"), FinalScore);
	}
	
	// 後でResults状態への遷移を実装予定
}

void ABirdDiveGameMode::InitializeDifficultySettings()
{
	DifficultySettings = CreateDifficultySettings(CurrentDifficulty);
}

void ABirdDiveGameMode::SetCurrentGameState(EGameState NewState)
{
	EGameState OldState = CurrentGameState;
	CurrentGameState = NewState;
	
	// デリゲート通知
	OnGameStateChanged.Broadcast(NewState);
	
	UE_LOG(LogBirdDive, Log, TEXT("Game state changed from %d to %d"), (int32)OldState, (int32)NewState);
}

FDifficultySettings ABirdDiveGameMode::CreateDifficultySettings(EDifficultyLevel Difficulty) const
{
	FDifficultySettings Settings;
	
	switch (Difficulty)
	{
		case EDifficultyLevel::Easy:
			Settings.WindStrengthMultiplier = 0.5f;
			Settings.GravityMultiplier = 0.8f;
			Settings.ScoreMultiplier = 1.0f;
			Settings.TimeLimit = 90.0f;
			break;
			
		case EDifficultyLevel::Normal:
			Settings.WindStrengthMultiplier = 1.0f;
			Settings.GravityMultiplier = 1.0f;
			Settings.ScoreMultiplier = 1.2f;
			Settings.TimeLimit = 60.0f;
			break;
			
		case EDifficultyLevel::Hard:
			Settings.WindStrengthMultiplier = 1.5f;
			Settings.GravityMultiplier = 1.2f;
			Settings.ScoreMultiplier = 1.5f;
			Settings.TimeLimit = 45.0f;
			break;
			
		default:
			// デフォルトはNormal設定
			Settings.WindStrengthMultiplier = 1.0f;
			Settings.GravityMultiplier = 1.0f;
			Settings.ScoreMultiplier = 1.2f;
			Settings.TimeLimit = 60.0f;
			break;
	}
	
	return Settings;
}

