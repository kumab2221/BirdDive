// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class BirdDive : ModuleRules
{
	public BirdDive(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"EnhancedInput",
			"Slate",
			"SlateCore",
			"UMG",
			"PhysicsCore",
			"Chaos",
			"ChaosVehicles",
			"GeometryCollectionEngine"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Niagara",
			"NiagaraCore", 
			"ControlRig",
			"RigVM",
			"AnimGraphRuntime",
			"GameplayTags",
			"AIModule",
			"NavigationSystem",
			"MetasoundEngine",
			"AudioExtensions",
			"SignificanceManager",
			"RenderCore",
			"RHI",
			"DeveloperSettings"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {
				"AnimGraph",
				"BlueprintGraph"
			});
		}

		// Bird Dive Challenge specific configuration
		bEnableExceptions = false;
		bUseRTTI = false;
		CppCompileWarningSettings.UndefinedIdentifierWarningLevel = WarningLevel.Off;

		// Test configuration
		if (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {
				"AutomationController"
			});
		}
	}
}
