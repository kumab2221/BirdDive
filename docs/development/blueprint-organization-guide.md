# Bird Dive Challenge - Blueprint組織化ガイド

## 概要

このドキュメントは、Bird Dive ChallengeプロジェクトにおけるBlueprint アセットの組織化と分類ルールを定義します。

## Blueprint分類システム

### 階層構造

```
/Content/BirdDive/
├── Core/                           # コアシステムBlueprints
│   ├── GameModes/                  # GM_*, GS_*
│   ├── GameStates/                 # 
│   ├── PlayerControllers/          # PC_*
│   └── HUD/                        # HUD_*, WBP_HUD*
├── Characters/                     # キャラクターBlueprints
│   ├── Birds/                      # BP_Bird*
│   └── Animations/                 # ABP_*, A_*
├── Environment/                    # 環境Blueprints
│   ├── Wind/                       # BP_Wind*, NS_Wind*
│   ├── Targets/                    # BP_Target*
│   ├── Obstacles/                  # BP_Obstacle*
│   └── Levels/                     # LVL_*
├── Effects/                        # エフェクトBlueprints
│   ├── Particles/                  # NS_*, PS_*
│   ├── Wind/                       # エフェクト関連
│   ├── Landing/                    # 着地エフェクト
│   └── Weather/                    # 天候エフェクト
├── Audio/                          # オーディオBlueprints
│   ├── Music/                      # SC_Music*, SW_Music*
│   ├── SFX/                        # SC_SFX*, SW_SFX*
│   └── Voice/                      # SC_Voice*, SW_Voice*
├── UI/                             # UIBlueprints
│   ├── Menus/                      # WBP_Menu*
│   ├── HUD/                        # WBP_HUD*
│   └── Widgets/                    # WBP_Widget*
├── Data/                           # データBlueprints
│   ├── DataTables/                 # DT_*
│   ├── Curves/                     # CRV_*
│   └── Settings/                   # DA_*
├── Materials/                      # マテリアルBlueprints
│   ├── Birds/                      # M_Bird*, MI_Bird*
│   ├── Environment/                # M_Env*, MI_Env*
│   ├── Effects/                    # M_FX*, MI_FX*
│   └── UI/                         # M_UI*, MI_UI*
├── Meshes/                         # メッシュアセット
│   ├── Birds/                      # SM_Bird*, SK_Bird*
│   ├── Environment/                # SM_Env*
│   └── Props/                      # SM_Prop*
└── Blueprints/                     # その他Blueprints
    ├── Gameplay/                   # ゲームプレイロジック
    ├── Components/                 # BP_Component*
    └── Utilities/                  # BFL_*, BML_*
```

## Blueprint命名規則

### Core Blueprints

| タイプ | プレフィックス | 例 | 説明 |
|--------|---------------|-------|------|
| Game Mode | GM_ | GM_BirdDive | メインゲームモード |
| Game State | GS_ | GS_BirdDive | ゲーム状態管理 |
| Player Controller | PC_ | PC_BirdPlayer | プレイヤー制御 |
| HUD | HUD_ | HUD_BirdDive | ゲーム内HUD |
| Player State | PS_ | PS_BirdPlayer | プレイヤー状態 |

### Character Blueprints

| タイプ | プレフィックス | 例 | 説明 |
|--------|---------------|-------|------|
| Bird Character | BP_Bird | BP_Bird_Eagle | 鳥キャラクター |
| Animation Blueprint | ABP_ | ABP_Bird | アニメーション制御 |
| Animation Sequence | A_ | A_Glide_Loop | アニメーション |
| Blend Space | BS_ | BS_Flight | ブレンドスペース |

### Environment Blueprints

| タイプ | プレフィックス | 例 | 説明 |
|--------|---------------|-------|------|
| Wind Zone | BP_Wind | BP_WindZone_Updraft | 風ゾーン |
| Target | BP_Target | BP_Target_Moving | ターゲット |
| Obstacle | BP_Obs | BP_Obs_Cloud | 障害物 |
| Level Actor | BP_Level | BP_LevelBounds | レベル制御 |

### UI Blueprints

| タイプ | プレフィックス | 例 | 説明 |
|--------|---------------|-------|------|
| Widget Blueprint | WBP_ | WBP_MainMenu | UIウィジェット |
| HUD Widget | WBP_HUD | WBP_HUD_Score | HUD要素 |
| Menu Widget | WBP_Menu | WBP_Menu_Settings | メニュー |

## Blueprint設計パターン

### 1. 基底クラス設計

