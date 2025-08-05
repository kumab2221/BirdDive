# TASK-102: データ構造・列挙型実装 - テスト実装 (RED フェーズ)

## 概要

TDD の RED フェーズとして、失敗するテストを実装します。実装がまだ存在しないため、これらのテストは全て失敗することが期待されます。

## テストファイル作成

### 1. テストファイル構造

```
BirdDive/Source/BirdDiveTests/
├── BirdDiveTypesTest.cpp           // 列挙型・構造体テスト
├── BirdDiveDataAssetsTest.cpp      // DataAsset テスト
└── BirdDiveDelegatesTest.cpp       // デリゲート テスト
```

## 実装されるテストファイル

### BirdDiveTypesTest.cpp

```cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "BirdDive/BirdDiveTypes.h"

// EDifficultyLevel テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEDifficultyLevelValuesTest, "BirdDive.Types.EDifficultyLevel.Values", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FEDifficultyLevelValuesTest::RunTest(const FString& Parameters)
{
    // Arrange & Act & Assert
    TestEqual("Easy should be 0", static_cast<int32>(EDifficultyLevel::Easy), 0);
    TestEqual("Normal should be 1", static_cast<int32>(EDifficultyLevel::Normal), 1);  
    TestEqual("Hard should be 2", static_cast<int32>(EDifficultyLevel::Hard), 2);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEDifficultyLevelDisplayNamesTest, "BirdDive.Types.EDifficultyLevel.DisplayNames",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FEDifficultyLevelDisplayNamesTest::RunTest(const FString& Parameters)
{
    // Arrange
    UEnum* DifficultyEnum = StaticEnum<EDifficultyLevel>();
    
    // Act & Assert
    TestNotNull("Difficulty enum should exist", DifficultyEnum);
    TestEqual("Easy display name", 
        DifficultyEnum->GetDisplayNameTextByValue(static_cast<int64>(EDifficultyLevel::Easy)).ToString(),
        TEXT("Easy"));
    TestEqual("Normal display name",
        DifficultyEnum->GetDisplayNameTextByValue(static_cast<int64>(EDifficultyLevel::Normal)).ToString(),
        TEXT("Normal"));
    TestEqual("Hard display name",
        DifficultyEnum->GetDisplayNameTextByValue(static_cast<int64>(EDifficultyLevel::Hard)).ToString(),
        TEXT("Hard"));
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEDifficultyLevelBlueprintTest, "BirdDive.Types.EDifficultyLevel.Blueprint",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FEDifficultyLevelBlueprintTest::RunTest(const FString& Parameters)
{
    // Arrange
    UEnum* DifficultyEnum = StaticEnum<EDifficultyLevel>();
    
    // Act & Assert
    TestNotNull("Difficulty enum should exist", DifficultyEnum);
    TestTrue("Should have BlueprintType metadata", DifficultyEnum->HasMetaData(TEXT("BlueprintType")));
    
    return true;
}

// EGameState テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEGameStateValuesTest, "BirdDive.Types.EGameState.Values",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FEGameStateValuesTest::RunTest(const FString& Parameters)
{
    // Arrange & Act & Assert
    TestEqual("Menu should be 0", static_cast<int32>(EGameState::Menu), 0);
    TestEqual("Playing should be 1", static_cast<int32>(EGameState::Playing), 1);
    TestEqual("Paused should be 2", static_cast<int32>(EGameState::Paused), 2);
    TestEqual("GameOver should be 3", static_cast<int32>(EGameState::GameOver), 3);
    TestEqual("Results should be 4", static_cast<int32>(EGameState::Results), 4);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEGameStateBlueprintTest, "BirdDive.Types.EGameState.Blueprint",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FEGameStateBlueprintTest::RunTest(const FString& Parameters)
{
    // Arrange
    UEnum* GameStateEnum = StaticEnum<EGameState>();
    
    // Act & Assert
    TestNotNull("GameState enum should exist", GameStateEnum);
    TestTrue("Should have BlueprintType metadata", GameStateEnum->HasMetaData(TEXT("BlueprintType")));
    
    return true;
}

// EFlightState テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEFlightStateValuesTest, "BirdDive.Types.EFlightState.Values",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FEFlightStateValuesTest::RunTest(const FString& Parameters)
{
    // Arrange & Act & Assert
    TestEqual("Idle should be 0", static_cast<int32>(EFlightState::Idle), 0);
    TestEqual("Flying should be 1", static_cast<int32>(EFlightState::Flying), 1);
    TestEqual("Gliding should be 2", static_cast<int32>(EFlightState::Gliding), 2);
    TestEqual("Turning should be 3", static_cast<int32>(EFlightState::Turning), 3);
    TestEqual("Landing should be 4", static_cast<int32>(EFlightState::Landing), 4);
    TestEqual("Landed should be 5", static_cast<int32>(EFlightState::Landed), 5);
    
    return true;
}

// ELandingQuality テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FELandingQualityValuesTest, "BirdDive.Types.ELandingQuality.Values",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FELandingQualityValuesTest::RunTest(const FString& Parameters)
{
    // Arrange & Act & Assert
    TestEqual("Miss should be 0", static_cast<int32>(ELandingQuality::Miss), 0);
    TestEqual("Poor should be 1", static_cast<int32>(ELandingQuality::Poor), 1);
    TestEqual("Good should be 2", static_cast<int32>(ELandingQuality::Good), 2);
    TestEqual("Perfect should be 3", static_cast<int32>(ELandingQuality::Perfect), 3);
    
    return true;
}

// FDifficultySettings テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFDifficultySettingsDefaultTest, "BirdDive.Types.FDifficultySettings.Default",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FFDifficultySettingsDefaultTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    FDifficultySettings Settings;
    
    // Assert
    TestEqual("WindStrengthMultiplier should be 1.0", Settings.WindStrengthMultiplier, 1.0f);
    TestEqual("GravityMultiplier should be 1.0", Settings.GravityMultiplier, 1.0f);
    TestEqual("ScoreMultiplier should be 1.0", Settings.ScoreMultiplier, 1.0f);
    TestEqual("TimeLimit should be 60.0", Settings.TimeLimit, 60.0f);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFDifficultySettingsAssignmentTest, "BirdDive.Types.FDifficultySettings.Assignment",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FFDifficultySettingsAssignmentTest::RunTest(const FString& Parameters)
{
    // Arrange
    FDifficultySettings Settings;
    const float TestWindMultiplier = 1.5f;
    const float TestTimeLimit = 90.0f;
    
    // Act
    Settings.WindStrengthMultiplier = TestWindMultiplier;
    Settings.TimeLimit = TestTimeLimit;
    
    // Assert
    TestEqual("WindStrengthMultiplier assignment", Settings.WindStrengthMultiplier, TestWindMultiplier);
    TestEqual("TimeLimit assignment", Settings.TimeLimit, TestTimeLimit);
    
    return true;
}

// FWindData テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFWindDataDefaultTest, "BirdDive.Types.FWindData.Default",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FFWindDataDefaultTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    FWindData WindData;
    
    // Assert
    TestEqual("Direction should be zero vector", WindData.Direction, FVector::ZeroVector);
    TestEqual("Strength should be 0.0", WindData.Strength, 0.0f);
    TestEqual("Turbulence should be 0.0", WindData.Turbulence, 0.0f);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFWindDataAssignmentTest, "BirdDive.Types.FWindData.Assignment",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FFWindDataAssignmentTest::RunTest(const FString& Parameters)
{
    // Arrange
    FWindData WindData;
    const FVector TestDirection = FVector(1.0f, 0.0f, 0.0f);
    const float TestStrength = 10.0f;
    
    // Act
    WindData.Direction = TestDirection;
    WindData.Strength = TestStrength;
    
    // Assert
    TestEqual("Direction assignment", WindData.Direction, TestDirection);
    TestEqual("Strength assignment", WindData.Strength, TestStrength);
    
    return true;
}

// FLandingResult テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFLandingResultDefaultTest, "BirdDive.Types.FLandingResult.Default",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FFLandingResultDefaultTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    FLandingResult Result;
    
    // Assert
    TestEqual("Quality should be Miss", Result.Quality, ELandingQuality::Miss);
    TestEqual("DistanceFromCenter should be 0.0", Result.DistanceFromCenter, 0.0f);
    TestEqual("LandingSpeed should be 0.0", Result.LandingSpeed, 0.0f);
    TestEqual("BaseScore should be 0", Result.BaseScore, 0);
    TestEqual("BonusScore should be 0", Result.BonusScore, 0);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFLandingResultAssignmentTest, "BirdDive.Types.FLandingResult.Assignment",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FFLandingResultAssignmentTest::RunTest(const FString& Parameters)
{
    // Arrange
    FLandingResult Result;
    const ELandingQuality TestQuality = ELandingQuality::Perfect;
    const float TestDistance = 2.5f;
    const int32 TestScore = 1000;
    
    // Act
    Result.Quality = TestQuality;
    Result.DistanceFromCenter = TestDistance;
    Result.BaseScore = TestScore;
    
    // Assert
    TestEqual("Quality assignment", Result.Quality, TestQuality);
    TestEqual("DistanceFromCenter assignment", Result.DistanceFromCenter, TestDistance);
    TestEqual("BaseScore assignment", Result.BaseScore, TestScore);
    
    return true;
}

// FGameSessionData テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFGameSessionDataDefaultTest, "BirdDive.Types.FGameSessionData.Default",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FFGameSessionDataDefaultTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    FGameSessionData SessionData;
    
    // Assert
    TestEqual("Difficulty should be Normal", SessionData.Difficulty, EDifficultyLevel::Normal);
    TestEqual("FinalScore should be 0", SessionData.FinalScore, 0);
    TestEqual("PlayTime should be 0.0", SessionData.PlayTime, 0.0f);
    
    return true;
}

// Blueprint 統合テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlueprintIntegrationTest, "BirdDive.Types.Blueprint.Integration",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBlueprintIntegrationTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    UStruct* DifficultySettingsStruct = FDifficultySettings::StaticStruct();
    UStruct* WindDataStruct = FWindData::StaticStruct();
    UStruct* LandingResultStruct = FLandingResult::StaticStruct();
    UStruct* GameSessionDataStruct = FGameSessionData::StaticStruct();
    
    // Assert - Blueprint公開確認
    TestNotNull("DifficultySettings struct should exist", DifficultySettingsStruct);
    TestTrue("DifficultySettings should have BlueprintType", DifficultySettingsStruct->HasMetaData(TEXT("BlueprintType")));
    
    TestNotNull("WindData struct should exist", WindDataStruct);
    TestTrue("WindData should have BlueprintType", WindDataStruct->HasMetaData(TEXT("BlueprintType")));
    
    TestNotNull("LandingResult struct should exist", LandingResultStruct);
    TestTrue("LandingResult should have BlueprintType", LandingResultStruct->HasMetaData(TEXT("BlueprintType")));
    
    TestNotNull("GameSessionData struct should exist", GameSessionDataStruct);
    TestTrue("GameSessionData should have BlueprintType", GameSessionDataStruct->HasMetaData(TEXT("BlueprintType")));
    
    return true;
}
```

