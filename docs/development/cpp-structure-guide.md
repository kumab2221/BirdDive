# Bird Dive Challenge - C++コード構造ガイド

## 概要

このドキュメントは、Bird Dive ChallengeプロジェクトにおけるC++コードの構造と組織化方針を定義します。

## ディレクトリ構造

```
/Source/BirdDive/
├── Core/                    # コアゲームシステム
│   ├── GameModes/          # ゲームモード関連
│   ├── GameStates/         # ゲームステート関連  
│   ├── PlayerControllers/  # プレイヤーコントローラー
│   └── HUD/                # HUD関連
├── Characters/             # キャラクター関連
│   ├── Birds/              # 鳥キャラクター
│   └── Components/         # キャラクターコンポーネント
├── Environment/            # 環境システム
│   ├── Wind/               # 風システム
│   └── Targets/            # ターゲットシステム
├── Systems/                # ゲームシステム
│   ├── Score/              # スコアシステム
│   └── Physics/            # 物理システム
├── Data/                   # データ構造
│   ├── Structs/            # 構造体定義
│   └── Enums/              # 列挙型定義
├── Interfaces/             # インターフェース
├── Utilities/              # ユーティリティ関数
├── BirdDive.h              # メインヘッダー
├── BirdDive.cpp            # メインソース
├── BirdDive.Build.cs       # ビルド設定
├── BirdDiveDummy.h         # ダミーヘッダー
└── BirdDiveDummy.cpp       # ダミーソース
```

## クラス設計方針

### 基底クラス構造

```
UObject
├── UBirdDiveGameInstanceSubsystem    # グローバルシステム管理
└── UBirdDiveSubsystem               # サブシステム基底

AActor
├── ABirdDiveGameModeBase            # ゲームモード基底
├── ABirdCharacterBase               # 鳥キャラクター基底
├── AWindZone                        # 風ゾーン
└── ATarget                          # ターゲット基底

APawn
└── ABirdPawn                        # 鳥操作キャラクター

APlayerController
└── ABirdDivePlayerController        # プレイヤーコントローラー

AGameStateBase
└── ABirdDiveGameState              # ゲーム状態管理

AHUD
└── ABirdDiveHUD                    # HUD表示

UActorComponent
├── UFlightComponent                 # 飛行コンポーネント
├── UWindInteractionComponent        # 風相互作用
└── UScoreComponent                  # スコア管理
```

## インクルード規則

### ヘッダーインクルード順序

```cpp
// 1. プリコンパイルヘッダー（オプション）
#include "CoreMinimal.h"

// 2. エンジンヘッダー
#include "Engine/Engine.h"
#include "Components/ActorComponent.h"

// 3. サードパーティヘッダー
// （現在なし）

// 4. プロジェクト内ヘッダー
#include "BirdDive/Core/BirdDiveGameModeBase.h"
#include "BirdDive/Data/Structs/FlightData.h"

// 5. 生成ヘッダー（最後に必須）
#include "BirdCharacterBase.generated.h"
```

### フォワード宣言の活用

```cpp
// ヘッダーファイルではフォワード宣言を活用
class UFlightComponent;
class AWindZone;
struct FWindData;

// CPPファイルで実際のインクルード
#include "BirdDive/Characters/Components/FlightComponent.h"
#include "BirdDive/Environment/Wind/WindZone.h"
```

## コーディング規約

### ファイル命名

- **ヘッダー**: `BirdCharacterBase.h`
- **ソース**: `BirdCharacterBase.cpp`
- **プライベートヘッダー**: `BirdCharacterBasePrivate.h`

### クラス命名

```cpp
// Actor系
class BIRDDIVE_API ABirdCharacterBase : public APawn
{
    GENERATED_BODY()
};

// Component系
class BIRDDIVE_API UFlightComponent : public UActorComponent
{
    GENERATED_BODY()
};

// Interface系
class BIRDDIVE_API IBirdTargetable
{
    GENERATED_BODY()
};
```

### メンバー変数

