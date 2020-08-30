// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class VRES_ver1_2EditorTarget : TargetRules
{
	public VRES_ver1_2EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "VRES_ver1_2" } );
	}
}
