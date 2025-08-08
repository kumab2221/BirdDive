# TASK-301: 分割スクリーンシステム実装 - 要件定義

## 目標
Bird Dive Challengeの2視点表示を実現するための分割スクリーンシステムを実装する。第一人称視点とサイドビュー視点を同時に表示し、プレイヤーの飛行体験を向上させる。

## 機能要件

### 1. USplitScreenManagerコンポーネント
- **コンポーネント設計**: UActorComponent継承の管理コンポーネント
- **アタッチ対象**: GameModeまたはPlayerControllerに統合
- **ライフサイクル管理**: BeginPlay/EndPlayでの適切な初期化/クリーンアップ
- **スレッドセーフティ**: メインスレッドでのレンダリング処理

### 2. 2画面レンダリングシステム  
- **デュアルビューポート**: 画面を2つの領域に分割
- **独立レンダリング**: 各ビューポートで異なるカメラからレンダリング
- **同期レンダリング**: フレーム同期での一貫性確保
- **レンダーターゲット管理**: 効率的なメモリ使用

### 3. ビューポート管理
- **画面分割**: 水平分割（上下）または垂直分割（左右）対応
- **ビューポートサイズ計算**: スクリーン解像度に応じた自動調整
- **アスペクト比維持**: 各ビューポートの適切なアスペクト比
- **境界線描画**: 分割線の視覚的表示（オプション）

### 4. 画面分割比率調整機能
- **動的分割比率**: ランタイムでの比率変更対応
- **比率設定範囲**: 20:80 〜 80:20 の範囲で調整可能
- **アニメーション**: スムーズな比率変更トランジション
- **設定永続化**: 比率設定の保存・復元

## 技術要件

### アーキテクチャ設計
```cpp
// Core Component
UCLASS(BlueprintType, Blueprintable, Category = "Camera")
class BIRDDIVE_API USplitScreenManager : public UActorComponent
{
    GENERATED_BODY()

public:
    // Split screen configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split Screen")
    ESplitScreenType SplitType = ESplitScreenType::Horizontal;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split Screen", meta = (ClampMin = "0.2", ClampMax = "0.8"))
    float SplitRatio = 0.5f;
    
    // Viewport management
    void InitializeSplitScreen();
    void UpdateSplitRatio(float NewRatio);
    void SetCameraForViewport(int32 ViewportIndex, ACameraActor* Camera);
    
protected:
    TArray<FViewportInfo> Viewports;
    TArray<class ACameraActor*> Cameras;
};
```

### データ構造定義
```cpp
// Viewport information structure
USTRUCT(BlueprintType)
struct FViewportInfo
{
    GENERATED_BODY()
    
    FIntRect ViewportRect;
    float AspectRatio;
    class UTextureRenderTarget2D* RenderTarget;
    class ACameraActor* AssignedCamera;
};

// Split screen type enumeration
UENUM(BlueprintType)
enum class ESplitScreenType : uint8
{
    Horizontal  UMETA(DisplayName = "Horizontal (Top/Bottom)"),
    Vertical    UMETA(DisplayName = "Vertical (Left/Right)")
};
```

## パフォーマンス要件

### フレームレート目標
- **60FPS維持**: 2画面描画時でも安定した60FPS
- **GPU負荷**: 使用率70%以下を維持  
- **描画負荷**: シングルスクリーン時の1.6倍以下
- **メモリ使用量**: レンダーターゲット用追加メモリ管理

### 最適化戦略
- **レンダーターゲット共有**: 可能な場合のテクスチャ再利用
- **描画最適化**: 視錐台カリング、LOD最適化
- **メモリプール**: レンダーターゲットのプール管理
- **非同期処理**: 可能な処理の並列化

## レンダリング仕様

### ビューポート分割仕様
- **水平分割**: 画面上部・下部に分割
- **垂直分割**: 画面左側・右側に分割
- **分割境界**: 1〜3ピクセルの境界線（設定可能）
- **マージン**: 各ビューポート間の余白設定

### カメラ統合
- **第一人称カメラ**: 上部/左側ビューポートに配置
- **サイドビューカメラ**: 下部/右側ビューポートに配置
- **カメラ切り替え**: ランタイムでのカメラ割り当て変更
- **同期制御**: 両カメラの時間同期

## 品質要件

### 表示品質
- **解像度維持**: 各ビューポートでの適切な解像度
- **色精度**: HDR対応、色空間管理
- **アンチエイリアシング**: 各ビューポートでのAA適用
- **エフェクト対応**: ポストプロセス効果の各ビューポート適用