```
BP_BirdCharacterBase (C++ベース)
├── BP_Bird_Eagle
├── BP_Bird_Hawk
└── BP_Bird_Sparrow

BP_TargetBase (C++ベース)
├── BP_Target_Static
├── BP_Target_Moving
└── BP_Target_Bonus
```

### 2. コンポーネント分離

```
BP_BirdCharacter
├── FlightComponent (C++)
├── ScoreComponent (C++)
├── EffectsComponent (BP)
└── AudioComponent (BP)
```

### 3. Interface活用

```
BPI_Targetable
├── BP_Target_* (実装)
├── BP_Obstacle_* (実装)
└── BP_PowerUp_* (実装)

BPI_Scoreable
├── BP_Target_* (実装)
└── BP_CollectibleItem (実装)
```

## Blueprint構成ルール

### 1. イベントグラフ構成

```
Event BeginPlay
├── Initialize Components
├── Setup Bindings
└── Start Initial State

Event Tick
├── Update Flight Physics
├── Check Collisions
└── Update UI

Custom Events
├── OnLanding
├── OnScoreChanged
└── OnWindChanged
```

### 2. 関数構成

```
Pure Functions (計算用)
├── Get Flight Speed
├── Calculate Wind Effect
└── Get Score Multiplier

Callable Functions (アクション用)
├── Start Diving
├── Update Score
└── Play Effect

Event Dispatchers
├── On Landing Successful
├── On Target Hit
└── On Game Over
```

### 3. 変数組織

```
[Category: Flight]
├── Max Speed
├── Dive Force
└── Wind Resistance

[Category: Score]  
├── Current Score
├── Score Multiplier
└── Bonus Points

[Category: State]
├── Is Flying
├── Is Diving
└── Current Target
```

## ベストプラクティス

### 1. Blueprint サイズ管理

- **小さなBlueprint**: 単一責任原則に従う
- **大きなBlueprint**: 関数に分割
- **再利用性**: 共通ロジックはFunction Libraryに

### 2. パフォーマンス考慮

```
✓ 良い例:
- Event Based Logic
- Cached References
- Efficient Collision Detection

✗ 避けるべき:
- Heavy Tick Logic
- Repeated Component Searches
- Unnecessary Casts
```

### 3. デバッグ支援

```
Blueprint Debug Features:
├── Print String for Development
├── Draw Debug Shapes
├── Breakpoints for Logic Check
└── Watch Variables
```

### 4. コメント規則

```
// セクションコメント
// ===================
// Flight Physics Logic
// ===================

// 機能コメント
// Calculate wind effect on bird movement

// TODO コメント  
// TODO: Implement advanced wind turbulence
```

## Blueprint-C++連携

### 1. BlueprintCallable Functions

```cpp
// C++で定義
UFUNCTION(BlueprintCallable, Category = "Flight")
void StartDiving(float Force);

// Blueprintで呼び出し可能
```

### 2. BlueprintImplementableEvent

```cpp
// C++で宣言
UFUNCTION(BlueprintImplementableEvent)
void OnLandingSuccessful(float Score);

// Blueprintで実装
```

### 3. BlueprintNativeEvent

```cpp
// C++でデフォルト実装
UFUNCTION(BlueprintNativeEvent)
void UpdateFlight();
virtual void UpdateFlight_Implementation();

// Blueprintでオーバーライド可能
```

## データ管理

### 1. Data Tables

```
DT_BirdSettings
├── Bird Type
├── Max Speed
├── Dive Force
└── Score Multiplier
```

### 2. Data Assets

```
DA_GameSettings
├── Difficulty Settings
├── Audio Settings
└── Graphics Settings
```

### 3. Curve Assets

```
CRV_ScoreDecay
├── Time (X-Axis)
└── Multiplier (Y-Axis)
```

## バージョン管理

### 1. Blueprint Merge対応

- テキストベースでのマージを可能にするため、単純な構造を維持
- 複雑なロジックは別々のBlueprintに分離

### 2. 実験的Blueprint

```
Experimental/
├── BP_Test_NewFlight
├── BP_Prototype_WindSystem
└── BP_Debug_TargetTesting
```

## 品質チェックリスト

### Blueprint作成時

- [ ] 適切な命名規則に従っている
- [ ] フォルダ構造に配置されている
- [ ] 必要なコメントが記載されている
- [ ] パフォーマンス影響を考慮している
- [ ] テスト可能な構造になっている

### Blueprint修正時

- [ ] 変更理由が明確
- [ ] 既存機能への影響を確認
- [ ] 適切なテストを実施
- [ ] コメントを更新
- [ ] バージョン管理にコミット

## 更新履歴

- 2025-08-04: 初版作成（TASK-003実装時）

---

**注意**: このガイドはプロジェクトの成長に合わせて継続的に更新されます。