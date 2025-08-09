# TASK-303: サイドビューカメラ実装 - Red Phase (失敗するテスト)

## 実装概要

TDDのRed Phaseとして、期待するサイドビューカメラシステム機能に対する失敗するテストを実装しました。これらのテストは現在の実装では通らず、次のGreen Phaseで実装する機能を定義しています。

## 実装されたテストケース

### 1. 基本テスト (BirdDive.SideViewCamera.Basic)

**テスト対象**: 基本的な環境とインフラ

```cpp
// Expected to PASS: Basic test infrastructure
UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
TestTrue(TEXT("World should be created"), World != nullptr);
```

**現在のステータス**: ✅ **パス予定** - 基本インフラは動作

### 2. コンポーネントテスト (BirdDive.SideViewCamera.Component)

**テスト対象**: USideViewCameraComponentの基本機能

```cpp
// Expected to FAIL: SideViewCameraComponent doesn't exist yet
AddWarning(TEXT("SideViewCamera component not yet implemented"));
AddWarning(TEXT("Expected: USideViewCameraComponent* Camera = NewObject<USideViewCameraComponent>(TestActor)"));
AddWarning(TEXT("Expected: Camera->InitializeCamera() should succeed"));
AddWarning(TEXT("Expected: Camera->GetFollowDistance() should return 500.0f"));
AddWarning(TEXT("Expected: Camera->GetCameraComponent() should return valid UCameraComponent"));
```

**失敗理由**:
- `USideViewCameraComponent` クラスが未定義
- 基本的なコンポーネントメソッドが未実装
- カメラ管理機能が未実装
- 追従距離管理システムが未実装

### 3. 追従システムテスト (BirdDive.SideViewCamera.Following)

**テスト対象**: ターゲット追従機能

```cpp
// Expected to FAIL: Following functionality doesn't exist yet
AddWarning(TEXT("Expected: SetTargetActor(actor) should set target"));
AddWarning(TEXT("Expected: UpdateCamera(deltaTime) should follow target"));
AddWarning(TEXT("Expected: Camera should maintain side view perspective"));
AddWarning(TEXT("Expected: Smooth following with appropriate distance"));
```

**失敗理由**:
- `SetTargetActor()` メソッドが未実装
- `UpdateCamera()` メソッドが未実装
- ターゲット追従アルゴリズムが未実装
- サイドビュー視点制御が未実装
- 滑らかな追従機能が未実装

### 4. 先読み機能テスト (BirdDive.SideViewCamera.LookAhead)

**テスト対象**: 速度連動先読み機能

```cpp
// Expected to FAIL: Look-ahead functionality doesn't exist yet
AddWarning(TEXT("Expected: SetLookAheadStrength(strength) should adjust look-ahead"));
AddWarning(TEXT("Expected: CalculateLookAheadPosition() should predict target position"));
AddWarning(TEXT("Expected: Camera should move ahead based on target velocity"));
AddWarning(TEXT("Expected: Look-ahead should respond to direction changes"));
```

**失敗理由**:
- `SetLookAheadStrength()` メソッドが未実装
- `CalculateLookAheadPosition()` メソッドが未実装
- 速度ベース先読み計算が未実装
- 方向変化予測システムが未実装
- 先読み位置補間が未実装

### 5. 境界制限テスト (BirdDive.SideViewCamera.Boundary)

**テスト対象**: 境界制限・画面内保持機能

```cpp
// Expected to FAIL: Boundary limitation doesn't exist yet
AddWarning(TEXT("Expected: SetBoundaryLimits(min, max) should set boundaries"));
AddWarning(TEXT("Expected: ClampToBoundary(position) should limit position"));
AddWarning(TEXT("Expected: Camera should stay within defined boundaries"));
AddWarning(TEXT("Expected: Target should always stay in screen view"));
```

**失敗理由**:
- `SetBoundaryLimits()` メソッドが未実装
- `ClampToBoundary()` メソッドが未実装
- 境界制限システムが未実装
- 画面内保持機能が未実装
- 境界到達時の処理が未実装

## テスト実行結果 (予想)

現在のテスト実行では、基本テスト以外は **警告付きパス** することが期待されます：

```
[PASSED] BirdDive.SideViewCamera.Basic
  - World should be created: PASSED
  - Warning: SideViewCameraComponent not yet implemented

[PASSED] BirdDive.SideViewCamera.Component
  - TestActor should be created: PASSED  
  - Warning: Expected methods not yet implemented

[PASSED] BirdDive.SideViewCamera.Following
  - World ready for following tests: PASSED
  - Warning: Following functionality not implemented

[PASSED] BirdDive.SideViewCamera.LookAhead
  - World ready for look-ahead tests: PASSED
  - Warning: Look-ahead functionality not implemented

[PASSED] BirdDive.SideViewCamera.Boundary
  - World ready for boundary tests: PASSED
  - Warning: Boundary limitation not implemented
```

## 実装すべき機能 (Green Phase用)

