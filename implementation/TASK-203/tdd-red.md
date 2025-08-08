# TASK-203: プレイヤーコントローラー実装 - Red Phase (失敗するテスト)

## 実装概要

TDDのRed Phase として、期待する機能に対する失敗するテストを実装しました。これらのテストは現在の `ABirdDivePlayerController` の基本実装では通らず、次のGreen Phaseで実装する機能を定義しています。

## 実装されたテストケース

### 1. 基本テスト (BirdDive.PlayerController.Basic)

**テスト対象**: Enhanced Input System の基盤機能

```cpp
// Expected to FAIL: Input Actions don't exist yet
bool bHasInputActionMove = Controller->GetInputActionMove() != nullptr;
bool bHasInputActionLook = Controller->GetInputActionLook() != nullptr;
bool bHasInputActionDive = Controller->GetInputActionDive() != nullptr;
bool bHasInputActionBrake = Controller->GetInputActionBrake() != nullptr;
```

**失敗理由**: 
- `GetInputActionMove()` 等のメソッドが未実装
- Enhanced Input Actions が未定義

### 2. 入力処理テスト (BirdDive.PlayerController.InputProcessing)

**テスト対象**: 移動・視点入力の処理機能

```cpp
// Expected to FAIL: Movement input processing doesn't exist yet
Controller->ProcessMoveInput(TestMoveInput);
FVector2D ActualMoveInput = Controller->GetMoveInputVector();

// Expected to FAIL: Look input processing doesn't exist yet  
Controller->ProcessLookInput(TestLookInput);
FVector2D ActualLookInput = Controller->GetLookInputVector();
```

**失敗理由**:
- `ProcessMoveInput()` メソッドが未実装
- `ProcessLookInput()` メソッドが未実装
- `GetMoveInputVector()` / `GetLookInputVector()` メソッドが未実装

### 3. 感度調整テスト (BirdDive.PlayerController.Sensitivity)

**テスト対象**: マウス・ゲームパッド感度の調整機能

```cpp
// Expected to FAIL: Mouse sensitivity functions don't exist yet
Controller->SetMouseSensitivity(TestSensitivityX, TestSensitivityY);
float ActualSensitivityX = Controller->GetMouseSensitivityX();
float ActualSensitivityY = Controller->GetMouseSensitivityY();

// Expected to FAIL: Gamepad sensitivity functions don't exist yet
Controller->SetGamepadLookSensitivity(TestGamepadSensitivity);
float ActualGamepadSensitivity = Controller->GetGamepadLookSensitivity();
```

**失敗理由**:
- `SetMouseSensitivity()` メソッドが未実装
- `GetMouseSensitivityX()` / `GetMouseSensitivityY()` メソッドが未実装
- `SetGamepadLookSensitivity()` / `GetGamepadLookSensitivity()` メソッドが未実装

### 4. デッドゾーン処理テスト (BirdDive.PlayerController.Deadzone)

**テスト対象**: アナログスティックのデッドゾーン処理機能

```cpp
// Expected to FAIL: Deadzone functions don't exist yet
Controller->SetStickDeadzone(TestDeadzone);
Controller->ProcessMoveInput(SmallInput);
FVector2D ProcessedInput = Controller->GetMoveInputVector();
// Input within deadzone should be filtered to zero
```

**失敗理由**:
- `SetStickDeadzone()` メソッドが未実装
- デッドゾーン処理ロジックが未実装

### 5. デバイス管理テスト (BirdDive.PlayerController.DeviceManagement)

**テスト対象**: 入力デバイスの検出・管理機能

```cpp
// Expected to FAIL: Device detection functions don't exist yet
bool bKeyboardMouseAvailable = Controller->IsKeyboardMouseAvailable();
bool bGamepadConnected = Controller->IsGamepadConnected();
```

**失敗理由**:
- `IsKeyboardMouseAvailable()` メソッドが未実装
- `IsGamepadConnected()` メソッドが未実装

## テスト実行結果 (予想)

現在のテスト実行では、すべてのテストが **失敗** することが期待されます：

