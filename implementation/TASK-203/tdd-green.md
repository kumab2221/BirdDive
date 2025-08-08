# TASK-203: プレイヤーコントローラー実装 - Green Phase (最小実装)

## 実装概要

TDDのGreen Phaseとして、Red Phaseで定義したテストを通すための最小限の機能実装を行います。

## 実装戦略

### フェーズ1: Enhanced Input基盤
1. Enhanced Input System依存関係の追加
2. InputAction、MappingContextの定義
3. 基本的なセットアップ実装

### フェーズ2: 基本入力処理
1. Move、Look入力の処理
2. 入力ベクターの取得機能
3. テスト通過のための最小実装

### フェーズ3: 設定システム基盤
1. 感度調整機能の基本実装
2. デッドゾーン処理の基本実装
3. デバイス検出機能の基本実装

## 実装詳細

### 1. Enhanced Input System依存関係

#### Build.csファイルの更新
```csharp
// BirdDive.Build.cs に Enhanced Input を追加
PublicDependencyModuleNames.AddRange(new string[] 
{ 
    "Core", "CoreUObject", "Engine", "InputCore",
    "EnhancedInput"  // 追加
});
```

#### ヘッダーファイルの更新
```cpp
// BirdDivePlayerController.h
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
```

### 2. InputAction・MappingContext定義

#### データアセットの作成
- **InputAction_Move**: 2D Vector入力
- **InputAction_Look**: 2D Vector入力  
- **InputAction_Dive**: Boolean入力
- **InputAction_Brake**: Boolean入力
- **IMC_Gameplay**: ゲームプレイ用マッピング

### 3. PlayerControllerクラス拡張

#### 必要なメンバ変数追加
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

    // Current input state
    FVector2D CurrentMoveInput;
    FVector2D CurrentLookInput;
    
    // Settings
    float MouseSensitivityX = 1.0f;
    float MouseSensitivityY = 1.0f;
    float GamepadLookSensitivity = 1.0f;
    float StickDeadzone = 0.2f;
```

#### 必要なメソッド追加
```cpp
public:
    // Enhanced Input accessor methods (テスト用)
    class UInputAction* GetInputActionMove() const { return InputActionMove; }
    class UInputAction* GetInputActionLook() const { return InputActionLook; }
    class UInputAction* GetInputActionDive() const { return InputActionDive; }
    class UInputAction* GetInputActionBrake() const { return InputActionBrake; }

    // Input processing methods (テスト用)
    void ProcessMoveInput(const FVector2D& InputVector) { CurrentMoveInput = InputVector; }
    void ProcessLookInput(const FVector2D& InputVector) { CurrentLookInput = InputVector; }
    FVector2D GetMoveInputVector() const { return CurrentMoveInput; }
    FVector2D GetLookInputVector() const { return CurrentLookInput; }

    // Sensitivity methods (テスト用)
    void SetMouseSensitivity(float SensitivityX, float SensitivityY) 
    { 
        MouseSensitivityX = SensitivityX; 
        MouseSensitivityY = SensitivityY; 
    }
    float GetMouseSensitivityX() const { return MouseSensitivityX; }
    float GetMouseSensitivityY() const { return MouseSensitivityY; }
    void SetGamepadLookSensitivity(float Sensitivity) { GamepadLookSensitivity = Sensitivity; }
    float GetGamepadLookSensitivity() const { return GamepadLookSensitivity; }

    // Deadzone methods (テスト用)
    void SetStickDeadzone(float DeadzoneRadius) { StickDeadzone = DeadzoneRadius; }
    float GetStickDeadzone() const { return StickDeadzone; }

    // Device management methods (テスト用)
    bool IsKeyboardMouseAvailable() const { return true; } // 簡易実装
    bool IsGamepadConnected() const { return false; } // 簡易実装
```

### 4. Input Handler実装

#### Enhanced Input Callbacks
```cpp
protected:
    // Input callbacks
    void OnMoveInput(const FInputActionValue& Value);
    void OnLookInput(const FInputActionValue& Value);
    void OnDiveInput(const FInputActionValue& Value);
    void OnBrakeInput(const FInputActionValue& Value);
```

#### SetupInputComponent Override
```cpp
void ABirdDivePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Enhanced Input Actionsをバインド
        if (InputActionMove)
        {
            EnhancedInputComponent->BindAction(InputActionMove, ETriggerEvent::Triggered, this, &ABirdDivePlayerController::OnMoveInput);
        }
        
        if (InputActionLook)
        {
            EnhancedInputComponent->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &ABirdDivePlayerController::OnLookInput);
        }
        
        if (InputActionDive)
        {
            EnhancedInputComponent->BindAction(InputActionDive, ETriggerEvent::Started, this, &ABirdDivePlayerController::OnDiveInput);
        }
        
        if (InputActionBrake)
        {
            EnhancedInputComponent->BindAction(InputActionBrake, ETriggerEvent::Started, this, &ABirdDivePlayerController::OnBrakeInput);
        }
    }
}
```

#### BeginPlayでのMappingContext追加
```cpp
void ABirdDivePlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // Enhanced Input Subsystem取得
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // Mapping Contextを追加
        if (InputMappingContext)
        {
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }
    
    UE_LOG(LogBirdDive, Log, TEXT("BirdDivePlayerController BeginPlay with Enhanced Input"));
}
```

## 実装手順

### Step 1: Build.cs更新
- Enhanced Input依存関係を追加
- プロジェクトを再コンパイル

### Step 2: ヘッダー更新
- BirdDivePlayerController.h にEnhanced Input関連インクルード追加
- 必要なメンバ変数・メソッド宣言追加

### Step 3: 実装更新
- BirdDivePlayerController.cpp にメソッド実装追加
- Enhanced Input処理の基本実装

### Step 4: Data Assets作成
- InputActionアセット作成（エディタ内）
- InputMappingContextアセット作成（エディタ内）

### Step 5: テスト実行
- 更新されたテストを実行
- パス確認

## 最小実装の特徴

### ✅ 実装する機能
- Enhanced Input基盤のセットアップ
- InputActionの取得メソッド
- 基本的な入力処理メソッド
- 感度・デッドゾーン設定メソッド（setter/getter）
- デバイス検出メソッド（簡易版）

### ❌ まだ実装しない機能（Refactorフェーズ用）
- 複雑な入力フィルタリング
- 高度なデッドゾーン計算
- 実際のデバイス検出ロジック
- 設定の永続化
- エラーハンドリング

## 期待される結果

Green Phase完了時には以下が達成される：

### テスト結果
- `BirdDive.PlayerController.Basic`: **PASS**
- `BirdDive.PlayerController.EnhancedInput`: **PASS** (警告は残る)

### 機能確認
- PlayerControllerが正常に作成される
- Enhanced Input Actionsが取得可能
- 基本的な入力処理メソッドが動作
- 感度・デッドゾーン設定が可能
- デバイス検出メソッドが応答

### コード品質
- コンパイルエラーなし
- 基本的なログ出力
- 最小限のメモリ使用量

## 次のRefactorフェーズ準備

Green Phase完了後、以下の改善点をRefactorフェーズで対応：

1. **パフォーマンス最適化**: 入力処理の効率化
2. **エラーハンドリング**: null check、初期化失敗対応
3. **設定システム**: より柔軟な設定管理
4. **デバイス管理**: 実際のデバイス検出実装
5. **入力フィルタリング**: デッドゾーン、感度カーブ

## TDD Green Phase実装開始

最小限でテストを通すことを目標に、段階的に実装を進めていきます。