```cpp
UCLASS()
class BIRDDIVE_API ABirdCharacterBase : public APawn
{
    GENERATED_BODY()

protected:
    // UPROPERTY付きメンバー
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
    class UFlightComponent* FlightComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    float MaxFlightSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsFlying;

private:
    // プライベートメンバー（UPROPERTYなし）
    float InternalTimer;
    TArray<AActor*> CachedTargets;
};
```

### 関数定義

```cpp
public:
    // Constructor
    ABirdCharacterBase();

    // UE4/UE5 Override Functions
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // BlueprintCallable Functions
    UFUNCTION(BlueprintCallable, Category = "Flight")
    void StartDiving(float Force);

    UFUNCTION(BlueprintPure, Category = "Flight")
    bool IsFlying() const;

    // BlueprintImplementableEvent
    UFUNCTION(BlueprintImplementableEvent, Category = "Events")
    void OnLandingSuccessful(float Score);

protected:
    // Protected Functions
    virtual void UpdateFlightPhysics(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Flight")
    void SetFlightState(bool bNewFlying);

private:
    // Private Helper Functions
    void CalculateWindEffects();
    void UpdateScoreMultiplier();
```

## データ構造設計

### 構造体例

```cpp
// Data/Structs/FlightData.h
USTRUCT(BlueprintType)
struct BIRDDIVE_API FWindData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector WindDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WindStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Turbulence;

    // デフォルトコンストラクター
    FWindData()
        : WindDirection(FVector::ZeroVector)
        , WindStrength(0.0f)
        , Turbulence(0.0f)
    {}
};
```

### 列挙型例

```cpp
// Data/Enums/BirdTypes.h
UENUM(BlueprintType)
enum class EBirdType : uint8
{
    Eagle       UMETA(DisplayName = "Eagle"),
    Hawk        UMETA(DisplayName = "Hawk"),
    Sparrow     UMETA(DisplayName = "Sparrow"),
    MAX         UMETA(Hidden)
};
```

## モジュール依存関係

### Public Dependencies
- Core
- CoreUObject
- Engine
- InputCore

### Private Dependencies
- Slate
- SlateCore
- UMG
- Niagara
- ControlRig
- Chaos Physics

## ベストプラクティス

### 1. コンストラクターの初期化

```cpp
ABirdCharacterBase::ABirdCharacterBase()
{
    // コンポーネント作成
    FlightComponent = CreateDefaultSubobject<UFlightComponent>(TEXT("FlightComponent"));
    
    // デフォルト値設定
    MaxFlightSpeed = 1000.0f;
    bIsFlying = false;
    
    // Tick設定
    PrimaryActorTick.bCanEverTick = true;
}
```

### 2. Nullチェック

```cpp
void ABirdCharacterBase::UpdateFlight()
{
    if (!FlightComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("FlightComponent is null"));
        return;
    }
    
    FlightComponent->UpdateFlight();
}
```

### 3. ログ出力

```cpp
// BirdDive.h で定義
DECLARE_LOG_CATEGORY_EXTERN(LogBirdDive, Log, All);

// BirdDive.cpp で実装
DEFINE_LOG_CATEGORY(LogBirdDive);

// 使用例
UE_LOG(LogBirdDive, Log, TEXT("Bird started diving with force: %f"), DiveForce);
UE_LOG(LogBirdDive, Warning, TEXT("Invalid target detected"));
UE_LOG(LogBirdDive, Error, TEXT("Critical flight system error"));
```

### 4. デリゲート使用

```cpp
// デリゲート宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);

// クラス内で使用
UPROPERTY(BlueprintAssignable)
FOnScoreChanged OnScoreChanged;

// 呼び出し
OnScoreChanged.Broadcast(CurrentScore);
```

## テスト考慮事項

### 1. テスタブルな設計

```cpp
// テスト用のファクトリー関数
static ABirdCharacterBase* CreateTestBird(UWorld* World);

// パラメーター注入可能な設計
void SetFlightParameters(const FFlightSettings& Settings);
```

### 2. デバッグ支援

```cpp
#if WITH_EDITOR
    // エディター専用のデバッグ機能
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

// デバッグ描画
void DrawDebugFlightPath();
```

## 更新履歴

- 2025-08-04: 初版作成（TASK-003実装時）

---

**注意**: このガイドは開発進行に応じて継続的に更新されます。