### BirdDiveDataAssetsTest.cpp

```cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "BirdDive/BirdDiveDataAssets.h"
#include "BirdDive/BirdDiveTypes.h"

// UDifficultyDataAsset テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUDifficultyDataAssetCreationTest, "BirdDive.DataAssets.UDifficultyDataAsset.Creation",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FUDifficultyDataAssetCreationTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    UDifficultyDataAsset* DifficultyAsset = NewObject<UDifficultyDataAsset>();
    
    // Assert
    TestNotNull("DifficultyAsset should be created", DifficultyAsset);
    TestTrue("DifficultyAsset should be valid", IsValid(DifficultyAsset));
    TestEqual("Class should be UDifficultyDataAsset", DifficultyAsset->GetClass(), UDifficultyDataAsset::StaticClass());
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUDifficultyDataAssetSettingsTest, "BirdDive.DataAssets.UDifficultyDataAsset.Settings",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FUDifficultyDataAssetSettingsTest::RunTest(const FString& Parameters)
{
    // Arrange
    UDifficultyDataAsset* DifficultyAsset = NewObject<UDifficultyDataAsset>();
    
    // Act
    DifficultyAsset->DifficultyLevel = EDifficultyLevel::Hard;
    DifficultyAsset->Settings.WindStrengthMultiplier = 1.5f;
    DifficultyAsset->DisplayName = FText::FromString(TEXT("Hard"));
    
    // Assert
    TestEqual("DifficultyLevel assignment", DifficultyAsset->DifficultyLevel, EDifficultyLevel::Hard);
    TestEqual("WindStrengthMultiplier assignment", DifficultyAsset->Settings.WindStrengthMultiplier, 1.5f);
    TestEqual("DisplayName assignment", DifficultyAsset->DisplayName.ToString(), TEXT("Hard"));
    
    return true;
}

// UGameConfigDataAsset テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUGameConfigDataAssetCreationTest, "BirdDive.DataAssets.UGameConfigDataAsset.Creation",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FUGameConfigDataAssetCreationTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    UGameConfigDataAsset* ConfigAsset = NewObject<UGameConfigDataAsset>();
    
    // Assert
    TestNotNull("ConfigAsset should be created", ConfigAsset);
    TestTrue("ConfigAsset should be valid", IsValid(ConfigAsset));
    TestEqual("Class should be UGameConfigDataAsset", ConfigAsset->GetClass(), UGameConfigDataAsset::StaticClass());
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUGameConfigDataAssetMapTest, "BirdDive.DataAssets.UGameConfigDataAsset.Map",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FUGameConfigDataAssetMapTest::RunTest(const FString& Parameters)
{
    // Arrange
    UGameConfigDataAsset* ConfigAsset = NewObject<UGameConfigDataAsset>();
    FDifficultySettings EasySettings;
    EasySettings.WindStrengthMultiplier = 0.5f;
    
    // Act
    ConfigAsset->DefaultDifficulties.Add(EDifficultyLevel::Easy, EasySettings);
    
    // Assert
    TestTrue("Should contain Easy difficulty", ConfigAsset->DefaultDifficulties.Contains(EDifficultyLevel::Easy));
    FDifficultySettings* RetrievedSettings = ConfigAsset->DefaultDifficulties.Find(EDifficultyLevel::Easy);
    TestNotNull("Retrieved settings should not be null", RetrievedSettings);
    TestEqual("WindStrengthMultiplier should match", RetrievedSettings->WindStrengthMultiplier, 0.5f);
    
    return true;
}

// DataAsset Blueprint 統合テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataAssetBlueprintIntegrationTest, "BirdDive.DataAssets.Blueprint.Integration",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDataAssetBlueprintIntegrationTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    UClass* DifficultyAssetClass = UDifficultyDataAsset::StaticClass();
    UClass* ConfigAssetClass = UGameConfigDataAsset::StaticClass();
    
    // Assert - Blueprint公開確認
    TestNotNull("DifficultyAsset class should exist", DifficultyAssetClass);
    TestTrue("DifficultyAsset should have BlueprintType", DifficultyAssetClass->HasMetaData(TEXT("BlueprintType")));
    
    TestNotNull("ConfigAsset class should exist", ConfigAssetClass);
    TestTrue("ConfigAsset should have BlueprintType", ConfigAssetClass->HasMetaData(TEXT("BlueprintType")));
    
    return true;
}
```

