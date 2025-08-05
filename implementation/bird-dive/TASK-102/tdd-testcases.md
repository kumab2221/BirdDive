# TASK-102: データ構造・列挙型実装 - テストケース定義

## 概要

Bird Dive Challenge で使用するデータ構造、列挙型、DataAsset、デリゲートの品質を保証するためのテストケースを定義します。TDD アプローチに従い、実装前に期待される動作を明確に定義します。

## テスト戦略

### テストピラミッド構成
```
    Integration Tests (10%)
        ↗            ↖
  Component Tests (20%)
        ↗            ↖
    Unit Tests (70%)
```

### テスト分類
1. **Unit Tests**: 各構造体・列挙型の個別機能
2. **Component Tests**: DataAssetとの統合
3. **Integration Tests**: 基底クラスとの連携

## 列挙型テスト

### EBirdType テストケース

#### TC-BT-001: 列挙型値テスト
```cpp
TEST_CASE("EBirdType Values", "[BirdType][Enum]")
{
    // Arrange & Act & Assert
    REQUIRE(static_cast<int32>(EBirdType::Eagle) == 0);
    REQUIRE(static_cast<int32>(EBirdType::Hawk) == 1);
    REQUIRE(static_cast<int32>(EBirdType::Sparrow) == 2);
    REQUIRE(static_cast<int32>(EBirdType::MAX) == 3);
}
```

#### TC-BT-002: 文字列変換テスト
```cpp
TEST_CASE("EBirdType String Conversion", "[BirdType][Conversion]")
{
    // Arrange
    UEnum* BirdTypeEnum = StaticEnum<EBirdType>();
    
    // Act & Assert
    REQUIRE(BirdTypeEnum->GetNameStringByValue((int64)EBirdType::Eagle) == TEXT("Eagle"));
    REQUIRE(BirdTypeEnum->GetNameStringByValue((int64)EBirdType::Hawk) == TEXT("Hawk"));
    REQUIRE(BirdTypeEnum->GetNameStringByValue((int64)EBirdType::Sparrow) == TEXT("Sparrow"));
}
```

#### TC-BT-003: Blueprint統合テスト
```cpp
TEST_CASE("EBirdType Blueprint Integration", "[BirdType][Blueprint]")
{
    // Arrange
    UEnum* BirdTypeEnum = StaticEnum<EBirdType>();
    
    // Act & Assert
    REQUIRE(BirdTypeEnum != nullptr);
    REQUIRE(BirdTypeEnum->HasMetaData(TEXT("BlueprintType")));
    REQUIRE(BirdTypeEnum->GetMaxEnumValue() == static_cast<int64>(EBirdType::MAX));
}
```

### EDifficultyLevel テストケース

#### TC-DL-001: 難易度順序テスト
```cpp
TEST_CASE("EDifficultyLevel Order", "[Difficulty][Enum]")
{
    // Arrange & Act & Assert
    REQUIRE(EDifficultyLevel::Easy < EDifficultyLevel::Medium);
    REQUIRE(EDifficultyLevel::Medium < EDifficultyLevel::Hard);
    REQUIRE(EDifficultyLevel::Hard < EDifficultyLevel::Expert);
}
```

#### TC-DL-002: 難易度表示名テスト
```cpp
TEST_CASE("EDifficultyLevel Display Names", "[Difficulty][Display]")
{
    // Arrange
    UEnum* DifficultyEnum = StaticEnum<EDifficultyLevel>();
    
    // Act & Assert
    REQUIRE(DifficultyEnum->GetDisplayNameTextByValue((int64)EDifficultyLevel::Easy).ToString() == TEXT("Easy"));
    REQUIRE(DifficultyEnum->GetDisplayNameTextByValue((int64)EDifficultyLevel::Medium).ToString() == TEXT("Medium"));
    REQUIRE(DifficultyEnum->GetDisplayNameTextByValue((int64)EDifficultyLevel::Hard).ToString() == TEXT("Hard"));
    REQUIRE(DifficultyEnum->GetDisplayNameTextByValue((int64)EDifficultyLevel::Expert).ToString() == TEXT("Expert"));
}
```

