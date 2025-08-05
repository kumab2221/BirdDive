# Bird Dive Challenge - アセット命名規則

## 概要

このドキュメントは、Bird Dive Challengeプロジェクトにおけるアセット命名規則を定義します。一貫した命名規則により、プロジェクトの保守性と可読性を向上させます。

## 基本命名規則

### プレフィックス規則

| アセットタイプ | プレフィックス | 例 |
|---|---|---|
| Blueprint Class | BP_ | BP_BirdCharacter |
| Blueprint Function Library | BFL_ | BFL_WindUtilities |
| Blueprint Interface | BPI_ | BPI_Targetable |
| Blueprint Macro Library | BML_ | BML_ScoreCalculations |
| Widget Blueprint | WBP_ | WBP_MainMenu |
| Animation Blueprint | ABP_ | ABP_Bird |
| Behaviour Tree | BT_ | BT_BirdAI |
| Blackboard | BB_ | BB_BirdData |
| Data Asset | DA_ | DA_BirdSettings |
| Data Table | DT_ | DT_ScoreSettings |
| Curve Asset | CRV_ | CRV_WindIntensity |
| Material | M_ | M_BirdFeathers |
| Material Instance | MI_ | MI_BlueBird |
| Material Function | MF_ | MF_WindDistortion |
| Material Parameter Collection | MPC_ | MPC_GlobalSettings |
| Texture | T_ | T_BirdDiffuse |
| Cubemap | TC_ | TC_SkyCube |
| Media Texture | MT_ | MT_IntroVideo |
| Render Target | RT_ | RT_ReflectionCapture |
| Static Mesh | SM_ | SM_Target |
| Skeletal Mesh | SK_ | SK_Bird |
| Sound Cue | SC_ | SC_WindAmbient |
| Sound Wave | SW_ | SW_WingFlap |
| Sound Class | SCL_ | SCL_SFX |
| Sound Mix | SMX_ | SMX_GameAudio |
| Particle System | PS_ | PS_LandingBurst |
| Niagara System | NS_ | NS_WindTrail |
| Niagara Emitter | NE_ | NE_Feathers |
| Control Rig | CR_ | CR_BirdFlight |
| Physics Asset | PA_ | PA_Bird |
| Physics Material | PM_ | PM_BirdBody |
| Level | LVL_ | LVL_MainStage |
| World | W_ | W_BirdDive |

### サフィックス規則

| 用途 | サフィックス | 例 |
|---|---|---|
| Diffuse/Albedo | _D | T_BirdFeathers_D |
| Normal | _N | T_BirdFeathers_N |
| Roughness | _R | T_BirdFeathers_R |
| Metallic | _M | T_BirdFeathers_M |
| Specular | _S | T_BirdFeathers_S |
| Opacity | _O | T_BirdFeathers_O |
| Displacement | _DP | T_BirdFeathers_DP |
| Ambient Occlusion | _AO | T_BirdFeathers_AO |
| Height | _H | T_BirdFeathers_H |
| Mask | _MSK | T_BirdSelection_MSK |
| Packed | _P | T_BirdFeathers_RMA_P |

## フォルダ構造別命名規則

### Core システム

```
/Content/BirdDive/Core/
├── GameModes/        GM_BirdDive, GM_Tutorial
├── GameStates/       GS_BirdDive, GS_Results  
├── PlayerControllers/ PC_BirdDive
└── HUD/              HUD_BirdDive, HUD_Score
```

### キャラクター

```
/Content/BirdDive/Characters/
├── Birds/            BP_BirdBase, BP_Eagle, BP_Hawk
└── Animations/       A_Glide_Loop, A_Dive_Start, A_Landing
```

### 環境

```
/Content/BirdDive/Environment/
├── Wind/             BP_WindZone, NS_WindParticles
├── Targets/          BP_Target_Small, BP_Target_Moving
├── Obstacles/        BP_Cloud, BP_Building
└── Levels/           LVL_Stage01, LVL_Tutorial
```

### エフェクト

```
/Content/BirdDive/Effects/
├── Particles/        NS_LandingBurst, NS_WindTrail
├── Wind/             M_WindDistortion, NS_WindFlow
├── Landing/          NS_TargetHit, SC_Landing
└── Weather/          NS_Rain, NS_CloudFog
```

### オーディオ