### BirdDiveDelegatesTest.cpp

```cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "BirdDive/BirdDiveDelegates.h"
#include "BirdDive/BirdDiveTypes.h"

// デリゲート定義テスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDelegateDefinitionsTest, "BirdDive.Delegates.Definitions",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDelegateDefinitionsTest::RunTest(const FString& Parameters)
{
    // Arrange & Act
    FOnGameStateChanged GameStateDelegate;
    FOnScoreUpdated ScoreDelegate;
    FOnSpeedChanged SpeedDelegate;
    FOnLandingCompleted LandingDelegate;
    FOnWarningTriggered WarningDelegate;
    
    // Assert - デリゲートが正常に定義されていることを確認
    TestFalse("GameState delegate should not be bound initially", GameStateDelegate.IsBound());
    TestFalse("Score delegate should not be bound initially", ScoreDelegate.IsBound());
    TestFalse("Speed delegate should not be bound initially", SpeedDelegate.IsBound());
    TestFalse("Landing delegate should not be bound initially", LandingDelegate.IsBound());
    TestFalse("Warning delegate should not be bound initially", WarningDelegate.IsBound());
    
    return true;
}

// デリゲートバインドテスト
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDelegateBindingTest, "BirdDive.Delegates.Binding",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDelegateBindingTest::RunTest(const FString& Parameters)
{
    // Arrange
    FOnGameStateChanged GameStateDelegate;
    bool bEventFired = false;
    EGameState ReceivedState;
    
    // Act
    GameStateDelegate.AddLambda([&](EGameState NewState) {
        bEventFired = true;
        ReceivedState = NewState;
    });
    
    GameStateDelegate.Broadcast(EGameState::Playing);
    
    // Assert
    TestTrue("Event should have fired", bEventFired);
    TestEqual("Received state should be Playing", ReceivedState, EGameState::Playing);
    TestTrue("Delegate should be bound", GameStateDelegate.IsBound());
    
    return true;
}
```