### EGameState テストケース

#### TC-GS-001: ゲーム状態遷移テスト
```cpp
TEST_CASE("EGameState Transitions", "[GameState][Enum]")
{
    // Arrange
    TArray<EGameState> ValidStates = {
        EGameState::Menu,
        EGameState::Playing,
        EGameState::Paused,
        EGameState::GameOver,
        EGameState::Results
    };
    
    // Act & Assert
    for (EGameState State : ValidStates)
    {
        REQUIRE(static_cast<int32>(State) >= 0);
        REQUIRE(static_cast<int32>(State) < static_cast<int32>(EGameState::MAX));
    }
}
```

### EWindIntensity テストケース

#### TC-WI-001: 風力強度順序テスト
```cpp
TEST_CASE("EWindIntensity Intensity Order", "[Wind][Enum]")
{
    // Arrange & Act & Assert
    REQUIRE(EWindIntensity::Calm < EWindIntensity::Light);
    REQUIRE(EWindIntensity::Light < EWindIntensity::Moderate);
    REQUIRE(EWindIntensity::Moderate < EWindIntensity::Strong);
    REQUIRE(EWindIntensity::Strong < EWindIntensity::Gale);
}
```

## 構造体テスト

### FBirdStats テストケース

#### TC-BS-001: デフォルトコンストラクタテスト
```cpp
TEST_CASE("FBirdStats Default Constructor", "[BirdStats][Struct]")
{
    // Arrange & Act
    FBirdStats Stats;
    
    // Assert
    REQUIRE(Stats.MaxSpeed > 0.0f);
    REQUIRE(Stats.Acceleration > 0.0f);
    REQUIRE(Stats.TurnRate > 0.0f);
    REQUIRE(Stats.WindResistance >= 0.0f);
    REQUIRE(Stats.Mass > 0.0f);
    REQUIRE(Stats.WingSpan > 0.0f);
}
```

#### TC-BS-002: 値設定・取得テスト
```cpp
TEST_CASE("FBirdStats Value Assignment", "[BirdStats][Struct]")
{
    // Arrange
    FBirdStats Stats;
    const float TestMaxSpeed = 1000.0f;
    const float TestAcceleration = 500.0f;
    
    // Act
    Stats.MaxSpeed = TestMaxSpeed;
    Stats.Acceleration = TestAcceleration;
    
    // Assert
    REQUIRE(Stats.MaxSpeed == TestMaxSpeed);
    REQUIRE(Stats.Acceleration == TestAcceleration);
}
```

#### TC-BS-003: 妥当性検証テスト
```cpp
TEST_CASE("FBirdStats Validation", "[BirdStats][Validation]")
{
    // Arrange
    FBirdStats Stats;
    
    // Act & Assert - 負の値は無効
    Stats.MaxSpeed = -100.0f;
    REQUIRE(!Stats.IsValid());
    
    Stats.MaxSpeed = 100.0f;
    Stats.Mass = -1.0f;
    REQUIRE(!Stats.IsValid());
    
    // 正常な値
    Stats.Mass = 1.0f;
    REQUIRE(Stats.IsValid());
}
```

#### TC-BS-004: シリアライゼーションテスト
```cpp
TEST_CASE("FBirdStats Serialization", "[BirdStats][Serialization]")
{
    // Arrange
    FBirdStats OriginalStats;
    OriginalStats.MaxSpeed = 800.0f;
    OriginalStats.Acceleration = 400.0f;
    OriginalStats.TurnRate = 180.0f;
    
    // Act - シリアライゼーション/デシリアライゼーション
    FString SerializedData;
    FBirdStats::StaticStruct()->SerializeTaggedProperties(
        FStructuredArchiveFromArchive(SerializedData).GetSlot(),
        (uint8*)&OriginalStats,
        FBirdStats::StaticStruct(),
        nullptr
    );
    
    FBirdStats DeserializedStats;
    FBirdStats::StaticStruct()->SerializeTaggedProperties(
        FStructuredArchiveFromArchive(SerializedData).GetSlot(),
        (uint8*)&DeserializedStats,
        FBirdStats::StaticStruct(),
        nullptr
    );
    
    // Assert
    REQUIRE(DeserializedStats.MaxSpeed == OriginalStats.MaxSpeed);
    REQUIRE(DeserializedStats.Acceleration == OriginalStats.Acceleration);
    REQUIRE(DeserializedStats.TurnRate == OriginalStats.TurnRate);
}
```

