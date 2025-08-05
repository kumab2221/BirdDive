#include "BirdCharacter.h"
#include "../../BirdDive.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABirdCharacter::ABirdCharacter()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Initialize state
	bHasBegunPlay = false;

	// Basic character setup - minimal implementation
	GetCapsuleComponent()->SetCapsuleHalfHeight(50.0f);
	GetCapsuleComponent()->SetCapsuleRadius(30.0f);

	// Basic movement settings
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 700.0f;

	UE_LOG(LogBirdDive, Log, TEXT("BirdCharacter constructed"));
}

void ABirdCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	bHasBegunPlay = true;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdCharacter BeginPlay"));
}

void ABirdCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bHasBegunPlay = false;
	
	UE_LOG(LogBirdDive, Log, TEXT("BirdCharacter EndPlay"));
	
	Super::EndPlay(EndPlayReason);
}

void ABirdCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Minimal tick implementation - just for performance testing
	// Actual flight logic will be added in later tasks
}

void ABirdCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Minimal input setup
	if (PlayerInputComponent)
	{
		UE_LOG(LogBirdDive, Log, TEXT("BirdCharacter input setup complete"));
	}
}