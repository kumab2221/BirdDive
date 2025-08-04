// Bird Dive Challenge - C++ 構造体・クラス定義
// Unreal Engine 5.6 対応

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "Engine/DataAsset.h"
#include "cpp-structures.generated.h"

// ============================================================================
// 列挙型定義
// ============================================================================

UENUM(BlueprintType)
enum class EDifficultyLevel : uint8
{
    Easy    UMETA(DisplayName = "Easy"),
    Normal  UMETA(DisplayName = "Normal"),
    Hard    UMETA(DisplayName = "Hard")
};

UENUM(BlueprintType)
enum class EGameState : uint8
{
    Menu        UMETA(DisplayName = "Menu"),
    Playing     UMETA(DisplayName = "Playing"),
    Paused      UMETA(DisplayName = "Paused"),
    GameOver    UMETA(DisplayName = "Game Over"),
    Result      UMETA(DisplayName = "Result")
};

UENUM(BlueprintType)
enum class ELandingQuality : uint8
{
    Perfect     UMETA(DisplayName = "Perfect"),
    Excellent   UMETA(DisplayName = "Excellent"),
    Good        UMETA(DisplayName = "Good"),
    Poor        UMETA(DisplayName = "Poor"),
    Failed      UMETA(DisplayName = "Failed")
};

UENUM(BlueprintType)
enum class ECameraViewType : uint8
{
    FirstPerson UMETA(DisplayName = "First Person"),
    SideView    UMETA(DisplayName = "Side View")
};

// ============================================================================
// 構造体定義
// ============================================================================

USTRUCT(BlueprintType)
struct BIRDDIVE_API FDifficultySettings
{
    GENERATED_BODY()

    // 風力パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    float WindStrengthMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    float WindNoiseFrequency = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    float WindVariationRange = 100.0f;

    // 速度制限
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    float MaxAllowedLandingSpeed = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    float GravityScale = 1.0f;

    // スコア倍率
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    float ScoreMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    float TimeBonus = 10.0f;

    // 操作性
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
    float ControlResponsiveness = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
    float MaxControlForce = 500.0f;

    FDifficultySettings()
    {
        WindStrengthMultiplier = 1.0f;
        WindNoiseFrequency = 0.15f;
        WindVariationRange = 100.0f;
        MaxAllowedLandingSpeed = 800.0f;
        GravityScale = 1.0f;
        ScoreMultiplier = 1.0f;
        TimeBonus = 10.0f;
        ControlResponsiveness = 1.0f;
        MaxControlForce = 500.0f;
    }
};

USTRUCT(BlueprintType)
struct BIRDDIVE_API FLandingResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    bool bIsValidLanding = false;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    ELandingQuality LandingQuality = ELandingQuality::Failed;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    float LandingSpeed = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    float DistanceFromTarget = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    float GameTime = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    FVector LandingPosition = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    int32 BaseScore = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    int32 TimeBonus = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    int32 AccuracyBonus = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Landing")
    int32 TotalScore = 0;
};

USTRUCT(BlueprintType)
struct BIRDDIVE_API FCameraSettings
{
    GENERATED_BODY()

    // 第一人称カメラ設定
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
    float BaseFOV = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
    float MaxSpeedFOV = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
    float FOVBlendSpeed = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "First Person")
    float MaxDistortionStrength = 0.5f;

    // サイドビューカメラ設定
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Side View")
    float SideViewDistance = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Side View")
    float SideViewHeight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Side View")
    float FollowSpeed = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Side View")
    float LookAheadDistance = 200.0f;
};

USTRUCT(BlueprintType)
struct BIRDDIVE_API FWindData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Wind")
    FVector WindDirection = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Wind")
    float WindStrength = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Wind")
    float NoiseValue = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Wind")
    float TimeStamp = 0.0f;
};

USTRUCT(BlueprintType)
struct BIRDDIVE_API FGameStatistics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Statistics")
    int32 GamesPlayed = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Statistics")
    int32 PerfectLandings = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Statistics")
    int32 HighScore = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Statistics")
    float BestTime = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Statistics")
    float TotalPlayTime = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Statistics")
    TMap<EDifficultyLevel, int32> ScoresByDifficulty;
};

// ============================================================================
// デリゲート定義
// ============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLandingEvent, const FLandingResult&, LandingResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameState, NewGameState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDifficultyChanged, EDifficultyLevel, NewDifficulty);

// ============================================================================
// DataAsset定義
// ============================================================================

UCLASS(BlueprintType)
class BIRDDIVE_API UDifficultyDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty")
    TMap<EDifficultyLevel, FDifficultySettings> DifficultySettings;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TMap<EDifficultyLevel, FText> DifficultyDisplayNames;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TMap<EDifficultyLevel, FText> DifficultyDescriptions;

    // デフォルト設定を取得
    UFUNCTION(BlueprintCallable, Category = "Difficulty")
    FDifficultySettings GetDifficultySettings(EDifficultyLevel Difficulty) const;
};

UCLASS(BlueprintType)
class BIRDDIVE_API UGameConfigDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    FCameraSettings CameraSettings;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
    float DefaultGravityZ = -980.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
    float AirDensity = 1.225f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring")
    TArray<float> ScoreThresholds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TMap<ELandingQuality, TSoftObjectPtr<class UNiagaraSystem>> LandingEffects;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TMap<ELandingQuality, TSoftObjectPtr<class USoundBase>> LandingSounds;
};

