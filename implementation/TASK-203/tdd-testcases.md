# TASK-203: プレイヤーコントローラー実装 - テストケース定義

## テストケース概要

### テスト戦略
- **単体テスト**: 個別機能の動作確認
- **統合テスト**: キャラクター制御との連携確認
- **ユーザビリティテスト**: 実際の操作感確認

### テスト環境
- **ハードウェア**: キーボード・マウス、Xbox/PS4/PS5コントローラー
- **プラットフォーム**: Windows 11
- **UEバージョン**: 5.6

## 単体テストケース

### TC-203-001: Enhanced Input基盤テスト

#### TC-203-001-01: Input Actions初期化
```cpp
TEST(BirdDivePlayerControllerTest, InputActionsInitialized)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    
    // Act
    Controller->BeginPlay();
    
    // Assert
    EXPECT_TRUE(Controller->GetInputActionMove() != nullptr);
    EXPECT_TRUE(Controller->GetInputActionLook() != nullptr);
    EXPECT_TRUE(Controller->GetInputActionDive() != nullptr);
    EXPECT_TRUE(Controller->GetInputActionBrake() != nullptr);
}
```

#### TC-203-001-02: Input Mapping Context設定
```cpp
TEST(BirdDivePlayerControllerTest, MappingContextsSetup)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    
    // Act
    Controller->BeginPlay();
    
    // Assert
    auto* EnhancedInputComponent = Controller->GetEnhancedInputComponent();
    EXPECT_TRUE(EnhancedInputComponent != nullptr);
    EXPECT_TRUE(Controller->HasMappingContext("IMC_Gameplay"));
}
```

### TC-203-002: キーボード・マウス入力テスト

#### TC-203-002-01: キーボード移動入力
```cpp
TEST(BirdDivePlayerControllerTest, KeyboardMovementInput)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    FVector2D ExpectedInput(0.0f, 1.0f); // W key pressed
    
    // Act
    Controller->SimulateKeyPress(EKeys::W);
    FVector2D ActualInput = Controller->GetMoveInputVector();
    
    // Assert
    EXPECT_NEAR(ActualInput.X, ExpectedInput.X, 0.01f);
    EXPECT_NEAR(ActualInput.Y, ExpectedInput.Y, 0.01f);
}
```

#### TC-203-002-02: マウス視点操作
```cpp
TEST(BirdDivePlayerControllerTest, MouseLookInput)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    FVector2D MouseDelta(100.0f, 50.0f);
    
    // Act
    Controller->SimulateMouseMove(MouseDelta);
    FVector2D ActualLook = Controller->GetLookInputVector();
    
    // Assert
    EXPECT_GT(ActualLook.X, 0.0f);
    EXPECT_GT(ActualLook.Y, 0.0f);
}
```

### TC-203-003: ゲームパッド入力テスト

#### TC-203-003-01: アナログスティック移動
```cpp
TEST(BirdDivePlayerControllerTest, GamepadMovementInput)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    FVector2D StickInput(0.8f, 0.6f);
    
    // Act
    Controller->SimulateGamepadStickInput(EGamepadStick::Left, StickInput);
    FVector2D ActualInput = Controller->GetMoveInputVector();
    
    // Assert
    EXPECT_NEAR(ActualInput.X, StickInput.X, 0.01f);
    EXPECT_NEAR(ActualInput.Y, StickInput.Y, 0.01f);
}
```

#### TC-203-003-02: 右スティック視点操作
```cpp
TEST(BirdDivePlayerControllerTest, GamepadLookInput)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    FVector2D StickInput(0.5f, -0.7f);
    
    // Act
    Controller->SimulateGamepadStickInput(EGamepadStick::Right, StickInput);
    FVector2D ActualLook = Controller->GetLookInputVector();
    
    // Assert
    EXPECT_NEAR(ActualLook.X, StickInput.X, 0.01f);
    EXPECT_NEAR(ActualLook.Y, StickInput.Y, 0.01f);
}
```

### TC-203-004: 入力感度調整テスト

#### TC-203-004-01: マウス感度設定
```cpp
TEST(BirdDivePlayerControllerTest, MouseSensitivityAdjustment)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    float SensitivityX = 2.0f;
    float SensitivityY = 1.5f;
    FVector2D MouseDelta(100.0f, 100.0f);
    
    // Act
    Controller->SetMouseSensitivity(SensitivityX, SensitivityY);
    Controller->SimulateMouseMove(MouseDelta);
    FVector2D ActualLook = Controller->GetLookInputVector();
    
    // Assert
    EXPECT_NEAR(ActualLook.X, MouseDelta.X * SensitivityX, 1.0f);
    EXPECT_NEAR(ActualLook.Y, MouseDelta.Y * SensitivityY, 1.0f);
}
```

