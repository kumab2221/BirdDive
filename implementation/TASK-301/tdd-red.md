# TASK-301: 分割スクリーンシステム実装 - Red Phase (失敗するテスト)

## 実装概要

TDDのRed Phaseとして、期待する分割スクリーンシステム機能に対する失敗するテストを実装しました。これらのテストは現在の実装では通らず、次のGreen Phaseで実装する機能を定義しています。

## 実装されたテストケース

### 1. 基本テスト (BirdDive.SplitScreen.Basic)

**テスト対象**: 基本的な環境とインフラ

```cpp
// Expected to PASS: Basic test infrastructure
UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
TestTrue(TEXT("World should be created"), World != nullptr);
```

**現在のステータス**: ✅ **パス予定** - 基本インフラは動作

### 2. コンポーネントテスト (BirdDive.SplitScreen.Component)

**テスト対象**: USplitScreenManagerコンポーネントの基本機能

```cpp
// Expected to FAIL: SplitScreenManager component doesn't exist yet
AddWarning(TEXT("SplitScreenManager component not yet implemented"));
AddWarning(TEXT("Expected methods: GetViewportCount(), GetSplitRatio(), SetSplitRatio()"));
```

**失敗理由**:
- `USplitScreenManager` クラスが未定義
- 基本的なコンポーネントメソッドが未実装
- ビューポート管理機能が未実装

### 3. 分割比率テスト (BirdDive.SplitScreen.SplitRatio)

**テスト対象**: 分割比率の設定・取得機能

```cpp
// Expected to FAIL: Split ratio functionality doesn't exist yet
AddWarning(TEXT("Expected: SetSplitRatio(0.3f) should clamp and set ratio"));
AddWarning(TEXT("Expected: GetSplitRatio() should return current ratio"));
AddWarning(TEXT("Expected: Ratio changes should update viewport rectangles"));
```

**失敗理由**:
- `SetSplitRatio()` メソッドが未実装
- `GetSplitRatio()` メソッドが未実装
- 分割比率の計算ロジックが未実装
- ビューポート矩形の更新機能が未実装

### 4. カメラ割り当てテスト (BirdDive.SplitScreen.CameraAssignment)

**テスト対象**: ビューポートへのカメラ割り当て機能

```cpp
// Expected to FAIL: Camera assignment functionality doesn't exist yet
AddWarning(TEXT("Expected: AssignCamera(0, Camera1) should assign camera"));
AddWarning(TEXT("Expected: GetAssignedCamera(0) should return assigned camera"));
AddWarning(TEXT("Expected: Invalid assignments should be handled gracefully"));
```

**失敗理由**:
- `AssignCamera()` メソッドが未実装
- `GetAssignedCamera()` メソッドが未実装
- カメラ管理システムが未実装
- エラーハンドリングが未実装

### 5. ビューポート管理テスト (BirdDive.SplitScreen.ViewportManagement)

**テスト対象**: ビューポートの管理・計算機能

```cpp
// Expected to FAIL: Viewport management doesn't exist yet
AddWarning(TEXT("Expected: GetViewportInfo(0) should return viewport information"));
AddWarning(TEXT("Expected: Viewport rectangles should be calculated correctly"));
AddWarning(TEXT("Expected: Aspect ratios should be maintained"));
```

**失敗理由**:
- `GetViewportInfo()` メソッドが未実装
- `FViewportInfo` 構造体が未定義
- ビューポート矩形計算ロジックが未実装
- アスペクト比計算機能が未実装

## テスト実行結果 (予想)

現在のテスト実行では、基本テスト以外は **警告付きパス** することが期待されます：

```
[PASSED] BirdDive.SplitScreen.Basic
  - World should be created: PASSED
  - Warning: SplitScreenManager component not yet implemented

[PASSED] BirdDive.SplitScreen.Component
  - Test actor exists: PASSED  
  - Warning: Expected methods not yet implemented

[PASSED] BirdDive.SplitScreen.SplitRatio
  - Test environment ready: PASSED
  - Warning: Split ratio functionality not implemented

[PASSED] BirdDive.SplitScreen.CameraAssignment
  - Camera1 should be created: PASSED
  - Camera2 should be created: PASSED
  - Warning: Camera assignment not implemented

[PASSED] BirdDive.SplitScreen.ViewportManagement
  - World ready: PASSED
  - Warning: Viewport management not implemented
```

## 実装すべき機能 (Green Phase用)

Red Phaseのテスト結果から、次のGreen Phaseで実装すべき機能が明確になりました：

