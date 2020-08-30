/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Build Settings of CriWare Module
 * File     : CriWare.Build.cs
 *
 ****************************************************************************/
using System.IO;
namespace UnrealBuildTool.Rules
{
	public class CriWare : ModuleRules
	{
		public CriWare(ReadOnlyTargetRules Target) : base(Target)
		{
			Type = ModuleType.External;

			string CriWarePath = Path.Combine(ModuleDirectory, "cri/");
            UnrealTargetPlatform TargetPlatformValue;

			if (Target.Platform == UnrealTargetPlatform.Win64) {
				string LibDir = CriWarePath + "pc/libs/x64/";

				PublicIncludePaths.Add(CriWarePath + "pc/include");
				bool VP9Extension = File.Exists(CriWarePath + "pc/include/cri_mana_vp9.h");
				bool McDspExtension = File.Exists(CriWarePath + "pc/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "pc/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_pcx64.lib",
							LibDir + "cri_file_system_pcx64.lib",
							LibDir + "cri_atom_pcx64.lib",
							LibDir + "cri_mana_pcx64.lib"
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibDir + "cri_mana_vpx_pcx64.lib");
					}
					if (McDspExtension) {
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_MD_pcx64.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_pcx64.lib");
					}
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_pcx64D.lib",
							LibDir + "cri_file_system_pcx64D.lib",
							LibDir + "cri_atom_monitor_pcx64D.lib",
							LibDir + "cri_mana_pcx64.lib"
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibDir + "cri_mana_vpx_pcx64.lib");
					}
					if (McDspExtension) {
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_MD_pcx64D.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_pcx64D.lib");
					}
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_pcx64.lib",
							LibDir + "cri_file_system_pcx64.lib",
							LibDir + "cri_atom_monitor_pcx64.lib",
							LibDir + "cri_mana_pcx64.lib"
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibDir + "cri_mana_vpx_pcx64.lib");
					}
					if (McDspExtension) {
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_MD_pcx64.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_pcx64.lib");
					}
				}
			} else if (Target.Platform == UnrealTargetPlatform.Win32) {
				string LibDir = CriWarePath + "pc/libs/x86/";

				PublicIncludePaths.Add(CriWarePath + "pc/include");
				bool VP9Extension = File.Exists(CriWarePath + "pc/include/cri_mana_vp9.h");
				bool McDspExtension = File.Exists(CriWarePath + "pc/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "pc/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_pcx86.lib",
							LibDir + "cri_file_system_pcx86.lib",
							LibDir + "cri_atom_pcx86.lib",
							LibDir + "cri_mana_pcx86.lib"
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibDir + "cri_mana_vpx_pcx86.lib");
					}
					if (McDspExtension) {
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_MD_pcx86.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_pcx86.lib");
					}
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_pcx86D.lib",
							LibDir + "cri_file_system_pcx86D.lib",
							LibDir + "cri_atom_monitor_pcx86D.lib",
							LibDir + "cri_mana_pcx86.lib"
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibDir + "cri_mana_vpx_pcx86.lib");
					}
					if (McDspExtension) {
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_MD_pcx86D.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_pcx86D.lib");
					}
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_pcx86.lib",
							LibDir + "cri_file_system_pcx86.lib",
							LibDir + "cri_atom_monitor_pcx86.lib",
							LibDir + "cri_mana_pcx86.lib"
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibDir + "cri_mana_vpx_pcx86.lib");
					}
					if (McDspExtension) {
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_MD_pcx86.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_pcx86.lib");
					}
				}
			} else if (Target.Platform == UnrealTargetPlatform.PS4) {
				string LibDir = CriWarePath + "ps4/libs/";

				PublicIncludePaths.Add(CriWarePath + "ps4/include");

                AddPublicSystemLibraries("SceHmd_stub_weak");
                AddPublicSystemLibraries("SceAudio3d_stub_weak");
                AddPublicSystemLibraries("SceHmdSetupDialog_stub_weak");
                //AddPublicSystemLibraries("SceVideodec_stub_weak");
                AddPublicSystemLibraries("SceVideodec2_stub_weak");
				bool McDspExtension = File.Exists(CriWarePath + "ps4/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "ps4/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "libcri_base_ps4.a",
							LibDir + "libcri_file_system_ps4.a",
							LibDir + "libcri_atom_ps4.a",
							LibDir + "libcri_mana_ps4.a",
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "libcriafx_mcdsp_ps4.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "libcri_lips_ps4.a");
					}
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "libcri_base_ps4D.a",
							LibDir + "libcri_file_system_ps4D.a",
							LibDir + "libcri_atom_monitor_ps4D.a",
							LibDir + "libcri_mana_ps4.a",
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "libcriafx_mcdsp_ps4D.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "libcri_lips_ps4D.a");
					}
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "libcri_base_ps4.a",
							LibDir + "libcri_file_system_ps4.a",
							LibDir + "libcri_atom_monitor_ps4.a",
							LibDir + "libcri_mana_ps4.a",
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "libcriafx_mcdsp_ps4.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "libcri_lips_ps4.a");
					}
				}
			} else if (Target.Platform == UnrealTargetPlatform.XboxOne) {
				string LibDir = CriWarePath + "xboxone/libs/";

				PublicIncludePaths.Add(CriWarePath + "xboxone/include");
				bool McDspExtension = File.Exists(CriWarePath + "xboxone/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "xboxone/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_xboxone.lib",
							LibDir + "cri_file_system_xboxone.lib",
							LibDir + "cri_atom_xboxone.lib",
							LibDir + "cri_mana_xboxone.lib",
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_xboxone.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_xboxone.lib");
					}
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_xboxoneD.lib",
							LibDir + "cri_file_system_xboxoneD.lib",
							LibDir + "cri_atom_monitor_xboxoneD.lib",
							LibDir + "cri_mana_xboxone.lib",
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_xboxoneD.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_xboxoneD.lib");
					}
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibDir + "cri_base_xboxone.lib",
							LibDir + "cri_file_system_xboxone.lib",
							LibDir + "cri_atom_monitor_xboxone.lib",
							LibDir + "cri_mana_xboxone.lib",
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "criafx_mcdsp_xboxone.lib");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibDir + "cri_lips_xboxone.lib");
					}
				}
			} else if (Target.Platform == UnrealTargetPlatform.Switch) {
				PublicIncludePaths.Add(CriWarePath + "switch/include");
				string LibraryPath = CriWarePath + "switch/libs/NX-NXFP2-a64/";
				bool McDspExtension = File.Exists(CriWarePath + "switch/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "switch/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "Release/libcri_base_switch.a",
							LibraryPath + "Release/libcri_file_system_switch.a",
							LibraryPath + "Release/libcri_atom_switch.a",
							LibraryPath + "Release/libcri_mana_switch.a"
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "Release/libcriafx_mcdsp_switch.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "Release/libcri_lips_switch.a");
					}
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "Debug/libcri_base_switch.a",
							LibraryPath + "Debug/libcri_file_system_switch.a",
							LibraryPath + "Debug/libcri_atom_monitor_switch.a",
							LibraryPath + "Debug/libcri_mana_switch.a"
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "Debug/libcriafx_mcdsp_switch.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "Debug/libcri_lips_switch.a");
					}
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "Develop/libcri_base_switch.a",
							LibraryPath + "Develop/libcri_file_system_switch.a",
							LibraryPath + "Develop/libcri_atom_monitor_switch.a",
							LibraryPath + "Develop/libcri_mana_switch.a"
						}
					);
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "Develop/libcriafx_mcdsp_switch.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "Develop/libcri_lips_switch.a");
					}
				}
			} else if (Target.Platform == UnrealTargetPlatform.Mac) {
				PublicFrameworks.AddRange(new string[] { "CoreAudio", "AudioToolbox", "AudioUnit", "CoreGraphics", "VideoToolbox", "OpenGL" });
				PublicIncludePaths.Add(CriWarePath + "macosx/include");
				string LibraryPath = CriWarePath + "macosx/libs/";
				bool VP9Extension = File.Exists(CriWarePath + "macosx/include/cri_mana_vp9.h");
				bool McDspExtension = File.Exists(CriWarePath + "macosx/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "macosx/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_macosx.a",
							LibraryPath + "libcri_file_system_macosx.a",
							LibraryPath + "libcri_atom_macosx.a",
							LibraryPath + "libcri_mana_macosx.a",
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_macosx.a");
					}
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_macosx.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_macosx.a");
					}
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_macosxD.a",
							LibraryPath + "libcri_file_system_macosxD.a",
							LibraryPath + "libcri_atom_monitor_macosxD.a",
							LibraryPath + "libcri_mana_macosx.a",
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_macosx.a");
					}
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_macosx.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_macosx.a");
					}
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_macosx.a",
							LibraryPath + "libcri_file_system_macosx.a",
							LibraryPath + "libcri_atom_monitor_macosx.a",
							LibraryPath + "libcri_mana_macosx.a",
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_macosx.a");
					}
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_macosx.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_macosx.a");
					}
				}
			} else if (Target.Platform == UnrealTargetPlatform.IOS) {
				PublicFrameworks.AddRange(new string[] { "CoreAudio", "AudioToolbox", "CoreGraphics", "VideoToolbox", "OpenGLES" });
				PublicIncludePaths.Add(CriWarePath + "ios/include");
				string LibraryPath = CriWarePath + "ios/libs/";
				bool VP9Extension = File.Exists(CriWarePath + "ios/include/cri_mana_vp9.h");
				bool McDspExtension = File.Exists(CriWarePath + "ios/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "ios/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_ios.a",
							LibraryPath + "libcri_file_system_ios.a",
							LibraryPath + "libcri_atom_ios.a",
							LibraryPath + "libcri_mana_ios.a",
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_ios.a");
					}
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_ios.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_ios.a");
					}
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_iosD.a",
							LibraryPath + "libcri_file_system_iosD.a",
							LibraryPath + "libcri_atom_monitor_iosD.a",
							LibraryPath + "libcri_mana_ios.a",
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_ios.a");
					}
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_iosD.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_iosD.a");
					}
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_ios.a",
							LibraryPath + "libcri_file_system_ios.a",
							LibraryPath + "libcri_atom_monitor_ios.a",
							LibraryPath + "libcri_mana_ios.a",
						}
					);
					if (VP9Extension) {
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_ios.a");
					}
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_ios.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_ios.a");
					}
				}
			} else if (Target.Platform == UnrealTargetPlatform.TVOS) {
                PublicFrameworks.AddRange(new string[] { "CoreAudio", "AudioToolbox", "CoreGraphics", "VideoToolbox", "OpenGLES" });
                PublicIncludePaths.Add(CriWarePath + "tvos/include");
                string LibraryPath = CriWarePath + "tvos/libs/";
                bool VP9Extension = File.Exists(CriWarePath + "tvos/include/cri_mana_vp9.h");
				bool McDspExtension = File.Exists(CriWarePath + "tvos/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "tvos/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
                	|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
                    PublicAdditionalLibraries.AddRange(
                    	new string[] {
                        	LibraryPath + "libcri_base_tvos.a",
                        	LibraryPath + "libcri_file_system_tvos.a",
                        	LibraryPath + "libcri_atom_tvos.a",
                        	LibraryPath + "libcri_mana_tvos.a",
                    	}
                    );
                    if (VP9Extension) {
                        PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_tvos.a");
                    }
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_tvos.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_tvos.a");
					}
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
                    PublicAdditionalLibraries.AddRange(
                    	new string[] {
                        	LibraryPath + "libcri_base_tvosD.a",
                        	LibraryPath + "libcri_file_system_tvosD.a",
                        	LibraryPath + "libcri_atom_monitor_tvosD.a",
                        	LibraryPath + "libcri_mana_tvos.a",
                    	}
                    );
                    if (VP9Extension) {
                        PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_tvos.a");
                    }
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_tvosD.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_tvosD.a");
					}
				} else {
                    PublicAdditionalLibraries.AddRange(
                    	new string[] {
                        	LibraryPath + "libcri_base_tvos.a",
                        	LibraryPath + "libcri_file_system_tvos.a",
                        	LibraryPath + "libcri_atom_monitor_tvos.a",
                        	LibraryPath + "libcri_mana_tvos.a",
                    	}
                    );
                    if (VP9Extension) {
                        PublicAdditionalLibraries.Add(LibraryPath + "libcri_mana_vpx_tvos.a");
                    }
					if (McDspExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcriafx_mcdsp_tvos.a");
					}
					if (AdxLipSyncExtension)
					{
						PublicAdditionalLibraries.Add(LibraryPath + "libcri_lips_tvos.a");
					}
				}
			} else if (Target.Platform == UnrealTargetPlatform.Linux) {
				PublicIncludePaths.Add(CriWarePath + "linux/include");
				string LibraryPath = CriWarePath + "linux/libs/x86_64-unknown-linux-gnu/";
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base.a",
							LibraryPath + "libcri_file_system.a",
							LibraryPath + "libcri_atom.a",
							LibraryPath + "libcri_mana.a",
						}
					);
					AddRangePublicSystemLibraries(
						new string[] {
							"asound",
							"pulse",
							"stdc++",
							"m",
							"gcc_s",
							"gcc",
							"c",
						}
					);
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_baseD.a",
							LibraryPath + "libcri_file_systemD.a",
							LibraryPath + "libcri_atomD.a",
							LibraryPath + "libcri_manaD.a",
						}
					);
					AddRangePublicSystemLibraries(
						new string[] {
							"asound",
							"pulse",
							"stdc++",
							"m",
							"gcc_s",
							"gcc",
							"c",
						}
					);
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base.a",
							LibraryPath + "libcri_file_system.a",
							LibraryPath + "libcri_atom.a",
							LibraryPath + "libcri_mana.a",
						}
					);
					AddRangePublicSystemLibraries(
						new string[] {
							"asound",
							"pulse",
							"stdc++",
							"m",
							"gcc_s",
							"gcc",
							"c",
						}
					);
				}
			} else if ((TargetPlatformTryParse("Quail", out TargetPlatformValue) && Target.Platform == TargetPlatformValue) 
                ||(TargetPlatformTryParse("Stadia", out TargetPlatformValue) && Target.Platform == TargetPlatformValue)) {
				PublicIncludePaths.Add(CriWarePath + "stadia/include");
				string LibraryPath = CriWarePath + "stadia/libs/";
				if (Target.Configuration == UnrealTargetConfiguration.Shipping) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_stadia.a",
							LibraryPath + "libcri_file_system_stadia.a",
							LibraryPath + "libcri_atom_stadia.a",
							LibraryPath + "libcri_mana_stadia.a",
						}
					);
					AddRangePublicSystemLibraries(
						new string[] {
							"pulse",
							"m",
							"gcc_s",
							"gcc",
							"c",
						}
					);
				} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_stadiaD.a",
							LibraryPath + "libcri_file_system_stadiaD.a",
							LibraryPath + "libcri_atom_stadiaD.a",
							LibraryPath + "libcri_mana_stadia.a",
						}
					);
					AddRangePublicSystemLibraries(
						new string[] {
							"pulse",
							"m",
							"gcc_s",
							"gcc",
							"c",
						}
					);
				} else {
					PublicAdditionalLibraries.AddRange(
						new string[] {
							LibraryPath + "libcri_base_stadia.a",
							LibraryPath + "libcri_file_system_stadia.a",
							LibraryPath + "libcri_atom_stadia.a",
							LibraryPath + "libcri_mana_stadia.a",
						}
					);
					AddRangePublicSystemLibraries(
						new string[] {
							"pulse",
							"m",
							"gcc_s",
							"gcc",
							"c",
						}
					);
				}
			} else if (Target.Platform == UnrealTargetPlatform.Android) {
				PublicIncludePaths.Add(CriWarePath + "android/include");
				string LibraryPath = CriWarePath + "android/libs/";
				var TargetArchs = new string[] {
					"armeabi-v7a",
					"arm64-v8a",
					/* "x86", */
					"x86_64"
				};
				bool VP9Extension = File.Exists(CriWarePath + "android/include/cri_mana_vp9.h");
				bool McDspExtension = File.Exists(CriWarePath + "android/include/criafx_mcdsp.h");
				bool AdxLipSyncExtension = File.Exists(CriWarePath + "android/include/cri_lips.h");
				if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
					|| (Target.Configuration == UnrealTargetConfiguration.Test))
				{
					AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
						new string[] {
							"libcri_base_android.a",
							"libcri_file_system_android.a",
							"libcri_atom_android.a",
							"libcri_mana_android.a"
						}
					);
					if (VP9Extension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcri_mana_vpx_android.a" });
					}
					if (McDspExtension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcriafx_mcdsp_android.a" });
						AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "CriMcDspAndroid.xml"));
					}
					if (AdxLipSyncExtension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcri_lips_android.a" });
					}
				}
				else if (Target.Configuration == UnrealTargetConfiguration.Debug)
				{
					AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
						new string[] {
							"libcri_base_androidD.a",
							"libcri_file_system_androidD.a",
							"libcri_atom_monitor_androidD.a",
							"libcri_mana_android.a"
						}
					);
					if (VP9Extension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcri_mana_vpx_android.a" });
					}
					if (McDspExtension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcriafx_mcdsp_android.a" });
						AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "CriMcDspAndroid.xml"));
					}
					if (AdxLipSyncExtension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcri_lips_android.a" });
					}
				}
				else
				{
					AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
						new string[] {
							"libcri_base_android.a",
							"libcri_file_system_android.a",
							"libcri_atom_monitor_android.a",
							"libcri_mana_android.a"
						}
					);
					if (VP9Extension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcri_mana_vpx_android.a" });
					}
					if (McDspExtension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcriafx_mcdsp_android.a" });
						AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "CriMcDspAndroid.xml"));
					}
					if (AdxLipSyncExtension)
					{
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs, new string[] { "libcri_lips_android.a" });
					}
				}
			} else {
				/* MICROSOFT_UWP_UNREAL - UWP support */
				if (TargetPlatformTryParse("UWP32", out TargetPlatformValue) && Target.Platform == TargetPlatformValue) {
					PublicIncludePaths.Add(CriWarePath + "uwp/include");
					var TargetArchs = new string[] {"x86"};
					var LibraryPath = Path.Combine(CriWarePath, "uwp/libs");
					if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
						|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
							new string[] {
								"cri_base_winrt_x86.lib",
								"cri_file_system_winrt_x86.lib",
								"cri_atom_winrt_x86.lib",
								"cri_mana_winrt_x86.lib"
							}
						);
					} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
							new string[] {
								"cri_base_winrt_x86.lib",
								"cri_file_system_winrt_x86.lib",
								"cri_atom_monitor_winrt_x86.lib",
								"cri_mana_winrt_x86.lib"
							}
						);
					} else {
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
							new string[] {
								"cri_base_winrt_x86.lib",
								"cri_file_system_winrt_x86.lib",
								"cri_atom_monitor_winrt_x86.lib",
								"cri_mana_winrt_x86.lib"
							}
						);
					}
				} else if (TargetPlatformTryParse("UWP64", out TargetPlatformValue) && Target.Platform == TargetPlatformValue) {
					PublicIncludePaths.Add(CriWarePath + "uwp/include");
					var TargetArchs = new string[] {"x64"};
					var LibraryPath = Path.Combine(CriWarePath, "uwp/libs");
					if ((Target.Configuration == UnrealTargetConfiguration.Shipping)
						|| (Target.Configuration == UnrealTargetConfiguration.Test)) {
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
							new string[] {
								"cri_base_winrt_x64.lib",
								"cri_file_system_winrt_x64.lib",
								"cri_atom_winrt_x64.lib",
								"cri_mana_winrt_x64.lib"
							}
						);
					} else if (Target.Configuration == UnrealTargetConfiguration.Debug) {
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
							new string[] {
								"cri_base_winrt_x64.lib",
								"cri_file_system_winrt_x64.lib",
								"cri_atom_monitor_winrt_x64.lib",
								"cri_mana_winrt_x64.lib"
							}
						);
					} else {
						AddCriLibrariesToPublicAdditionalLibraries(LibraryPath, TargetArchs,
							new string[] {
								"cri_base_winrt_x64.lib",
								"cri_file_system_winrt_x64.lib",
								"cri_atom_monitor_winrt_x64.lib",
								"cri_mana_winrt_x64.lib"
							}
						);
					}
				}
			}
		}

        public void AddPublicSystemLibraries(string LibName)
        {
            string TheObjectName = "PublicSystemLibraries";

            /* backward compatibility with 4.23 */
            BuildVersion Version;
            if (BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version) &&
                Version.MinorVersion < 24)
            {
                TheObjectName = "PublicAdditionalLibraries";
            }

            object TheObject = GetType().GetField(TheObjectName).GetValue(this);
            TheObject.GetType().GetMethod("Add").Invoke(TheObject, new object[] { LibName });
        }

        public void AddRangePublicSystemLibraries(string[] LibNameArray)
        {
            string TheObjectName = "PublicSystemLibraries";

            /* backward compatibility with 4.23 */
            BuildVersion Version;
            if (BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version) &&
                Version.MinorVersion < 24)
            {
                TheObjectName = "PublicAdditionalLibraries";
            }

            object TheObject = GetType().GetField(TheObjectName).GetValue(this);
            TheObject.GetType().GetMethod("AddRange").Invoke(TheObject, new object[] { LibNameArray });
        }

        public void AddCriLibrariesToPublicAdditionalLibraries(string LibraryPath, string[] Archs, string[] LibFilenames)
        {
            foreach (var Arch in Archs)
            {
                foreach (var LibFilename in LibFilenames)
                {
                    PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, Arch, LibFilename));
                }
            }
        }

		static public bool TargetPlatformTryParse(string Name, out UnrealTargetPlatform Platform)
		{
			/* backward compatibility with 4.24 */
			BuildVersion Version;
			if (BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version) &&
				Version.MinorVersion < 25)
			{
				return System.Enum.TryParse(Name, out Platform);
			}
			else
			{
				return UnrealTargetPlatform.TryParse(Name, out Platform);
			}
		}
    }
}