### FWindData テストケース

#### TC-WD-001: 風向きベクトルテスト
```cpp
TEST_CASE("FWindData Direction Vector", "[WindData][Struct]")
{
    // Arrange
    FWindData WindData;
    const FVector TestDirection = FVector(1.0f, 0.0f, 0.0f).GetSafeNormal();
    
    // Act
    WindData.Direction = TestDirection;
    WindData.Strength = 10.0f;
    
    // Assert
    REQUIRE(WindData.Direction.IsUnit());
    REQUIRE(WindData.GetWindForce().Size() == WindData.Strength);
}
```

#### TC-WD-002: 風力強度マッピングテスト
```cpp
TEST_CASE("FWindData Intensity Mapping", "[WindData][Mapping]")
{
    // Arrange
    FWindData WindData;
    
    // Act & Assert
    WindData.Strength = 3.0f;
    REQUIRE(WindData.GetIntensityFromStrength() == EWindIntensity::Calm);
    
    WindData.Strength = 10.0f;
    REQUIRE(WindData.GetIntensityFromStrength() == EWindIntensity::Light);
    
    WindData.Strength = 25.0f;
    REQUIRE(WindData.GetIntensityFromStrength() == EWindIntensity::Moderate);
    
    WindData.Strength = 40.0f;
    REQUIRE(WindData.GetIntensityFromStrength() == EWindIntensity::Strong);
    
    WindData.Strength = 60.0f;
    REQUIRE(WindData.GetIntensityFromStrength() == EWindIntensity::Gale);
}
```

### FScoreData テストケース

#### TC-SD-001: スコア計算テスト
```cpp
TEST_CASE("FScoreData Score Calculation", "[ScoreData][Calculation]")
{
    // Arrange
    FScoreData ScoreData;
    ScoreData.BaseScore = 1000;
    ScoreData.Multiplier = 1.5f;
    ScoreData.BonusPoints = 200;
    
    // Act
    int32 TotalScore = ScoreData.CalculateTotalScore();
    
    // Assert
    int32 ExpectedScore = (ScoreData.BaseScore * ScoreData.Multiplier) + ScoreData.BonusPoints;
    REQUIRE(TotalScore == ExpectedScore);
}
```

#### TC-SD-002: 精度ボーナステスト
```cpp
TEST_CASE("FScoreData Accuracy Bonus", "[ScoreData][Bonus]")
{
    // Arrange
    FScoreData ScoreData;
    ScoreData.BaseScore = 1000;
    ScoreData.Accuracy = 95; // 95%
    
    // Act
    float AccuracyMultiplier = ScoreData.GetAccuracyMultiplier();
    
    // Assert
    REQUIRE(AccuracyMultiplier > 1.0f); // 高精度なのでボーナス
    REQUIRE(AccuracyMultiplier <= 2.0f); // 最大2倍まで
}
```

### FGameConfig テストケース

#### TC-GC-001: デフォルト設定テスト
```cpp
TEST_CASE("FGameConfig Default Values", "[GameConfig][Defaults]")
{
    // Arrange & Act
    FGameConfig Config;
    
    // Assert
    REQUIRE(Config.Difficulty == EDifficultyLevel::Medium);
    REQUIRE(Config.bEnableWind == true);
    REQUIRE(Config.TimeLimit > 0.0f);
    REQUIRE(Config.TargetCount > 0);
    REQUIRE(Config.ScoreDecayRate >= 0.0f);
}
```