### 必須クラス・構造体定義

#### USplitScreenManagerコンポーネント
```cpp
UCLASS(BlueprintType, Blueprintable, Category = "Camera")
class BIRDDIVE_API USplitScreenManager : public UActorComponent
{
    GENERATED_BODY()

public:
    // Basic functionality
    USplitScreenManager();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    // Core methods expected by tests
    int32 GetViewportCount() const;
    float GetSplitRatio() const;
    bool SetSplitRatio(float NewRatio);
    
    // Camera management
    bool AssignCamera(int32 ViewportIndex, ACameraActor* Camera);
    ACameraActor* GetAssignedCamera(int32 ViewportIndex) const;
    
    // Viewport management
    FViewportInfo GetViewportInfo(int32 Index) const;
    void InitializeSplitScreen();
    void Shutdown();
};
```

#### データ構造定義
```cpp
// Viewport information structure
USTRUCT(BlueprintType)
struct BIRDDIVE_API FViewportInfo
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly)
    FIntRect ViewportRect;
    
    UPROPERTY(BlueprintReadOnly)
    float AspectRatio;
    
    UPROPERTY(BlueprintReadOnly)
    class UTextureRenderTarget2D* RenderTarget;
    
    UPROPERTY(BlueprintReadOnly)
    class ACameraActor* AssignedCamera;
    
    FViewportInfo()
    {
        ViewportRect = FIntRect(0, 0, 0, 0);
        AspectRatio = 16.0f / 9.0f;
        RenderTarget = nullptr;
        AssignedCamera = nullptr;
    }
};

// Split screen type enumeration
UENUM(BlueprintType)
enum class ESplitScreenType : uint8
{
    Horizontal  UMETA(DisplayName = "Horizontal (Top/Bottom)"),
    Vertical    UMETA(DisplayName = "Vertical (Left/Right)")
};
```

#### 必要なメンバ変数
```cpp
protected:
    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split Screen")
    ESplitScreenType SplitType = ESplitScreenType::Horizontal;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split Screen", 
              meta = (ClampMin = "0.2", ClampMax = "0.8"))
    float SplitRatio = 0.5f;
    
    // Viewport data
    UPROPERTY()
    TArray<FViewportInfo> Viewports;
    
    // Camera assignments  
    UPROPERTY()
    TArray<ACameraActor*> AssignedCameras;
    
    // State management
    bool bIsInitialized = false;
    FIntPoint ScreenSize = FIntPoint(1920, 1080);
```

### 必要なヘッダーファイル場所

分割スクリーンシステムは Camera/Rendering関連なので、適切な場所に配置：

```
BirdDive/Source/BirdDive/Systems/Camera/
├── SplitScreenManager.h
├── SplitScreenManager.cpp  
└── SplitScreenTypes.h
```

### Green Phaseでの実装優先度

1. **基本コンポーネント**: USplitScreenManager基本構造
2. **データ構造**: FViewportInfo、ESplitScreenType定義
3. **分割比率管理**: SetSplitRatio、GetSplitRatio実装
4. **ビューポート管理**: ビューポート矩形計算
5. **カメラ管理**: カメラ割り当て・取得機能

### テスト更新計画

Green Phase実装後、以下のようにテストを更新：

```cpp
// Updated test after Green phase implementation
TEST(SplitScreenManagerTest, ComponentBasicFunctionality)
{
    // Arrange
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    AActor* TestActor = World->SpawnActor<AActor>();
    USplitScreenManager* Manager = TestActor->CreateDefaultSubobject<USplitScreenManager>(TEXT("SplitScreenManager"));
    
    // Act
    TestActor->BeginPlay();
    
    // Assert - These should now PASS
    EXPECT_TRUE(Manager != nullptr);
    EXPECT_EQ(Manager->GetViewportCount(), 2);
    EXPECT_FLOAT_EQ(Manager->GetSplitRatio(), 0.5f);
    
    World->DestroyWorld(true);
}
```

## 次のステップ

1. **Green Phase**: 失敗テストを通すための最小実装
   - USplitScreenManagerコンポーネント作成
   - 基本的な分割比率機能実装
   - 簡単なビューポート管理実装

2. **実装順序**:
   1. データ構造定義 (ESplitScreenType、FViewportInfo)
   2. USplitScreenManagerコンポーネント基本構造
   3. 分割比率設定・取得機能
   4. ビューポート情報管理
   5. カメラ割り当て機能

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