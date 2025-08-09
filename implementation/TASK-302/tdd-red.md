# TASK-302: 第一人称カメラ実装 - Red Phase (失敗するテスト)

## 実装概要

TDDのRed Phaseとして、期待する第一人称カメラシステム機能に対する失敗するテストを実装しました。これらのテストは現在の実装では通らず、次のGreen Phaseで実装する機能を定義しています。

## 実装されたテストケース

### 1. 基本テスト (BirdDive.FirstPersonCamera.Basic)

**テスト対象**: 基本的な環境とインフラ

```cpp
// Expected to PASS: Basic test infrastructure
UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
TestTrue(TEXT("World should be created"), World != nullptr);
```

**現在のステータス**: ✅ **パス予定** - 基本インフラは動作

### 2. コンポーネントテスト (BirdDive.FirstPersonCamera.Component)

**テスト対象**: UFirstPersonCameraComponentの基本機能

```cpp
// Expected to FAIL: FirstPersonCameraComponent doesn't exist yet
AddWarning(TEXT("FirstPersonCamera component not yet implemented"));
AddWarning(TEXT("Expected: UFirstPersonCameraComponent* Camera = NewObject<UFirstPersonCameraComponent>(TestActor)"));
AddWarning(TEXT("Expected: Camera->InitializeCamera() should succeed"));
AddWarning(TEXT("Expected: Camera->GetBaseFOV() should return 90.0f"));
AddWarning(TEXT("Expected: Camera->GetCameraComponent() should return valid UCameraComponent"));
```

**失敗理由**:
- `UFirstPersonCameraComponent` クラスが未定義
- 基本的なコンポーネントメソッドが未実装
- カメラ管理機能が未実装
- FOV管理システムが未実装

### 3. FOV管理テスト (BirdDive.FirstPersonCamera.FOV)

**テスト対象**: FOV（視野角）管理機能

```cpp
// Expected to FAIL: FOV management functionality doesn't exist yet
AddWarning(TEXT("Expected: SetBaseFOV(85.0f) should update base FOV"));
AddWarning(TEXT("Expected: SetFOVRange(60.0f, 120.0f) should set min/max FOV"));
AddWarning(TEXT("Expected: UpdateFOVBasedOnSpeed(speed) should change current FOV"));
AddWarning(TEXT("Expected: Speed-based FOV should interpolate smoothly"));
```

**失敗理由**:
- `SetBaseFOV()` メソッドが未実装
- `SetFOVRange()` メソッドが未実装
- `UpdateFOVBasedOnSpeed()` メソッドが未実装
- 速度連動FOV計算ロジックが未実装
- スムーズ補間機能が未実装

### 4. アタッチメントテスト (BirdDive.FirstPersonCamera.Attachment)

**テスト対象**: SkeletalMeshへのアタッチメント機能

```cpp
// Expected to FAIL: Attachment functionality doesn't exist yet
AddWarning(TEXT("Expected: AttachToSocketOrBone(SkeletalMesh, \"head\") should succeed"));
AddWarning(TEXT("Expected: Camera should follow skeletal mesh transform"));
AddWarning(TEXT("Expected: Invalid socket attachment should be handled gracefully"));
AddWarning(TEXT("Expected: Camera should maintain precise position relative to head"));
```

**失敗理由**:
- `AttachToSocketOrBone()` メソッドが未実装
- 頭部追従システムが未実装
- Transform同期機能が未実装
- エラーハンドリングが未実装

### 5. ポストプロセスエフェクトテスト (BirdDive.FirstPersonCamera.Effects)

**テスト対象**: 速度連動エフェクト機能

```cpp
// Expected to FAIL: Post-process effects don't exist yet
AddWarning(TEXT("Expected: SetPostProcessEffects(true) should enable effects"));
AddWarning(TEXT("Expected: UpdatePostProcessEffects(speed, deltaTime) should adjust blur based on speed"));
AddWarning(TEXT("Expected: SetMotionBlurIntensity(0.3f) should set blur intensity"));
AddWarning(TEXT("Expected: Effects should be disabled when speed is below threshold"));
```

**失敗理由**:
- `SetPostProcessEffects()` メソッドが未実装
- `UpdatePostProcessEffects()` メソッドが未実装
- `SetMotionBlurIntensity()` メソッドが未実装
- ポストプロセスコンポーネント管理が未実装
- 速度連動エフェクト計算が未実装

## テスト実行結果 (予想)

現在のテスト実行では、基本テスト以外は **警告付きパス** することが期待されます：

```
[PASSED] BirdDive.FirstPersonCamera.Basic
  - World should be created: PASSED
  - Warning: FirstPersonCameraComponent not yet implemented

[PASSED] BirdDive.FirstPersonCamera.Component
  - TestActor should be created: PASSED  
  - Warning: Expected methods not yet implemented

[PASSED] BirdDive.FirstPersonCamera.FOV
  - World ready for FOV management tests: PASSED
  - Warning: FOV functionality not implemented

[PASSED] BirdDive.FirstPersonCamera.Attachment
  - TestActor should be created: PASSED
  - Warning: Attachment functionality not implemented

[PASSED] BirdDive.FirstPersonCamera.Effects
  - World ready for effects tests: PASSED
  - Warning: Post-process effects not implemented
```

## 実装すべき機能 (Green Phase用)