#### TC-GC-002: 設定妥当性テスト
```cpp
TEST_CASE("FGameConfig Validation", "[GameConfig][Validation]")
{
    // Arrange
    FGameConfig Config;
    
    // Act & Assert - 無効な設定
    Config.TimeLimit = -1.0f;
    REQUIRE(!Config.IsValid());
    
    Config.TimeLimit = 60.0f;
    Config.TargetCount = -1;
    REQUIRE(!Config.IsValid());
    
    // 正常な設定
    Config.TargetCount = 5;
    REQUIRE(Config.IsValid());
}
```

## DataAsset テストケース

### UBirdDataAsset テストケース

#### TC-BDA-001: アセット作成テスト
```cpp
TEST_CASE("UBirdDataAsset Creation", "[BirdDataAsset][Creation]")
{
    // Arrange & Act
    UBirdDataAsset* BirdAsset = NewObject<UBirdDataAsset>();
    
    // Assert
    REQUIRE(BirdAsset != nullptr);
    REQUIRE(BirdAsset->IsValidLowLevel());
    REQUIRE(BirdAsset->GetClass() == UBirdDataAsset::StaticClass());
}
```

#### TC-BDA-002: 鳥ステータス設定テスト
```cpp
TEST_CASE("UBirdDataAsset Stats Configuration", "[BirdDataAsset][Configuration]")
{
    // Arrange
    UBirdDataAsset* BirdAsset = NewObject<UBirdDataAsset>();
    FBirdStats TestStats;
    TestStats.MaxSpeed = 1200.0f;
    TestStats.TurnRate = 90.0f;
    
    // Act
    BirdAsset->SetBirdStats(EBirdType::Eagle, TestStats);
    
    // Assert
    FBirdStats RetrievedStats = BirdAsset->GetBirdStats(EBirdType::Eagle);
    REQUIRE(RetrievedStats.MaxSpeed == TestStats.MaxSpeed);
    REQUIRE(RetrievedStats.TurnRate == TestStats.TurnRate);
}
```

#### TC-BDA-003: データ永続化テスト
```cpp
TEST_CASE("UBirdDataAsset Data Persistence", "[BirdDataAsset][Persistence]")
{
    // Arrange
    UBirdDataAsset* BirdAsset = NewObject<UBirdDataAsset>();
    const FString TestAssetPath = TEXT("/Game/Test/TestBirdData");
    
    // Act - 保存
    BirdAsset->SaveToAsset(TestAssetPath);
    
    // Assert - 読み込み
    UBirdDataAsset* LoadedAsset = LoadObject<UBirdDataAsset>(nullptr, *TestAssetPath);
    REQUIRE(LoadedAsset != nullptr);
    REQUIRE(LoadedAsset != BirdAsset); // 別のインスタンス
    
    // データ整合性確認
    // (具体的なデータ比較は実装依存)
}
```

### UDifficultyDataAsset テストケース

#### TC-DDA-001: 難易度パラメーター取得テスト
```cpp
TEST_CASE("UDifficultyDataAsset Parameter Retrieval", "[DifficultyDataAsset][Parameters]")
{
    // Arrange
    UDifficultyDataAsset* DifficultyAsset = NewObject<UDifficultyDataAsset>();
    
    // Act
    float EasyTimeLimit = DifficultyAsset->GetTimeLimit(EDifficultyLevel::Easy);
    float HardTimeLimit = DifficultyAsset->GetTimeLimit(EDifficultyLevel::Hard);
    
    // Assert
    REQUIRE(EasyTimeLimit > HardTimeLimit); // 初心者の方が時間に余裕
    REQUIRE(EasyTimeLimit > 0.0f);
    REQUIRE(HardTimeLimit > 0.0f);
}
```

