#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BirdDiveTypes.h"
#include "BirdDiveDelegates.generated.h"

// ゲーム状態変更時のデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameState, NewGameState);

// スコア更新時のデリゲート  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, int32, NewScore);

// 速度変更時のデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedChanged, float, NewSpeed);

// 着地時のデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLandingCompleted, const FLandingResult&, Result);

// 警告表示用デリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWarningTriggered, const FText&, WarningMessage);

/**
 * デリゲート管理用クラス
 * UE5でデリゲートを使用するために必要
 */
UCLASS()
class BIRDDIVE_API UBirdDiveDelegates : public UObject
{
	GENERATED_BODY()

public:
	UBirdDiveDelegates() = default;
};