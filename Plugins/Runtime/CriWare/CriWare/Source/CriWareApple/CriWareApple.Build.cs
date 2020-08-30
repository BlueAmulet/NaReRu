/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE Apple plugin for Unreal Engine 4
 * Module   : Build Settings of CriWareApple Module
 * File     : CriWareApple.Build.cs
 *
 ****************************************************************************/
namespace UnrealBuildTool.Rules
{
	public class CriWareApple : ModuleRules
	{
		public CriWareApple(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PrivateIncludePaths.Add("CriWareApple/Private");

			PublicDependencyModuleNames.AddRange(
				new string[] {
					"Engine",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"Core",
					"CoreUObject",
					"Projects",
					"RenderCore",
					"RHI",
					"MetalRHI",
					"CriWareRuntime",
				}
			);
		}
	}
}