#### TC-DDA-002: スコア倍率テスト
```cpp
TEST_CASE("UDifficultyDataAsset Score Multipliers", "[DifficultyDataAsset][Scoring]")
{
    // Arrange
    UDifficultyDataAsset* DifficultyAsset = NewObject<UDifficultyDataAsset>();
    
    // Act
    float EasyMultiplier = DifficultyAsset->GetScoreMultiplier(EDifficultyLevel::Easy);
    float ExpertMultiplier = DifficultyAsset->GetScoreMultiplier(EDifficultyLevel::Expert);
    
    // Assert
    REQUIRE(EasyMultiplier < ExpertMultiplier); // 高難易度の方が高倍率
    REQUIRE(EasyMultiplier > 0.0f);
    REQUIRE(ExpertMultiplier <= 3.0f); // 最大3倍程度
}
```

## 統合テスト

### TC-INT-001: GameState連携テスト
```cpp
TEST_CASE("Data Structures GameState Integration", "[Integration][GameState]")
{
    // Arrange
    ABirdDiveGameState* GameState = NewObject<ABirdDiveGameState>();
    FScoreData TestScoreData;
    TestScoreData.BaseScore = 1500;
    TestScoreData.Multiplier = 2.0f;
    
    // Act
    GameState->UpdateScoreData(TestScoreData);
    
    // Assert
    FScoreData RetrievedData = GameState->GetCurrentScoreData();
    REQUIRE(RetrievedData.BaseScore == TestScoreData.BaseScore);
    REQUIRE(RetrievedData.Multiplier == TestScoreData.Multiplier);
}
```

### TC-INT-002: Character統合テスト
```cpp
TEST_CASE("BirdStats Character Integration", "[Integration][Character]")
{
    // Arrange
    ABirdCharacter* BirdCharacter = NewObject<ABirdCharacter>();
    FBirdStats TestStats;
    TestStats.MaxSpeed = 800.0f;
    TestStats.TurnRate = 120.0f;
    
    // Act
    BirdCharacter->ApplyBirdStats(TestStats);
    
    // Assert
    float CharacterMaxSpeed = BirdCharacter->GetMaxSpeed();
    float CharacterTurnRate = BirdCharacter->GetTurnRate();
    
    REQUIRE(CharacterMaxSpeed == TestStats.MaxSpeed);
    REQUIRE(CharacterTurnRate == TestStats.TurnRate);
}
```

### TC-INT-003: Blueprint変数公開テスト
```cpp
TEST_CASE("Blueprint Variable Exposure", "[Integration][Blueprint]")
{
    // Arrange & Act
    UStruct* BirdStatsStruct = FBirdStats::StaticStruct();
    UStruct* WindDataStruct = FWindData::StaticStruct();
    UStruct* ScoreDataStruct = FScoreData::StaticStruct();
    
    // Assert - Blueprint公開確認
    REQUIRE(BirdStatsStruct->HasMetaData(TEXT("BlueprintType")));
    REQUIRE(WindDataStruct->HasMetaData(TEXT("BlueprintType")));
    REQUIRE(ScoreDataStruct->HasMetaData(TEXT("BlueprintType")));
    
    // プロパティのBlueprint公開確認
    for (FProperty* Property : TFieldRange<FProperty>(BirdStatsStruct))
    {
        if (Property->HasMetaData(TEXT("BlueprintReadWrite")) || 
            Property->HasMetaData(TEXT("BlueprintReadOnly")))
        {
            REQUIRE(Property->HasAnyPropertyFlags(CPF_BlueprintVisible));
        }
    }
}
```

## パフォーマンステスト

### TC-PERF-001: データアクセス性能テスト
```cpp
TEST_CASE("Data Access Performance", "[Performance][Access]")
{
    // Arrange
    UBirdDataAsset* BirdAsset = NewObject<UBirdDataAsset>();
    const int32 AccessCount = 10000;
    
    // Act
    double StartTime = FPlatformTime::Seconds();
    for (int32 i = 0; i < AccessCount; ++i)
    {
        FBirdStats Stats = BirdAsset->GetBirdStats(EBirdType::Eagle);
        // アクセスのみ、使用はしない
    }
    double EndTime = FPlatformTime::Seconds();
    
    // Assert
    double TotalTime = EndTime - StartTime;
    double AverageTime = TotalTime / AccessCount;
    
    REQUIRE(AverageTime < 0.000001); // 1マイクロ秒以下
    REQUIRE(TotalTime < 0.01); // 全体で10ms以下
}
```