```
/Content/BirdDive/Audio/
├── Music/            SW_MainTheme, SW_GameplayMusic
├── SFX/              SW_WingFlap, SW_WindRush
└── Voice/            SW_Announcer_Start, SW_Countdown
```

### UI

```
/Content/BirdDive/UI/
├── Menus/            WBP_MainMenu, WBP_Settings
├── HUD/              WBP_ScoreDisplay, WBP_WindIndicator
└── Widgets/          WBP_Button, WBP_ScoreCounter
```

### データ

```
/Content/BirdDive/Data/
├── DataTables/       DT_BirdSettings, DT_ScoreMultipliers
├── Curves/           CRV_WindIntensity, CRV_ScoreDecay
└── Settings/         DA_GameSettings, DA_DifficultySettings
```

## C++クラス命名規則

### クラス名プレフィックス

| クラスタイプ | プレフィックス | 例 |
|---|---|---|
| Actor | A | ABirdCharacter |
| Pawn | A | ABirdPawn |
| Character | A | ABirdCharacter |
| Controller | A | ABirdPlayerController |
| GameMode | A | ABirdDiveGameMode |
| GameState | A | ABirdDiveGameState |
| PlayerState | A | ABirdPlayerState |
| HUD | A | ABirdDiveHUD |
| Component | U | UFlightComponent |
| Object | U | UWindSystem |
| Widget | U | UScoreWidget |
| Interface | I | ITargetable |
| Enum | E | EBirdType |
| Struct | F | FWindData |

### ファイル名規則

- ヘッダーファイル: `BirdCharacter.h`
- ソースファイル: `BirdCharacter.cpp`
- プライベートヘッダー: `BirdCharacterPrivate.h`

## 変数・関数命名規則

### 変数命名

```cpp
// プライベートメンバー変数
float WindStrength;
int32 CurrentScore;
bool bIsFlying;

// パブリックメンバー変数 (避けるべき)
// 代わりにGetterとSetterを使用

// ローカル変数
float LocalWindStrength;
int32 TempScore;
```

### 関数命名

```cpp
// Getter
float GetWindStrength() const;
int32 GetCurrentScore() const;
bool IsFlying() const;

// Setter  
void SetWindStrength(float NewStrength);
void SetCurrentScore(int32 NewScore);

// Action
void StartDiving();
void UpdateScore();
void HandleWindChange();

// Event
UFUNCTION(BlueprintImplementableEvent)
void OnLandingSuccessful();

UFUNCTION(BlueprintCallable)
void CalculateScore();
```

## Blueprint変数・関数命名規則

### 変数命名

- **Public変数**: `WindStrength`, `CurrentScore`, `IsFlying`
- **Private変数**: `InternalTimer`, `CachedResult`
- **Component参照**: `FlightComponent`, `ScoreComponent`

### 関数命名

- **イベント**: `OnBeginPlay`, `OnLanding`, `OnScoreChanged`
- **純粋関数**: `GetWindDirection`, `CalculateDistance`
- **カスタムイベント**: `StartDiving`, `UpdateUI`, `PlayLandingEffect`

## 特殊規則

### バリエーション命名

同じベースアセットの複数バリエーション：
- `BP_Bird_Eagle`
- `BP_Bird_Hawk`  
- `BP_Bird_Sparrow`

### 難易度別バリエーション

- `LVL_Stage01_Easy`
- `LVL_Stage01_Medium`
- `LVL_Stage01_Hard`

### LOD命名

- `SM_Bird_LOD0`
- `SM_Bird_LOD1` 
- `SM_Bird_LOD2`

### テスト・デバッグアセット

- `BP_DebugBird`
- `M_TestMaterial`
- `LVL_TestLevel`

## 命名規則チェックリスト

- [ ] 適切なプレフィックスが使用されている
- [ ] 名前が機能を明確に表している
- [ ] 略語や省略形を避けている
- [ ] PascalCaseまたはcamelCaseを適切に使用している
- [ ] 数字の使用が適切（バージョンやLODなど）
- [ ] フォルダ構造に適合している
- [ ] チーム内で一貫性がある

## 更新履歴

- 2025-08-04: 初版作成（TASK-003実装時）

---

**注意**: この命名規則は Bird Dive Challenge プロジェクト専用です。プロジェクトの進行に応じて更新される可能性があります。