#### TC-203-004-02: ゲームパッド感度設定
```cpp
TEST(BirdDivePlayerControllerTest, GamepadSensitivityAdjustment)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    float Sensitivity = 2.5f;
    FVector2D StickInput(0.5f, 0.5f);
    
    // Act
    Controller->SetGamepadLookSensitivity(Sensitivity);
    Controller->SimulateGamepadStickInput(EGamepadStick::Right, StickInput);
    FVector2D ActualLook = Controller->GetLookInputVector();
    
    // Assert
    EXPECT_NEAR(ActualLook.X, StickInput.X * Sensitivity, 0.1f);
    EXPECT_NEAR(ActualLook.Y, StickInput.Y * Sensitivity, 0.1f);
}
```

### TC-203-005: デッドゾーン処理テスト

#### TC-203-005-01: 内側デッドゾーン
```cpp
TEST(BirdDivePlayerControllerTest, InnerDeadzone)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    float DeadzoneRadius = 0.2f;
    FVector2D SmallInput(0.1f, 0.1f); // Within deadzone
    
    // Act
    Controller->SetStickDeadzone(DeadzoneRadius);
    Controller->SimulateGamepadStickInput(EGamepadStick::Left, SmallInput);
    FVector2D ActualInput = Controller->GetMoveInputVector();
    
    // Assert
    EXPECT_NEAR(ActualInput.Size(), 0.0f, 0.01f);
}
```

#### TC-203-005-02: 外側デッドゾーン
```cpp
TEST(BirdDivePlayerControllerTest, OuterDeadzone)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    float OuterDeadzone = 0.9f;
    FVector2D LargeInput(0.95f, 0.95f); // Beyond outer deadzone
    
    // Act
    Controller->SetStickOuterDeadzone(OuterDeadzone);
    Controller->SimulateGamepadStickInput(EGamepadStick::Left, LargeInput);
    FVector2D ActualInput = Controller->GetMoveInputVector();
    
    // Assert
    EXPECT_NEAR(ActualInput.Size(), 1.0f, 0.01f);
}
```

### TC-203-006: デバイス管理テスト

#### TC-203-006-01: デバイス自動検出
```cpp
TEST(BirdDivePlayerControllerTest, DeviceAutoDetection)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    
    // Act
    Controller->BeginPlay();
    
    // Assert
    EXPECT_TRUE(Controller->IsKeyboardMouseAvailable());
    EXPECT_EQ(Controller->GetConnectedGamepadCount() >= 0, true);
}
```

#### TC-203-006-02: ホットスワップ対応
```cpp
TEST(BirdDivePlayerControllerTest, DeviceHotswap)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    
    // Act
    Controller->SimulateDeviceConnect(EInputDevice::Gamepad);
    bool GamepadDetected = Controller->IsGamepadConnected();
    Controller->SimulateDeviceDisconnect(EInputDevice::Gamepad);
    bool GamepadStillDetected = Controller->IsGamepadConnected();
    
    // Assert
    EXPECT_TRUE(GamepadDetected);
    EXPECT_FALSE(GamepadStillDetected);
}
```

## 統合テストケース

### TC-203-101: キャラクター制御統合

#### TC-203-101-01: 入力→移動連携
```cpp
TEST(BirdDivePlayerControllerIntegrationTest, InputToMovementIntegration)
{
    // Arrange
    auto [Controller, Character] = CreateTestControllerWithCharacter();
    FVector2D MoveInput(1.0f, 0.0f);
    
    // Act
    Controller->SimulateKeyPress(EKeys::D); // Right movement
    Controller->ProcessInput(GetDeltaTime());
    Character->Tick(GetDeltaTime());
    
    // Assert
    FVector CharacterVelocity = Character->GetVelocity();
    EXPECT_GT(CharacterVelocity.Y, 0.0f); // Moving right
}
```

#### TC-203-101-02: 視点操作→カメラ連携
```cpp
TEST(BirdDivePlayerControllerIntegrationTest, LookInputToCameraIntegration)
{
    // Arrange
    auto [Controller, Character] = CreateTestControllerWithCharacter();
    FVector2D LookInput(1.0f, 0.0f);
    FRotator InitialRotation = Character->GetControlRotation();
    
    // Act
    Controller->SimulateMouseMove(FVector2D(100.0f, 0.0f));
    Controller->ProcessInput(GetDeltaTime());
    
    // Assert
    FRotator NewRotation = Character->GetControlRotation();
    EXPECT_NE(NewRotation.Yaw, InitialRotation.Yaw);
}
```

### TC-203-102: 設定システム統合