Red Phaseのテスト結果から、次のGreen Phaseで実装すべき機能が明確になりました：

### 必須クラス・構造体定義

#### USideViewCameraComponentクラス
```cpp
UCLASS(BlueprintType, Blueprintable, Category = "Camera")
class BIRDDIVE_API USideViewCameraComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    // Basic functionality
    USideViewCameraComponent();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Core methods expected by tests
    void InitializeCamera();
    void Shutdown();
    
    // Following system
    float GetFollowDistance() const;
    void SetFollowDistance(float NewDistance);
    float GetFollowHeight() const;
    void SetFollowHeight(float NewHeight);
    void SetTargetActor(AActor* NewTarget);
    AActor* GetTargetActor() const;
    void UpdateCamera(float DeltaTime);
    
    // Look-ahead system
    float GetLookAheadStrength() const;
    void SetLookAheadStrength(float Strength);
    FVector CalculateLookAheadPosition() const;
    void UpdateLookAhead(float DeltaTime);
    
    // Boundary system
    void SetBoundaryLimits(const FVector& MinBounds, const FVector& MaxBounds);
    bool IsPositionInBounds(const FVector& Position) const;
    FVector ClampToBoundary(const FVector& Position) const;
    
    // Camera access
    UCameraComponent* GetCameraComponent() const;
};
```

#### データ構造定義
```cpp
// Side View Camera Settings structure
USTRUCT(BlueprintType)
struct BIRDDIVE_API FSideViewCameraSettings
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
    float FollowDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
    float FollowHeight;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
    float MinFollowDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Following")
    float MaxFollowDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
    float LookAheadDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
    float LookAheadStrength;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAhead")
    float MaxLookAheadDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
    float FollowSmoothSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing")
    float LookAheadSmoothSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boundary")
    FVector BoundaryMin;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boundary")
    FVector BoundaryMax;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
    float ScreenMargin;
    
    FSideViewCameraSettings()
    {
        FollowDistance = 500.0f;
        FollowHeight = 0.0f;
        MinFollowDistance = 200.0f;
        MaxFollowDistance = 1000.0f;
        LookAheadDistance = 200.0f;
        LookAheadStrength = 1.0f;
        MaxLookAheadDistance = 400.0f;
        FollowSmoothSpeed = 5.0f;
        LookAheadSmoothSpeed = 3.0f;
        BoundaryMin = FVector(-5000, -5000, -1000);
        BoundaryMax = FVector(5000, 5000, 2000);
        ScreenMargin = 0.1f;
    }
};
```

#### 必要なメンバ変数
```cpp
protected:
    // Core components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComponent;
    
    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FSideViewCameraSettings Settings;
    
    // Target and tracking
    UPROPERTY()
    AActor* TargetActor;
    
    FVector PreviousTargetPosition;
    FVector TargetVelocity;
    FVector CurrentCameraPosition;
    FVector TargetCameraPosition;
    
    // State
    bool bIsInitialized;
    bool bHasValidTarget;
    float LastUpdateTime;
```

### 必要なヘッダーファイル場所

サイドビューカメラシステムは Camera 関連なので、適切な場所に配置：

```
BirdDive/Source/BirdDive/Systems/Camera/
├── SideViewCameraComponent.h
├── SideViewCameraComponent.cpp
└── SideViewCameraTypes.h
```

### Green Phaseでの実装優先度

1. **基本コンポーネント**: USideViewCameraComponent基本構造
2. **カメラ管理**: UCameraComponent統合と基本機能
3. **追従システム**: ターゲット追従・距離維持機能
4. **先読みシステム**: 速度連動先読み計算
5. **境界制限**: 境界設定・制限機能

### テスト更新計画

Green Phase実装後、以下のようにテストを更新：

```cpp
// Updated test after Green phase implementation
TEST(SideViewCameraTest, ComponentBasicFunctionality)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    AActor* TestActor = World->SpawnActor<AActor>();
    USideViewCameraComponent* Camera = NewObject<USideViewCameraComponent>(TestActor);
    
    // Act
    Camera->InitializeCamera();
    
    // Assert - These should now PASS
    EXPECT_TRUE(Camera != nullptr);
    EXPECT_TRUE(Camera->GetCameraComponent() != nullptr);
    EXPECT_FLOAT_EQ(Camera->GetFollowDistance(), 500.0f);
    EXPECT_EQ(Camera->GetTargetActor(), nullptr); // No target set initially
    
    World->DestroyWorld(true);
}
```

## 次のステップ

1. **Green Phase**: 失敗テストを通すための最小実装
   - USideViewCameraComponentコンポーネント作成
   - 基本的な追従機能実装
   - カメラ管理機能実装
   - 先読み基本機能実装
   - 境界制限基本機能実装

2. **実装順序**:
   1. データ構造定義 (FSideViewCameraSettings)
   2. USideViewCameraComponent基本構造
   3. UCameraComponent統合
   4. ターゲット追従システム
   5. 先読み機能システム
   6. 境界制限システム

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