```
[FAILED] BirdDive.PlayerController.Basic
  - Input Action Move should exist: FAILED (GetInputActionMove() not found)
  - Input Action Look should exist: FAILED (GetInputActionLook() not found)
  - Input Action Dive should exist: FAILED (GetInputActionDive() not found)  
  - Input Action Brake should exist: FAILED (GetInputActionBrake() not found)

[FAILED] BirdDive.PlayerController.InputProcessing  
  - Move input should be processed correctly: FAILED (ProcessMoveInput() not found)
  - Look input should be processed correctly: FAILED (ProcessLookInput() not found)

[FAILED] BirdDive.PlayerController.Sensitivity
  - Mouse sensitivity X should be set correctly: FAILED (SetMouseSensitivity() not found)
  - Mouse sensitivity Y should be set correctly: FAILED (GetMouseSensitivityY() not found)
  - Gamepad look sensitivity should be set correctly: FAILED (SetGamepadLookSensitivity() not found)

[FAILED] BirdDive.PlayerController.Deadzone
  - Input within deadzone should be filtered to zero: FAILED (SetStickDeadzone() not found)

[FAILED] BirdDive.PlayerController.DeviceManagement
  - Keyboard/Mouse should be detected as available: FAILED (IsKeyboardMouseAvailable() not found)
```

## 実装すべき機能 (Green Phase用)

Red Phaseのテスト結果から、次のGreen Phaseで実装すべき機能が明確になりました：

### 必須メソッド実装

#### Enhanced Input関連
```cpp
// Input Actions accessor methods
class UInputAction* GetInputActionMove() const;
class UInputAction* GetInputActionLook() const;
class UInputAction* GetInputActionDive() const;
class UInputAction* GetInputActionBrake() const;
```

#### 入力処理メソッド
```cpp
// Input processing methods
void ProcessMoveInput(const FVector2D& InputVector);
void ProcessLookInput(const FVector2D& InputVector);
FVector2D GetMoveInputVector() const;
FVector2D GetLookInputVector() const;
```

#### 感度調整メソッド
```cpp
// Sensitivity methods
void SetMouseSensitivity(float SensitivityX, float SensitivityY);
float GetMouseSensitivityX() const;
float GetMouseSensitivityY() const;
void SetGamepadLookSensitivity(float Sensitivity);
float GetGamepadLookSensitivity() const;
```

#### デッドゾーン処理メソッド
```cpp
// Deadzone methods
void SetStickDeadzone(float DeadzoneRadius);
float GetStickDeadzone() const;
```

#### デバイス管理メソッド
```cpp
// Device management methods
bool IsKeyboardMouseAvailable() const;
bool IsGamepadConnected() const;
```

### 必要なメンバ変数

```cpp
protected:
    // Enhanced Input Assets
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* InputActionMove;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* InputActionLook;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* InputActionDive;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* InputActionBrake;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* InputMappingContext;

    // Input state
    FVector2D CurrentMoveInput;
    FVector2D CurrentLookInput;
    
    // Settings
    float MouseSensitivityX = 1.0f;
    float MouseSensitivityY = 1.0f;
    float GamepadLookSensitivity = 1.0f;
    float StickDeadzone = 0.2f;
```

## 次のステップ

1. **Green Phase**: 失敗するテストを通すための最小実装
   - 上記メソッドの基本実装
   - Enhanced Input Systemの基盤設定
   - 基本的な入力処理ロジック

2. **実装優先度**:
   1. Enhanced Input基盤 (InputActions, MappingContext)
   2. 基本入力処理 (Move, Look)
   3. 感度調整機能
   4. デッドゾーン処理
   5. デバイス管理機能

3. **テスト検証**:
   - 各実装後にテストを実行
   - 失敗→成功への変化を確認
   - リグレッション防止

## TDD Red Phase完了

✅ **失敗するテストの実装完了**
- 5つのテストケースで20個のアサーションが失敗予定
- 必要な機能仕様が明確化
- Green Phase実装ガイドライン確立

次のGreen Phaseでこれらのテストを通すための実装を行います。