## テスト実行準備

### Build.cs ファイル更新

BirdDiveTests モジュールの Build.cs を更新：

```cs
using UnrealBuildTool;

public class BirdDiveTests : ModuleRules
{
    public BirdDiveTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "Core", 
            "CoreUObject", 
            "Engine",
            "BirdDive"
        });

        PrivateDependencyModuleNames.AddRange(new string[] 
        {
            "UnrealEd",
            "AutomationController",
            "AutomationTest"
        });
    }
}
```

## 期待される結果

このフェーズでは、以下のテストが全て失敗することが期待されます：

### コンパイルエラー
- `BirdDiveTypes.h` が存在しないため、include エラー
- `EDifficultyLevel`, `EGameState`, `EFlightState`, `ELandingQuality` が未定義
- `FDifficultySettings`, `FWindData`, `FLandingResult`, `FGameSessionData` が未定義

### リンクエラー  
- `UDifficultyDataAsset`, `UGameConfigDataAsset` クラスが未定義
- デリゲート型が未定義

### 実行時エラー
- StaticEnum 呼び出しで null pointer
- StaticStruct 呼び出しで null pointer
- NewObject でクラス作成失敗

## 実行コマンド

```bash
# コンパイル（失敗が期待される）
"C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" BirdDive Development Win64 -Project="D:\UEProject\BirdDiveProject\BirdDive\BirdDive.uproject" -NoHotReloadFromIDE

# テスト実行（コンパイル成功後）
"C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe" "D:\UEProject\BirdDiveProject\BirdDive\BirdDive.uproject" -ExecCmds="Automation RunTests BirdDive" -unattended -nopause -testexit="Automation Test Queue Empty"
```

## 成功条件

- [ ] 全テストファイルが作成される
- [ ] コンパイルが失敗する（実装が存在しないため）
- [ ] エラーメッセージから必要な実装項目が明確になる
- [ ] テスト構造が要件を適切にカバーしている

## 次のステップ

RED フェーズ完了後、GREEN フェーズで最小限の実装を追加してテストを通します。

---

**作成日**: 2025-08-05  
**作成者**: Claude Code (TASK-102 TDD実装)  
**フェーズ**: RED (失敗テスト実装)  
**ステータス**: 実装準備完了