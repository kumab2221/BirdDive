#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Systems/Physics/WindForceComponent.h"
#include "../Systems/Physics/BirdCharacterMovementComponent.h"

// ============================
// UWindForceComponent Tests
// ============================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindForceComponentBasicTest, "BirdDive.Physics.WindForceComponent.BasicWindCalculation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FWindForceComponentBasicTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    TestNotNull(TEXT("WindForceComponent should be created"), WindComponent);
    
    FVector TestPosition(100.0f, 200.0f, 300.0f);
    float TestHeight = 500.0f;
    
    // Act
    FVector WindForce = WindComponent->CalculateWindForce(TestPosition, TestHeight);
    
    // Assert
    TestTrue(TEXT("Wind force should be finite"), FMath::IsFinite(WindForce.Size()));
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindForcePerlinNoiseTest, "BirdDive.Physics.WindForceComponent.PerlinNoiseRange",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FWindForcePerlinNoiseTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    TestNotNull(TEXT("WindForceComponent should be created"), WindComponent);
    
    // Act & Assert
    for (int32 i = 0; i < 100; ++i)
    {
        FVector RandomPos(FMath::RandRange(-1000.0f, 1000.0f), 
                         FMath::RandRange(-1000.0f, 1000.0f), 
                         FMath::RandRange(0.0f, 2000.0f));
        float NoiseValue = WindComponent->GetPerlinNoiseValue(RandomPos, 0.0f);
        
        TestTrue(TEXT("Perlin noise should be in valid range"), 
                NoiseValue >= -1.0f && NoiseValue <= 1.0f);
    }
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindForceDifficultyTest, "BirdDive.Physics.WindForceComponent.DifficultyBasedWindStrength",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FWindForceDifficultyTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    TestNotNull(TEXT("WindForceComponent should be created"), WindComponent);
    
    FVector TestPosition(0.0f, 0.0f, 1000.0f);
    
    // Easy difficulty
    WindComponent->WindStrength = 0.5f;
    FVector EasyWind = WindComponent->CalculateWindForce(TestPosition, 1000.0f);
    
    // Hard difficulty  
    WindComponent->WindStrength = 2.0f;
    FVector HardWind = WindComponent->CalculateWindForce(TestPosition, 1000.0f);
    
    // Assert
    TestTrue(TEXT("Hard wind should be stronger than easy wind"), 
            HardWind.Size() >= EasyWind.Size());
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindForcePerformanceTest, "BirdDive.Physics.WindForceComponent.Performance",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FWindForcePerformanceTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    TestNotNull(TEXT("WindForceComponent should be created"), WindComponent);
    
    FVector TestPositions[1000];
    for (int32 i = 0; i < 1000; ++i)
    {
        TestPositions[i] = FVector(FMath::RandRange(-5000.0f, 5000.0f),
                                  FMath::RandRange(-5000.0f, 5000.0f),
                                  FMath::RandRange(0.0f, 2000.0f));
    }
    
    // Act
    double StartTime = FPlatformTime::Seconds();
    for (int32 i = 0; i < 1000; ++i)
    {
        WindComponent->CalculateWindForce(TestPositions[i], TestPositions[i].Z);
    }
    double EndTime = FPlatformTime::Seconds();
    
    // Assert
    double ElapsedTime = EndTime - StartTime;
    TestTrue(TEXT("1000 wind calculations should complete within 10ms"), 
            ElapsedTime < 0.01); // Relaxed for development
    
    return true;
}

// ============================
// UBirdCharacterMovementComponent Tests
// ============================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementGravityTest, "BirdDive.Physics.BirdCharacterMovementComponent.GravityApplication",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementGravityTest::RunTest(const FString& Parameters)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    TestNotNull(TEXT("BirdCharacterMovementComponent should be created"), Movement);
    
    Movement->GravityMultiplier = 1.0f;
    FVector InitialVelocity(0.0f, 0.0f, 0.0f);
    
    // Act
    Movement->Velocity = InitialVelocity;
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f); // 60FPS
    
    // Assert
    TestTrue(TEXT("Velocity should be downward after gravity"), 
            Movement->Velocity.Z < 0.0f);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementWindForceTest, "BirdDive.Physics.BirdCharacterMovementComponent.WindForceApplication",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementWindForceTest::RunTest(const FString& Parameters)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    TestNotNull(TEXT("BirdCharacterMovementComponent should be created"), Movement);
    
    FVector WindForce(100.0f, 50.0f, 0.0f);
    FVector InitialVelocity = Movement->Velocity;
    
    // Act
    Movement->ApplyWindForce(WindForce, 0.016f);
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    TestTrue(TEXT("Velocity should change due to wind force"), 
            !Movement->Velocity.Equals(InitialVelocity, 0.1f));
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementAirResistanceTest, "BirdDive.Physics.BirdCharacterMovementComponent.AirResistance",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementAirResistanceTest::RunTest(const FString& Parameters)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    TestNotNull(TEXT("BirdCharacterMovementComponent should be created"), Movement);
    
    Movement->AirResistance = 0.02f;
    Movement->Velocity = FVector(1000.0f, 0.0f, -1000.0f);
    float InitialSpeed = Movement->Velocity.Size();
    
    // Act
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    float FinalSpeed = Movement->Velocity.Size();
    TestTrue(TEXT("Air resistance should reduce speed"), 
            FinalSpeed <= InitialSpeed);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementMaxSpeedTest, "BirdDive.Physics.BirdCharacterMovementComponent.MaxSpeedLimit",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementMaxSpeedTest::RunTest(const FString& Parameters)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    TestNotNull(TEXT("BirdCharacterMovementComponent should be created"), Movement);
    
    Movement->MaxDiveSpeed = 2000.0f;
    Movement->Velocity = FVector(0.0f, 0.0f, -3000.0f); // Over limit
    
    // Act
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    TestTrue(TEXT("Speed should be limited to MaxDiveSpeed"), 
            FMath::Abs(Movement->Velocity.Z) <= Movement->MaxDiveSpeed + 0.1f);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBirdMovementNaNHandlingTest, "BirdDive.Physics.BirdCharacterMovementComponent.NaNHandling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FBirdMovementNaNHandlingTest::RunTest(const FString& Parameters)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    TestNotNull(TEXT("BirdCharacterMovementComponent should be created"), Movement);
    
    // Act
    FVector NaNForce(NAN, NAN, NAN);
    Movement->ApplyWindForce(NaNForce, 0.016f);
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    TestTrue(TEXT("Velocity should remain finite after NaN input"), 
            FMath::IsFinite(Movement->Velocity.Size()));
    
    return true;
}