### TC-PERF-002: メモリ使用量テスト
```cpp
TEST_CASE("Memory Usage Test", "[Performance][Memory]")
{
    // Arrange
    const size_t InitialMemory = GetCurrentMemoryUsage();
    TArray<UObject*> CreatedObjects;
    
    // Act - 大量のデータ構造作成
    for (int32 i = 0; i < 1000; ++i)
    {
        UBirdDataAsset* Asset = NewObject<UBirdDataAsset>();
        UDifficultyDataAsset* DiffAsset = NewObject<UDifficultyDataAsset>();
        CreatedObjects.Add(Asset);
        CreatedObjects.Add(DiffAsset);
    }
    
    const size_t PeakMemory = GetCurrentMemoryUsage();
    
    // Cleanup
    for (UObject* Obj : CreatedObjects)
    {
        Obj->MarkAsGarbage();
    }
    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
    
    const size_t FinalMemory = GetCurrentMemoryUsage();
    
    // Assert
    const size_t MemoryIncrease = PeakMemory - InitialMemory;
    const size_t MemoryLeak = FinalMemory - InitialMemory;
    
    REQUIRE(MemoryIncrease < 5 * 1024 * 1024); // 5MB以下
    REQUIRE(MemoryLeak < 1 * 1024 * 1024); // 1MB以下のリーク
}
```

## デリゲートテスト

### TC-DEL-001: スコア変更デリゲートテスト
```cpp
TEST_CASE("Score Change Delegate", "[Delegates][Score]")
{
    // Arrange
    ABirdDiveGameState* GameState = NewObject<ABirdDiveGameState>();
    bool bEventFired = false;
    int32 ReceivedScore = 0;
    int32 ReceivedDelta = 0;
    
    GameState->OnScoreChanged.AddLambda([&](int32 NewScore, int32 ScoreDelta) {
        bEventFired = true;
        ReceivedScore = NewScore;
        ReceivedDelta = ScoreDelta;
    });
    
    // Act
    GameState->AddScore(500);
    
    // Assert
    REQUIRE(bEventFired);
    REQUIRE(ReceivedScore == 500);
    REQUIRE(ReceivedDelta == 500);
}
```

### TC-DEL-002: ゲーム状態変更デリゲートテスト
```cpp
TEST_CASE("Game State Change Delegate", "[Delegates][GameState]")
{
    // Arrange
    ABirdDiveGameState* GameState = NewObject<ABirdDiveGameState>();
    bool bEventFired = false;
    EGameState ReceivedOldState;
    EGameState ReceivedNewState;
    
    GameState->OnGameStateChanged.AddLambda([&](EGameState OldState, EGameState NewState) {
        bEventFired = true;
        ReceivedOldState = OldState;
        ReceivedNewState = NewState;
    });
    
    // Act
    GameState->ChangeGameState(EGameState::Playing);
    
    // Assert
    REQUIRE(bEventFired);
    REQUIRE(ReceivedOldState == EGameState::Menu);
    REQUIRE(ReceivedNewState == EGameState::Playing);
}
```

## 成功基準

- **単体テスト成功率**: 100% (30テストケース)
- **統合テスト成功率**: 100% (3テストケース)  
- **パフォーマンステスト成功率**: 100% (2テストケース)
- **デリゲートテスト成功率**: 100% (2テストケース)
- **総テスト実行時間**: 3分以内
- **コードカバレッジ**: 90%以上

---

**作成日**: 2025-08-05  
**作成者**: Claude Code (TASK-102 TDD実装)  
**レビュー状態**: 未レビュー  
**総テストケース数**: 21個