// ============================================================================
// コンポーネントクラス定義
// ============================================================================

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BIRDDIVE_API UWindForceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWindForceComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    // 風力設定
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    FDifficultySettings CurrentDifficultySettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    float WindSeed = 0.0f;

    // 現在の風力データ
    UPROPERTY(BlueprintReadOnly, Category = "Wind")
    FWindData CurrentWindData;

    // 風力計算
    UFUNCTION(BlueprintCallable, Category = "Wind")
    FVector CalculateWindForce(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Wind")
    void SetDifficultySettings(const FDifficultySettings& NewSettings);

    // イベント
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnScoreUpdated OnWindForceApplied;

private:
    float AccumulatedTime = 0.0f;
    FVector PreviousWindDirection = FVector::ZeroVector;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BIRDDIVE_API USplitScreenManager : public USceneComponent
{
    GENERATED_BODY()

public:
    USplitScreenManager();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split Screen")
    FCameraSettings CameraSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split Screen")
    float SplitRatio = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category = "Cameras")
    class UCameraComponent* FirstPersonCamera;

    UPROPERTY(BlueprintReadOnly, Category = "Cameras")
    class UCameraComponent* SideViewCamera;

    // カメラ制御
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void UpdateCameras(FVector BirdLocation, FVector BirdVelocity);

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraSettings(const FCameraSettings& NewSettings);

    UFUNCTION(BlueprintCallable, Category = "Split Screen")
    void SetSplitRatio(float NewRatio);

private:
    void UpdateFirstPersonCamera(FVector BirdLocation, FVector BirdVelocity);
    void UpdateSideViewCamera(FVector BirdLocation, FVector BirdVelocity);
    void ApplySpeedBasedEffects(FVector BirdVelocity);
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BIRDDIVE_API UBirdEffectManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UBirdEffectManager();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TMap<ELandingQuality, TSoftObjectPtr<class UNiagaraSystem>> LandingEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TMap<ELandingQuality, TSoftObjectPtr<class USoundBase>> LandingSounds;

    // エフェクト再生
    UFUNCTION(BlueprintCallable, Category = "Effects")
    void PlayLandingEffect(ELandingQuality Quality, FVector Location, FRotator Rotation);

    UFUNCTION(BlueprintCallable, Category = "Effects")
    void PlayLandingSound(ELandingQuality Quality, FVector Location);

    // エフェクトプール管理
    UFUNCTION(BlueprintCallable, Category = "Effects")
    void PreloadEffects();

    UFUNCTION(BlueprintCallable, Category = "Effects")
    void CleanupEffects();

private:
    UPROPERTY()
    TArray<class UNiagaraComponent*> EffectPool;

    UPROPERTY()
    class UAudioComponent* AudioComponent;
};

// ============================================================================
// インターフェース定義
// ============================================================================

UINTERFACE(MinimalAPI, Blueprintable)
class UBirdGameplayInterface : public UInterface
{
    GENERATED_BODY()
};

class BIRDDIVE_API IBirdGameplayInterface
{
    GENERATED_BODY()

public:
    // ゲームプレイイベント
    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
    void OnGameStarted(EDifficultyLevel Difficulty);

    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
    void OnGameEnded(const FLandingResult& Result);

    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
    void OnSpeedWarning(float CurrentSpeed, float MaxSpeed);

    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
    void OnScoreUpdated(int32 NewScore);
};

UINTERFACE(MinimalAPI, Blueprintable)
class UCameraControlInterface : public UInterface
{
    GENERATED_BODY()
};

class BIRDDIVE_API ICameraControlInterface
{
    GENERATED_BODY()

public:
    // カメラ制御
    UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
    void UpdateCameraTransform(ECameraViewType ViewType, FTransform NewTransform);

    UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
    void ApplyPostProcessEffect(float SpeedRatio, float DistortionStrength);

    UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
    void SetCameraFOV(float NewFOV);
};

// ============================================================================
// ユーティリティクラス定義
// ============================================================================

UCLASS(BlueprintType)
class BIRDDIVE_API UBirdGameplayStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // スコア計算
    UFUNCTION(BlueprintCallable, Category = "Scoring", CallInEditor = true)
    static int32 CalculateScore(float DistanceFromTarget, float GameTime, EDifficultyLevel Difficulty);

    // 着地品質判定
    UFUNCTION(BlueprintCallable, Category = "Scoring", CallInEditor = true)
    static ELandingQuality DetermineLandingQuality(float LandingSpeed, float DistanceFromTarget, float MaxSpeed);

    // 物理計算ユーティリティ
    UFUNCTION(BlueprintCallable, Category = "Physics", CallInEditor = true)
    static FVector CalculateAirResistance(FVector Velocity, float AirDensity, float DragCoefficient);

    // 風力計算
    UFUNCTION(BlueprintCallable, Category = "Physics", CallInEditor = true)
    static float GeneratePerlinNoise(float X, float Y, float Frequency);

    // デバッグ機能
    UFUNCTION(BlueprintCallable, Category = "Debug", CallInEditor = true)
    static void DrawDebugWindVector(const UObject* WorldContext, FVector Origin, FVector WindForce, float Duration = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Debug", CallInEditor = true)
    static void LogGameplayEvent(const FString& EventName, const FString& Details);
};