// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PuzzleStay3 : ModuleRules
{
	public PuzzleStay3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "SlateCore", "ModelViewViewModel" });
	}
}
