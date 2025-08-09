#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BirdControlRigTypes.generated.h"

/**
 * ControlRig制御パラメータ構造体
 */
USTRUCT(BlueprintType)
struct BIRDDIVE_API FBirdControlRigParams
{
    GENERATED_BODY()

    // 羽ばたき関連パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flapping")
    float FlappingFrequency = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flapping")
    float WingAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flapping")
    float WingSpread = 1.0f;

    // 姿勢制御パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Posture")
    FRotator BodyRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Posture")
    float TailFanAngle = 0.0f;

    // 風力応答パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    FVector WindInfluence = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wind")
    float StabilizationStrength = 0.0f;

    // コンストラクタ
    FBirdControlRigParams()
    {
        FlappingFrequency = 1.0f;
        WingAngle = 0.0f;
        WingSpread = 1.0f;
        BodyRotation = FRotator::ZeroRotator;
        TailFanAngle = 0.0f;
        WindInfluence = FVector::ZeroVector;
        StabilizationStrength = 0.0f;
    }
};