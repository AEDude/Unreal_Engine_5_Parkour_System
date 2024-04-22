// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Technical_Animator : ModuleRules
{
	public Technical_Animator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
		"Core", 
		"CoreUObject", 
		"Engine", 
		"InputCore", 
		"EnhancedInput",  
		"MotionWarping",
		"GameplayTags",
		"Technical_Animator",
		"AnimGraphRuntime",
		"AnimationLocomotionLibraryRuntime"});
	}
}