### 応答性
- **リアルタイム更新**: 分割比率変更の即座反映
- **入力応答**: UI操作への即座対応
- **設定変更**: 動的設定変更の滑らかな適用
- **エラー復旧**: 異常状態からの自動復旧

### 堅牢性
- **メモリ管理**: メモリリーク防止、適切なクリーンアップ
- **エラーハンドリング**: レンダリング失敗時の処理
- **デバイス対応**: 各種GPU・解像度への対応
- **デバッグ支援**: 開発時のデバッグ情報表示

## インターフェース仕様

### Blueprint統合
```cpp
// Blueprint callable functions
UFUNCTION(BlueprintCallable, Category = "Split Screen")
void SetSplitRatio(float NewRatio);

UFUNCTION(BlueprintCallable, Category = "Split Screen")
void SetSplitType(ESplitScreenType NewType);

UFUNCTION(BlueprintCallable, Category = "Split Screen")  
void AssignCameraToViewport(int32 ViewportIndex, ACameraActor* Camera);

// Blueprint events
UFUNCTION(BlueprintImplementableEvent, Category = "Split Screen")
void OnSplitRatioChanged(float OldRatio, float NewRatio);
```

### C++インターフェース
```cpp
// Public methods
class BIRDDIVE_API USplitScreenManager
{
public:
    void InitializeSplitScreen();
    void Shutdown();
    
    bool SetSplitRatio(float NewRatio, bool bAnimated = true);
    void SetSplitType(ESplitScreenType NewType);
    
    FViewportInfo GetViewportInfo(int32 Index) const;
    int32 GetViewportCount() const { return Viewports.Num(); }
    
    // Camera management
    bool AssignCamera(int32 ViewportIndex, ACameraActor* Camera);
    ACameraActor* GetAssignedCamera(int32 ViewportIndex) const;
};
```

## テスト観点

### 単体テスト
- 分割比率計算の正確性
- ビューポートサイズ算出の精度
- カメラ割り当て機能の動作確認
- メモリ管理の適切性

### 統合テスト
- GameModeとの連携確認
- PlayerControllerとの統合
- カメラシステムとの連携
- レンダリングパイプラインとの統合

### パフォーマンステスト
- 2画面描画時のフレームレート測定
- GPU使用率の監視
- メモリ使用量の計測
- レンダリング負荷の分析

### 視覚テスト
- 表示品質の確認
- 分割境界の表示品質
- アスペクト比の正確性
- カラーマッチングの確認

## 受け入れ基準

### ✅ 必須機能
- [ ] 水平・垂直分割の両方に対応
- [ ] 分割比率の動的変更が可能
- [ ] 2つの独立したカメラからのレンダリング
- [ ] 60FPS@1080pでの安定動作
- [ ] メモリリークなし

### ✅ 品質基準
- [ ] GPU使用率70%以下での動作
- [ ] 画面分割精度±1ピクセル以内
- [ ] 分割比率変更の滑らかなアニメーション
- [ ] エラー処理の完全性

### ✅ ユーザビリティ
- [ ] 直感的な設定インターフェース
- [ ] Blueprint統合の完全性
- [ ] デバッグ情報の適切な表示
- [ ] ドキュメントの完全性

## 実装方針

### フェーズ1: 基盤システム
1. USplitScreenManagerコンポーネント作成
2. 基本的なビューポート分割実装
3. レンダーターゲット管理システム

### フェーズ2: レンダリング統合
1. カメラシステムとの統合
2. レンダリングパイプライン統合
3. パフォーマンス最適化の基礎

### フェーズ3: 機能拡張
1. 動的分割比率変更機能
2. Blueprint統合とユーザビリティ
3. エラーハンドリング強化

### フェーズ4: 最適化・仕上げ
1. パフォーマンス最適化
2. メモリ管理最適化
3. デバッグ機能とドキュメント

## リスク・考慮事項

### 技術的リスク
- レンダリング負荷による性能低下
- メモリ使用量の増加
- GPU制限による制約

### 対応策
- 段階的なパフォーマンステスト
- メモリプール戦略の採用
- LOD・カリング最適化の実装

## 成功指標
- 2画面分割の安定動作
- パフォーマンス要件の完全クリア
- 開発者・プレイヤーの使いやすさ確保
- 将来拡張への対応準備