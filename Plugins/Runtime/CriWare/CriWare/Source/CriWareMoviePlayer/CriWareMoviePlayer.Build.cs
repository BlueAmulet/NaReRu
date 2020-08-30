/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2016 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Build Settings of CriWareMoviePlayer Module
 * File     : CriWareMoviePlayer.Build.cs
 *
 ****************************************************************************/
namespace UnrealBuildTool.Rules
{
	public class CriWareMoviePlayer : ModuleRules
	{
		public CriWareMoviePlayer(ReadOnlyTargetRules Target) : base(Target)
        {
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	        PublicDependencyModuleNames.AddRange(
	            new string[] {
	                "Engine",
	            }
	        );
	
	        PrivateDependencyModuleNames.AddRange(
				new string[] {
	                "Core",
	                "CoreUObject",
	                "MoviePlayer",
	                "Projects",
	                "RenderCore",
	                "RHI",
	                "Slate",
	                "SlateCore",
	                "CriWareRuntime",
	            }
			);
		}
	}
}