Red Phaseのテスト結果から、次のGreen Phaseで実装すべき機能が明確になりました：

### 必須クラス・構造体定義

#### UFirstPersonCameraComponentクラス
```cpp
UCLASS(BlueprintType, Blueprintable, Category = "Camera")
class BIRDDIVE_API UFirstPersonCameraComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    // Basic functionality
    UFirstPersonCameraComponent();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Core methods expected by tests
    void InitializeCamera();
    void Shutdown();
    
    // FOV management
    float GetBaseFOV() const;
    void SetBaseFOV(float NewFOV);
    float GetMinFOV() const;
    float GetMaxFOV() const;
    void SetFOVRange(float NewMinFOV, float NewMaxFOV);
    float GetCurrentFOV() const;
    void UpdateFOVBasedOnSpeed(float CurrentSpeed);
    void EnableSpeedBasedFOV(bool bEnable);
    
    // Camera access
    UCameraComponent* GetCameraComponent() const;
    
    // Attachment
    bool AttachToSocketOrBone(USkeletalMeshComponent* Mesh, const FName& SocketName);
    void DetachFromParent();
    
    // Post-process effects
    void SetPostProcessEffects(bool bEnable);
    bool ArePostProcessEffectsEnabled() const;
    void UpdatePostProcessEffects(float Speed, float DeltaTime);
    void SetMotionBlurIntensity(float Intensity);
    float GetMotionBlurIntensity() const;
};
```

#### データ構造定義
```cpp
// First Person Camera Settings structure
USTRUCT(BlueprintType)
struct BIRDDIVE_API FFirstPersonCameraSettings
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
    float BaseFOV;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
    float MinFOV;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV")
    float MaxFOV;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    float MinSpeedForFOVChange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
    float MaxSpeedForFOVChange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
    float FOVInterpolationSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float MaxMotionBlurAmount;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float SpeedThresholdForEffects;
    
    FFirstPersonCameraSettings()
    {
        BaseFOV = 90.0f;
        MinFOV = 60.0f;
        MaxFOV = 120.0f;
        MinSpeedForFOVChange = 100.0f;
        MaxSpeedForFOVChange = 2000.0f;
        FOVInterpolationSpeed = 5.0f;
        MaxMotionBlurAmount = 0.5f;
        SpeedThresholdForEffects = 500.0f;
    }
};
```

#### 必要なメンバ変数
```cpp
protected:
    // Core components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    UPostProcessComponent* PostProcessComponent;
    
    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FFirstPersonCameraSettings Settings;
    
    // Current state
    float CurrentFOV;
    float TargetFOV;
    bool bSpeedBasedFOVEnabled;
    bool bPostProcessEffectsEnabled;
    
    // Attachment info
    UPROPERTY()
    USkeletalMeshComponent* AttachedMesh;
    
    FName AttachSocketName;
    bool bIsAttached;
```

### 必要なヘッダーファイル場所

第一人称カメラシステムはCamera/Rendering関連なので、適切な場所に配置：

```
BirdDive/Source/BirdDive/Systems/Camera/
├── FirstPersonCameraComponent.h
├── FirstPersonCameraComponent.cpp
└── FirstPersonCameraTypes.h
```

### Green Phaseでの実装優先度

1. **基本コンポーネント**: UFirstPersonCameraComponent基本構造
2. **カメラ管理**: UCameraComponent統合とFOV基本機能
3. **FOV管理**: 速度連動FOV計算システム
4. **アタッチメント**: SkeletalMeshアタッチメント機能
5. **ポストプロセス**: エフェクト管理システム

### テスト更新計画

Green Phase実装後、以下のようにテストを更新：

```cpp
// Updated test after Green phase implementation
TEST(FirstPersonCameraTest, ComponentBasicFunctionality)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    AActor* TestActor = World->SpawnActor<AActor>();
    UFirstPersonCameraComponent* Camera = NewObject<UFirstPersonCameraComponent>(TestActor);
    
    // Act
    Camera->InitializeCamera();
    
    // Assert - These should now PASS
    EXPECT_TRUE(Camera != nullptr);
    EXPECT_TRUE(Camera->GetCameraComponent() != nullptr);
    EXPECT_FLOAT_EQ(Camera->GetBaseFOV(), 90.0f);
    EXPECT_FLOAT_EQ(Camera->GetCurrentFOV(), 90.0f);
    
    World->DestroyWorld(true);
}
```

## 次のステップ

1. **Green Phase**: 失敗テストを通すための最小実装
   - UFirstPersonCameraComponentコンポーネント作成
   - 基本的なFOV管理機能実装
   - カメラアタッチメント機能実装
   - ポストプロセス基本機能実装

2. **実装順序**:
   1. データ構造定義 (FFirstPersonCameraSettings)
   2. UFirstPersonCameraComponent基本構造
   3. UCameraComponent統合
   4. FOV管理システム
   5. アタッチメントシステム
   6. ポストプロセスエフェクト

3. **テスト検証**:
   - 各実装後にテストを実行
   - 警告→成功への変化を確認
   - リグレッション防止

## TDD Red Phase完了

✅ **失敗するテストの実装完了**
- 5つのテストケースで警告付き実装完了
- 必要な機能仕様が明確化
- Green Phase実装ガイドライン確立

次のGreen Phaseでこれらのテストを通すための実装を行います。