#### TC-203-102-01: 設定永続化
```cpp
TEST(BirdDivePlayerControllerIntegrationTest, SettingsPersistence)
{
    // Arrange
    ABirdDivePlayerController* Controller1 = CreateTestController();
    float TestSensitivity = 2.5f;
    
    // Act
    Controller1->SetMouseSensitivity(TestSensitivity, TestSensitivity);
    Controller1->SaveSettings();
    
    // Create new controller instance
    ABirdDivePlayerController* Controller2 = CreateTestController();
    Controller2->LoadSettings();
    
    // Assert
    EXPECT_NEAR(Controller2->GetMouseSensitivityX(), TestSensitivity, 0.01f);
    EXPECT_NEAR(Controller2->GetMouseSensitivityY(), TestSensitivity, 0.01f);
}
```

## パフォーマンステストケース

### TC-203-201: 入力遅延測定

#### TC-203-201-01: 入力レスポンスタイム
```cpp
TEST(BirdDivePlayerControllerPerformanceTest, InputLatency)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    
    // Act
    auto StartTime = FDateTime::Now();
    Controller->SimulateKeyPress(EKeys::W);
    Controller->ProcessInput(GetDeltaTime());
    FVector2D Result = Controller->GetMoveInputVector();
    auto EndTime = FDateTime::Now();
    
    // Assert
    float ProcessingTime = (EndTime - StartTime).GetTotalMilliseconds();
    EXPECT_LT(ProcessingTime, 16.0f); // Less than 1 frame at 60fps
}
```

## ユーザビリティテストケース

### TC-203-301: 操作性評価

#### 手動テスト項目
1. **初回プレイ感**: 説明なしでの操作理解度
2. **設定変更**: 感度調整の直感性
3. **デバイス切り替え**: 自然な切り替え感
4. **長時間プレイ**: 疲労度・操作精度維持

#### 評価基準
- **優**: 即座に理解・操作可能
- **良**: 少しの試行錯誤で習得
- **可**: 説明があれば理解可能  
- **不可**: 説明でも理解困難

## エラーハンドリングテストケース

### TC-203-401: 異常系処理

#### TC-203-401-01: デバイス切断時処理
```cpp
TEST(BirdDivePlayerControllerErrorTest, DeviceDisconnectionHandling)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    Controller->SimulateDeviceConnect(EInputDevice::Gamepad);
    
    // Act
    Controller->SimulateDeviceDisconnect(EInputDevice::Gamepad);
    Controller->SimulateGamepadStickInput(EGamepadStick::Left, FVector2D(1.0f, 0.0f));
    
    // Assert - Should not crash and gracefully fallback
    EXPECT_NO_THROW(Controller->ProcessInput(GetDeltaTime()));
    EXPECT_FALSE(Controller->IsGamepadConnected());
}
```

#### TC-203-401-02: 不正設定値処理
```cpp
TEST(BirdDivePlayerControllerErrorTest, InvalidSettingsHandling)
{
    // Arrange
    ABirdDivePlayerController* Controller = CreateTestController();
    
    // Act & Assert
    EXPECT_NO_THROW(Controller->SetMouseSensitivity(-1.0f, 10000.0f));
    EXPECT_NO_THROW(Controller->SetStickDeadzone(-0.5f));
    EXPECT_NO_THROW(Controller->SetStickDeadzone(1.5f));
    
    // Values should be clamped to valid range
    EXPECT_GT(Controller->GetMouseSensitivityX(), 0.0f);
    EXPECT_LT(Controller->GetMouseSensitivityX(), 10.0f);
}
```

## テスト実行手順

### 自動テスト実行
```bash
# 全単体テスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.PlayerController"

# 特定テストケース実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.PlayerController.InputActions"

# パフォーマンステスト実行
UnrealEditor.exe -ExecCmds="Automation RunTests BirdDive.PlayerController.Performance"
```

### 手動テスト実行
1. エディタ起動
2. テストマップ読み込み
3. PIE (Play In Editor) 開始
4. 各デバイスでの操作確認
5. 設定画面での調整確認

## 成功基準

### 自動テスト
- [ ] 全単体テストケースが通過 (100%)
- [ ] 統合テストケースが通過 (100%)
- [ ] パフォーマンステストが通過 (100%)
- [ ] エラーハンドリングテストが通過 (100%)

### 手動テスト
- [ ] ユーザビリティ評価で「良」以上 (80%)
- [ ] 操作遅延が体感的に感じられない
- [ ] 設定変更が直感的に行える
- [ ] 複数デバイス使用時も混乱なし

### 品質基準
- [ ] クラッシュ・フリーズなし
- [ ] メモリリークなし
- [ ] CPU使用率への影響1%以下
- [ ] コードカバレッジ90%以上