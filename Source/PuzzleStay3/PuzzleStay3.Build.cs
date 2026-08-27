// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PuzzleStay3 : ModuleRules
{
	public PuzzleStay3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