// ============================
// Integration Tests
// ============================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsIntegrationTest, "BirdDive.Physics.Integration.WindAndMovement",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FPhysicsIntegrationTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    TestNotNull(TEXT("WindForceComponent should be created"), WindComponent);
    TestNotNull(TEXT("BirdCharacterMovementComponent should be created"), Movement);
    
    FVector Position(0.0f, 0.0f, 1000.0f);
    
    // Act
    FVector WindForce = WindComponent->CalculateWindForce(Position, 1000.0f);
    FVector InitialVelocity = Movement->Velocity;
    Movement->ApplyWindForce(WindForce, 0.016f);
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    TestTrue(TEXT("Movement should respond to wind force"), 
            !Movement->Velocity.Equals(InitialVelocity, 0.1f));
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultipleForceIntegrationTest, "BirdDive.Physics.Integration.MultipleForces",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FMultipleForceIntegrationTest::RunTest(const FString& Parameters)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    TestNotNull(TEXT("BirdCharacterMovementComponent should be created"), Movement);
    
    FVector WindForce(100.0f, 0.0f, 0.0f);
    
    // Act - Apply wind and gravity simultaneously
    Movement->ApplyWindForce(WindForce, 0.016f);
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    TestTrue(TEXT("Should have horizontal component from wind"), 
            FMath::Abs(Movement->Velocity.X) > 0.1f);
    TestTrue(TEXT("Should have vertical component from gravity"), 
            Movement->Velocity.Z < 0.0f);
    
    return true;
}

// ============================
// Edge Case Tests
// ============================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsEdgeCaseZeroWindTest, "BirdDive.Physics.EdgeCases.ZeroWindForce",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FPhysicsEdgeCaseZeroWindTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    TestNotNull(TEXT("WindForceComponent should be created"), WindComponent);
    
    WindComponent->WindStrength = 0.0f;
    
    // Act
    FVector WindForce = WindComponent->CalculateWindForce(FVector::ZeroVector, 1000.0f);
    
    // Assert
    TestTrue(TEXT("Zero wind strength should produce minimal force"), 
            WindForce.IsNearlyZero(0.1f));
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsEdgeCaseExtremeHeightTest, "BirdDive.Physics.EdgeCases.ExtremeHeight",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FPhysicsEdgeCaseExtremeHeightTest::RunTest(const FString& Parameters)
{
    // Arrange
    UWindForceComponent* WindComponent = NewObject<UWindForceComponent>();
    TestNotNull(TEXT("WindForceComponent should be created"), WindComponent);
    
    // Act & Assert
    FVector WindAtZero = WindComponent->CalculateWindForce(FVector::ZeroVector, 0.0f);
    FVector WindAtMax = WindComponent->CalculateWindForce(FVector::ZeroVector, 100000.0f);
    
    TestTrue(TEXT("Wind at zero height should be finite"), 
            FMath::IsFinite(WindAtZero.Size()));
    TestTrue(TEXT("Wind at extreme height should be finite"), 
            FMath::IsFinite(WindAtMax.Size()));
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicsEdgeCaseNegativeValuesTest, "BirdDive.Physics.EdgeCases.NegativeValues",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FPhysicsEdgeCaseNegativeValuesTest::RunTest(const FString& Parameters)
{
    // Arrange
    UBirdCharacterMovementComponent* Movement = NewObject<UBirdCharacterMovementComponent>();
    TestNotNull(TEXT("BirdCharacterMovementComponent should be created"), Movement);
    
    // Act
    Movement->ApplyWindForce(FVector(-1000.0f, -1000.0f, -1000.0f), 0.016f);
    Movement->CalcVelocity(0.016f, 0.0f, false, 0.0f);
    
    // Assert
    TestTrue(TEXT("Should handle negative wind forces"), 
            FMath::IsFinite(Movement->Velocity.Size()));
    
    return true;
}