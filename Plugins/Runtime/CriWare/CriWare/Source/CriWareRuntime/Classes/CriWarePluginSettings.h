/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2015 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : PluginSettings
* File     : CriWarePluginSettings.h
*
****************************************************************************/

/* 備考）SSourceControlSubmit.cpp を参考に作成。 */

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* Unreal Engine 4関連ヘッダ */
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "Engine/EngineTypes.h"

/* CRIWARE Plugin Headers */
#include "CriWareApi.h"

/* モジュールヘッダ */
#include "CriWarePluginSettings.generated.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
#define NUM_FILESYSTEM_SECTIONS 12
#define NUM_ATOM_SECTIONS 19
#define NUM_MANA_SECTIONS 4

#define NUM_ENGINEDIR_INIFILE 4
#define NUM_PROJECTDIR_INIFILE 4
#else	/* </cri_delete_if_LE> */
#define NUM_FILESYSTEM_SECTIONS 6
#define NUM_ATOM_SECTIONS 10

#define NUM_ENGINEDIR_INIFILE 2
#define NUM_PROJECTDIR_INIFILE 2
#endif
#define NUM_INIFILE (NUM_ENGINEDIR_INIFILE + NUM_PROJECTDIR_INIFILE)

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
/* CriWareIniPathに格納されるPathと配列の番号対応 */
UENUM()
namespace EEngineIniFileType
{
	/* EEngineIniFileType::Type should be typical enums (not class enums). */
	enum Type
	{
		EngBaseEngineIni = 0,
		EngWindowsEngineIni,
#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
		EngPS4EngineIni,
		EngXboxOneEngineIni,
#endif	/* </cri_delete_if_LE> */
		ProjDefaultEngineIni,
		ProjWindowsEngineIni,
#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
		ProjPS4EngineIni,
		ProjXboxOneEngineIni
#endif	/* </cri_delete_if_LE> */
	};
}

/* サウンドレンダラタイプ */
UENUM()
namespace EAtomSoundRendererType
{
	/* EAtomSoundRendererType::Type should be typical enums (not class enums). */
	enum Type
	{
		/* 未指定 */
		Any = CriAtomSoundRendererType::CRIATOM_SOUND_RENDERER_ANY,

		/* Native Sound Renderer */
		Native = CriAtomSoundRendererType::CRIATOM_SOUND_RENDERER_NATIVE,

		/* Atom Sound Renderer */
		Asr = CriAtomSoundRendererType::CRIATOM_SOUND_RENDERER_ASR,

		/* Hardware */
		Hardware1 = CriAtomSoundRendererType::CRIATOM_SOUND_RENDERER_HW1,
		Hardware2 = CriAtomSoundRendererType::CRIATOM_SOUND_RENDERER_HW2,
		Hardware3 = CriAtomSoundRendererType::CRIATOM_SOUND_RENDERER_HW3,
		Hardware4 = CriAtomSoundRendererType::CRIATOM_SOUND_RENDERER_HW4,

		/* PS4 Pad Speaker */
		Pad = 255
	};
}

/** Loop settings used to overwrite default Cue settings of CueSheet presets.
 * @deprected Use EAtomLoopSetting instead.
 */
UENUM()
enum class EAtomLoopSettingID_DEPRECEATED : uint8
{
	DefaultLoop_DEPRECATED UMETA(DisplayName = "Default", ToolTip = "Use preset loop setting."),
	ForceLoop_DEPRECATED UMETA(DisplayName = "Force Loop", ToolTip = "Loop sounds."),
	IgnoreLoop_DEPRECATED UMETA(DisplayName = "Ignore Loop", ToolTip = "play sounds one shot.")
};

/** Leveled loop settings
 * Cue Settings Priority Order (Highest to Lowest): 
 *  - AtomComponent (Current Atom settings for playback).
 *  - SoundAtomCue (Local mutable cue settings).
 *  - SoundAtomCueSheet (immutable Atom cue preset).
 */
UENUM(BlueprintType)
enum class EAtomLoopSetting : uint8
{
	Inherited UMETA(DisplayName = "Inherited", ToolTip = "Use loop setting from lower level (SoundAtomCue or Cue presets stored in SoundAtomCueSheet)."),
	Loop UMETA(DisplayName = "Loop", ToolTip = "Force loop setting and always play sounds looped."),
	OneShot UMETA(DisplayName = "One Shot", ToolTip = "Ignore loop setting and always play sounds one shot.")
};

/* ASRラック設定 */
USTRUCT()
struct FAtomAsrRackConfigUI
{
	GENERATED_USTRUCT_BODY()

	/* サウンドレンダラタイプ */
	UPROPERTY(EditAnywhere, Category="AtomAsrRackConfig", meta = (DisplayName = "Sound Renderer Type"))
	TEnumAsByte<EAtomSoundRendererType::Type> SoundRendererTypeUI;

	/* ID */
	UPROPERTY(VisibleAnywhere, Category = "AtomAsrRackConfig")
	int32 ElementID;
};

/* ASRラック設定 */
USTRUCT()
struct FAtomAsrRackConfig
{
	GENERATED_USTRUCT_BODY()

	/* サウンドレンダラタイプ */
	UPROPERTY(VisibleAnywhere, Category = "AtomAsrRackConfig")
	int32 SoundRendererType;

	/* ID */
	UPROPERTY(VisibleAnywhere, Category = "AtomAsrRackConfig")
	int32 ElementID;
};

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/
UCLASS(config = Engine, defaultconfig, meta=(ToolTip="CriWarePluginSettings class."))
class CRIWARERUNTIME_API UCriWarePluginSettings : public UObject
{
	GENERATED_UCLASS_BODY()


	/* false:CriWare.iniが存在する、true:CriWare.iniが存在しない */
	/* CriWare.iniが存在する場合ProjectSettingsからのパラメータ編集を可否を決定するeditcondition用の変数 */
	/* ProjectSettingsからのパラメータ変更をできなくするための変数 */
	UPROPERTY(config, VisibleDefaultsOnly, Category = "CheckExistingOldConfigrationFiles", AdvancedDisplay,
		meta = (
			ConsoleVariable = "IsExistingOldFormatConfigFile",
			DisplayName = "Is Not Existing Old Format Configration File",
			Tooltip = "Not existing: checked, existing: unchecked."
			)
	)
	bool bNoExistCriWareIni;

	UPROPERTY(VisibleDefaultsOnly, Category = "CheckIsEnableToEditCriWareAdxLipSyncSetting")
	bool bEditableCriWareAdxLipSyncSetting = false;

	/* === FileSystem関連パラメータ === */
	/* コンテンツ配置場所のパス（UI表示用） */
	UPROPERTY(EditAnyWhere, Category = "FileSystem",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "NonAssetContentDir",
			DisplayName = "Non-Asset Content Directory",
			Tooltip = "Content directory where non-asset files are stored."
			)
	)
	FDirectoryPath NonAssetContentDir;

	/* コンテンツ配置場所のパス文字列（iniファイル書き込み用） */
	UPROPERTY(config)
	FString ContentDir;
	FString AppliedContentDir;

	/* バインダ数 */
	UPROPERTY(config, EditAnyWhere, Category = "FileSystem",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "NumBinders",
			DisplayName = "Number of Binders",
			Tooltip = "Number of binders."
		)
	)
	int32 NumBinders;
	int32 AppliedNumBinders;

	/* 最大バインド回数 */
	UPROPERTY(config, EditAnyWhere, Category = "FileSystem",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "MaxBinds",
			DisplayName = "Maximum Number of Binds",
			Tooltip = "Maximum number of binds."
		)
	)
	int32 MaxBinds;
	int32 AppliedMaxBinds;

	/* ローダ数 */
	UPROPERTY(config, EditAnyWhere, Category = "FileSystem",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "NumLoaders",
			DisplayName = "Number of Loaders",
			Tooltip = "Number of loaders."
		)
	)
	int32 NumLoaders;
	int32 AppliedNumLoaders;

	/* パスの最大長 */
	UPROPERTY(config, EditAnyWhere, Category = "FileSystem",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "MaxPath",
			DisplayName = "Maximum Path Length",
			Tooltip = "Maximum length for a path."
		)
	)
	int32 MaxPath;
	int32 AppliedMaxPath;

	/* ログ出力を行うかどうかの指定値 */
	UPROPERTY(config, EditAnyWhere, Category = "FileSystem",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "OutputsLogFileSystem",
			DisplayName = "Output log",
			Tooltip = "Enable log output."
		)
	)
	bool OutputsLogFileSystem;
	bool AppliedOutputsLogFileSystem;

	/* <cri_delete_if_LE> */
	/* PS4: コアアフィニティ設定値 */
	UPROPERTY(config, EditAnyWhere, Category = "FileSystem", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ClampMax = "767",
			ConsoleVariable = "PS4_FileAccessThreadAffinityMask",
			DisplayName = "PS4 File Access Thread Affinity Mask",
			Tooltip = "PS4 file access thread affinity mask."
		)
	)
	int32 PS4_FileAccessThreadAffinityMask;
	int32 AppliedPS4_FileAccessThreadAffinityMask;

	UPROPERTY(config, EditAnyWhere, Category = "FileSystem", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ClampMax = "767",
			ConsoleVariable = "PS4_DataDecompressionThreadAffinityMask",
			DisplayName = "PS4 Data Decompression Thread Affinity Mask",
			Tooltip = "PS4 data decompression thread affinity mask."
		)
	)
	int32 PS4_DataDecompressionThreadAffinityMask;
	int32 AppliedPS4_DataDecompressionThreadAffinityMask;

	UPROPERTY(config, EditAnyWhere, Category = "FileSystem", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ClampMax = "767",
			ConsoleVariable = "PS4_MemoryFileSystemThreadAffinityMask",
			DisplayName = "PS4 Memory File System Thread Affinity Mask",
			Tooltip = "PS4 memory file system thread affinity mask."
		)
	)
	int32 PS4_MemoryFileSystemThreadAffinityMask;
	int32 AppliedPS4_MemoryFileSystemThreadAffinityMask;

	/* PS4: スレッドプライオリティ設定値 */
	UPROPERTY(config, EditAnyWhere, Category = "FileSystem", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "256",
			ClampMax = "767",
			ConsoleVariable = "PS4_FileAccessThreadPriority",
			DisplayName = "PS4 File Access Thread Priority",
			Tooltip = "PS4 file access thread priority."
		)
	)
	int32 PS4_FileAccessThreadPriority;
	int32 AppliedPS4_FileAccessThreadPriority;

	UPROPERTY(config, EditAnyWhere, Category = "FileSystem", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "256",
			ClampMax = "767",
			ConsoleVariable = "PS4_DataDecompressionThreadPriority",
			DisplayName = "PS4 Data Decompression Thread Priority",
			Tooltip = "PS4 data decompression thread priority."
		)
	)
	int32 PS4_DataDecompressionThreadPriority;
	int32 AppliedPS4_DataDecompressionThreadPriority;

	UPROPERTY(config, EditAnyWhere, Category = "FileSystem", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "256",
			ClampMax = "767",
			ConsoleVariable = "PS4_MemoryFileSystemThreadPriority",
			DisplayName = "PS4 Memory File System Thread Priority",
			Tooltip = "PS4 memory file system thread priority."
		)
	)
	int32 PS4_MemoryFileSystemThreadPriority;
	int32 AppliedPS4_MemoryFileSystemThreadPriority;
	/* </cri_delete_if_LE> */

	/* === ATOM関連パラメータ === */
	/* キューアセットを自動的に作成するかどうかの指定値 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "AutomaticallyCreateCueAsset",
			DisplayName = "Automatically Create Cue Asset",
			Tooltip = "Automatically create cue asset at ACB file import."
		)
	)
	bool AutomaticallyCreateCueAsset;

	/* インゲームプレビューを使用するかどうかの指定値 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "UsesInGamePreview",
			DisplayName = "Use In Game Preview",
			Tooltip = "Enable In-Game-Preview functionality."
		)
	)
	bool UsesInGamePreview;
	bool AppliedUsesInGamePreview;

	/* ログ出力を行うかどうかの指定値 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "OutputsLogAtom",
			DisplayName = "Output Log",
			Tooltip = "Enable log output."
		)
	)
	bool OutputsLogAtom;
	bool AppliedOutputsLogAtom;

	/* ログ出力を行うかどうかの指定値 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "MonitorCommunicationBufferSize",
			ClampMin = "2097152",//2*1024*1024
			ConsoleVariable = "MonitorCommunicationBufferSize",
			DisplayName = "Atom Monitor Communication Buffer Size",
			Tooltip = "Atom Monitor Communication Buffer Size for communicating with CRI Atom Craft tool."
			)
	)
	int32 MonitorCommunicationBufferSize;
	int32 AppliedMonitorCommunicationBufferSize;

	/* 最大バーチャルボイス数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ClampMax = "32767",
			ConsoleVariable = "MaxVirtualVoices",
			DisplayName = "Maximum Number of Virtual Voices",
			Tooltip = "Maximum number of virtual voices."
		)
	)
	int32 MaxVirtualVoices;
	int32 AppliedMaxVirtualVoices;

	/* メモリ再生に使用するボイス数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "NumStandardMemoryVoices",
			DisplayName = "Number of Standard Memory Voices",
			Tooltip = "Number of standard memory voices."
		)
	)
	int32 NumStandardMemoryVoices;
	int32 AppliedNumStandardMemoryVoices;

	/* メモリ再生用ボイスで再生可能な最大チャンネル数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "StandardMemoryVoiceNumChannels",
			DisplayName = "Number of Channels of Standard Memory Voice",
			Tooltip = "Number of channels of standard memory voice."
		)
	)
	int32 StandardMemoryVoiceNumChannels;
	int32 AppliedStandardMemoryVoiceNumChannels;

	/* メモリ再生用ボイスで再生可能な最大サンプリングレート */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "StandardMemoryVoiceSamplingRate",
			DisplayName = "Sampling Rate of Standard Memory Voice",
			Tooltip = "Standard memory voice sampling rate."
		)
	)
	int32 StandardMemoryVoiceSamplingRate;
	int32 AppliedStandardMemoryVoiceSamplingRate;

	/* ストリーム再生兼用（メモリ再生とストリーム再生の両方が可能な）ボイスの数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "NumStandardStreamingVoices",
			DisplayName = "Number of Standard Streaming Voices",
			Tooltip = "Number of standard streaming voices."
		)
	)
	int32 NumStandardStreamingVoices;
	int32 AppliedNumStandardStreamingVoices;

	/* ストリーム再生兼用ボイスで再生可能な最大チャンネル数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "StandardStreamingVoiceNumChannels",
			DisplayName = "Number of Channels of Standard Streaming Voice",
			Tooltip = "Number of channels of standard streaming voice."
		)
	)
	int32 StandardStreamingVoiceNumChannels;
	int32 AppliedStandardStreamingVoiceNumChannels;

	/* ストリーム再生兼用ボイスで再生可能な最大サンプリングレート */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "StandardStreamingVoiceSamplingRate",
			DisplayName = "Sampling Rate of Standard Streaming Voice",
			Tooltip = "Standard streaming voice sampling rate."
		)
	)
	int32 StandardStreamingVoiceSamplingRate;
	int32 AppliedStandardStreamingVoiceSamplingRate;

	UPROPERTY(config)
	bool PoolAtomComponent;

	UPROPERTY(config, EditAnywhere, Category="Atom",
		meta=(
			editcondition = "bNoExistCriWareIni",
			AllowedClasses="SoundAtomConfig",
			ConsoleVariable = "AtomConfig",
			DisplayName = "Atom Config",
			Tooltip = "Atom Config asset."
		)
	)
	FSoftObjectPath AtomConfig;
	FSoftObjectPath AppliedAtomConfig;

	/* DEPRECATED - Use AtomConfig instead. */
	UPROPERTY(config)
	FString AcfFileName;

	UPROPERTY(config, EditAnywhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			AllowedClasses = "DataTable",
			ConsoleVariable = "AcfDataTable",
			DisplayName = "Atom Config Data Table",
			Tooltip = "Atom Config asset."
			)
	)
	FSoftObjectPath AtomConfigDataTable;
	FSoftObjectPath AppliedAtomConfigDataTable;
	
	/* 距離係数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "DistanceFactor",
			DisplayName = "Distance Factor",
			Tooltip = "Atom distance factor."
		)
	)
	float DistanceFactor;
	float AppliedDistanceFactor;

	/* サウンドレンダラタイプ */
	UPROPERTY(EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "SoundRendererType",
			DisplayName = "Sound Renderer Type",
			Tooltip = "Sound renderer type of library output."
		)
	)
	TEnumAsByte<EAtomSoundRendererType::Type> SoundRendererTypeUI;

	UPROPERTY(config)
	int32 SoundRendererType;
	int32 AppliedSoundRendererType;

	/* ASRラック設定 */
	UPROPERTY(EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "AsrRackConfig",
			DisplayName = "Asr Rack Config",
			Tooltip = "Configuration of additional ASR Rack."
		)
	)
	TArray<FAtomAsrRackConfigUI> AsrRackConfigUI;

	UPROPERTY(config)
	TArray<FAtomAsrRackConfig> AsrRackConfig;
	TArray<FAtomAsrRackConfig> AppliedAsrRackConfig;

	UPROPERTY(config)
	FString Hardware1;

	UPROPERTY(config)
	FString Hardware2;

	UPROPERTY(config)
	FString Hardware3;

	UPROPERTY(config)
	FString Hardware4;

	/* Economic-Tick機能用のマージン距離 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "EconomicTickMarginDistance",
			DisplayName = "Economic Tick Margin Distance (Unreal Unit)",
			Tooltip = "While the sound source is far enough from the listener, reduce the frequency of the 'Tick' processing. If the distance between the sound source and the listener is equal to or greater than the 'maximum attenuation-distance' + 'Economic Tick Margin', it is considered to be far enough."
			)
	)
	float EconomicTickMarginDistance;
	float AppliedEconomicTickMarginDistance;

	/* Economic-Tick効果適用時のTickの頻度(回/秒) */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "EconomicTickFrequency",
			DisplayName = "Economic Tick Frequency (Tick/Second)",
			Tooltip = "The frequency of Economic Tick (Tick/Second)."
			)
	)
	int32 EconomicTickFrequency;
	int32 AppliedEconomicTickFrequency;

	/* Culling機能用のマージン距離 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "CullingMarginDistance",
			DisplayName = "Culling Margin Distance (Unreal Unit)",
			Tooltip = "If the sound source is far enough from the listener, ignore the play command. If the distance between the sound source and the listener is equal to or greater than the 'maximum attenuation-distance' + 'Culling Margin', it is considered to be far enough."
			)
	)
	float CullingMarginDistance;
	float AppliedCullingMarginDistance;

	/* HCA-MXボイスのサンプリングレート */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "HcaMxVoiceSamplingRate",
			DisplayName = "Sampling Rate of HCA-MX Voice",
			Tooltip = "HCA-MX voice sampling rate."
		)
	)
	int32 HcaMxVoiceSamplingRate;
	int32 AppliedHcaMxVoiceSamplingRate;

	/* HCA-MXメモリ再生に使用するボイス数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "NumHcaMxMemoryVoices",
			DisplayName = "Number of HCA-MX Memory Voices",
			Tooltip = "Number of HCA-MX memory voices."
		)
	)
	int32 NumHcaMxMemoryVoices;
	int32 AppliedNumHcaMxMemoryVoices;

	/* HCA-MXメモリ再生用ボイスで再生可能な最大チャンネル数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "HcaMxMemoryVoiceNumChannels",
			DisplayName = "Number of Channels of HCA-MX Memory Voice",
			Tooltip = "Number of channels of HCA-MX memory voice."
		)
	)
	int32 HcaMxMemoryVoiceNumChannels;
	int32 AppliedHcaMxMemoryVoiceNumChannels;

	/* HCA-MXストリーム再生兼用（メモリ再生とストリーム再生の両方が可能な）ボイスの数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "NumHcaMxStreamingVoices",
			DisplayName = "Number of HCA-MX Streaming Voices",
			Tooltip = "Number of HCA-MX streaming voices."
		)
	)
	int32 NumHcaMxStreamingVoices;
	int32 AppliedNumHcaMxStreamingVoices;

	/* HCA-MXストリーム再生兼用ボイスで再生可能な最大チャンネル数 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "HcaMxStreamingVoiceNumChannels",
			DisplayName = "Number of Channels of HCA-MX Streaming Voice",
			Tooltip = "Number of channels of HCA-MX streaming voice."
		)
	)
	int32 HcaMxStreamingVoiceNumChannels;
	int32 AppliedHcaMxStreamingVoiceNumChannels;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "WASAPI_IsExclusive",
			DisplayName = "WASAPI Is Exclusive",
			Tooltip = "WASAPI is exclusive."
		)
	)
	bool WASAPI_IsExclusive;
	bool AppliedWASAPI_IsExclusive;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "WASAPI_BitsPerSample",
			DisplayName = "WASAPI Bits Per Sample",
			Tooltip = "WASAPI bits per sample."
		)
	)
	int32 WASAPI_BitsPerSample;
	int32 AppliedWASAPI_BitsPerSample;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "WASAPI_SamplingRate",
			DisplayName = "WASAPI Sampling Rate",
			Tooltip = "WASAPI sampling rate."
		)
	)
	int32 WASAPI_SamplingRate;
	int32 AppliedWASAPI_SamplingRate;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "WASAPI_NumChannels",
			DisplayName = "WASAPI Number of Output Channels",
			Tooltip = "WASAPI number of output channels."
		)
	)
	int32 WASAPI_NumChannels;
	int32 AppliedWASAPI_NumChannels;

	/* <cri_delete_if_LE> */
	/* PS4: コアアフィニティ設定値 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ClampMax = "767",
			ConsoleVariable = "PS4_ServerThreadAffinityMask",
			DisplayName = "PS4 Server Thread Affinity Mask",
			Tooltip = "PS4 server thread affinity mask."
		)
	)
	int32 PS4_ServerThreadAffinityMask;
	int32 AppliedPS4_ServerThreadAffinityMask;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ClampMax = "767",
			ConsoleVariable = "PS4_OutputThreadAffinityMask",
			DisplayName = "PS4 Output Thread Affinity Mask",
			Tooltip = "PS4 output thread affinity mask."
		)
	)
	int32 PS4_OutputThreadAffinityMask;
	int32 AppliedPS4_OutputThreadAffinityMask;

	/* PS4: スレッドプライオリティ設定値 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "256",
			ClampMax = "767",
			ConsoleVariable = "PS4_ServerThreadPriority",
			DisplayName = "PS4 Server Thread Priority",
			Tooltip = "PS4 server thread priority."
		)
	)
	int32 PS4_ServerThreadPriority;
	int32 AppliedPS4_ServerThreadPriority;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "256",
			ClampMax = "767",
			ConsoleVariable = "PS4_OutputThreadPriority",
			DisplayName = "PS4 Output Thread Priority",
			Tooltip = "PS4 output thread priority."
		)
	)
	int32 PS4_OutputThreadPriority;
	int32 AppliedPS4_OutputThreadPriority;

	/* Audio3d設定用パラメータ */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "PS4_UseAudio3d",
			DisplayName = "PS4 Use Audio3d",
			Tooltip = "PS4 use Audio3d."
			)
	)
	bool PS4_UseAudio3d;
	bool AppliedPS4_UseAudio3d;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "PS4_NumberOfAudio3dMemoryVoices",
			DisplayName = "PS4 Number of Audio3d Memory Voices",
			Tooltip = "PS4 number of audio3d memory voices."
			)
	)
	int32 PS4_NumberOfAudio3dMemoryVoices;
	int32 AppliedPS4_NumberOfAudio3dMemoryVoices;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "PS4_SamplingRateOfAudio3dMemoryVoices",
			DisplayName = "PS4 Sampling Rate of Audio3d Memory Voices",
			Tooltip = "PS4 sampling rate of Audio3d memory voices."
			)
	)
	int32 PS4_SamplingRateOfAudio3dMemoryVoices;
	int32 AppliedPS4_SamplingRateOfAudio3dMemoryVoices;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "PS4_NumberOfAudio3dStreamingVoices",
			DisplayName = "PS4 Number of Audio3d Streaming Voices",
			Tooltip = "PS4 number of Audio3d streaming voices."
			)
	)
	int32 PS4_NumberOfAudio3dStreamingVoices;
	int32 AppliedPS4_NumberOfAudio3dStreamingVoices;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "PS4_SamplingRateOfAudio3dStreamingVoices",
			DisplayName = "PS4 Sampling Rate of Audio3d Streaming Voices",
			Tooltip = "PS4 sampling rate of Audio3d streaming voices."
			)
	)
	int32 PS4_SamplingRateOfAudio3dStreamingVoices;
	int32 AppliedPS4_SamplingRateOfAudio3dStreamingVoices;

	/* Switch Opus用パラメータ */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "Switch_NumOpusMemoryVoices",
			DisplayName = "Switch Number of Opus Memory Voices",
			Tooltip = "Switch number of Opus memory voices."
		)
	)
	int32 Switch_NumOpusMemoryVoices;
	int32 AppliedSwitch_NumOpusMemoryVoices;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "Switch_OpusMemoryVoiceNumChannels",
			DisplayName = "Switch Number of Channels of Opus Memory Voice",
			Tooltip = "Switch number of channels of Opus memory voice."
		)
	)
	int32 Switch_OpusMemoryVoiceNumChannels;
	int32 AppliedSwitch_OpusMemoryVoiceNumChannels;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "Switch_OpusMemoryVoiceSamplingRate",
			DisplayName = "Switch Sampling Rate of Opus Memory Voice",
			Tooltip = "Switch Opus memory voice sampling rate."
		)
	)
	int32 Switch_OpusMemoryVoiceSamplingRate;
	int32 AppliedSwitch_OpusMemoryVoiceSamplingRate;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "Switch_NumOpusStreamingVoices",
			DisplayName = "Switch Number of Opus Streaming Voices",
			Tooltip = "Switch number of Opus streaming voices."
		)
	)
	int32 Switch_NumOpusStreamingVoices;
	int32 AppliedSwitch_NumOpusStreamingVoices;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "Switch_OpusStreamingVoiceNumChannels",
			DisplayName = "Switch Number of Channels of Opus Streaming Voice",
			Tooltip = "Switch number of channels of Opus streaming voice."
		)
	)
	int32 Switch_OpusStreamingVoiceNumChannels;
	int32 AppliedSwitch_OpusStreamingVoiceNumChannels;

	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ClampMin = "0",
			ConsoleVariable = "Switch_OpusStreamingVoiceSamplingRate",
			DisplayName = "Switch Sampling Rate of Opus Streaming Voice",
			Tooltip = "Switch Opus streaming voice sampling rate."
		)
	)
	int32 Switch_OpusStreamingVoiceSamplingRate;
	int32 AppliedSwitch_OpusStreamingVoiceSamplingRate;

	/* UE4標準のサウンド出力モジュールによって ADX サウンドを再生するかどうかの指定値 */
	UPROPERTY(config, EditAnyWhere, Category = "Atom", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "UseUnrealSoundRenderer",
			DisplayName = "Use Unreal Sound Renderer (Editor only)",
			Tooltip = "Bypass ADX output to UE4 sound renderer."
			)
	)
	bool UseUnrealSoundRenderer;
	bool AppliedUseUnrealSoundRenderer;

	/* === Mana関連パラメータ === */
	UPROPERTY(config, EditAnyWhere, Category = "Mana",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "InitializeMana",
			DisplayName = "Initialize Mana",
			Tooltip = "Initialize Mana."
		)
	)
	bool InitializeMana;
	bool AppliedInitializeMana;

	UPROPERTY(config, EditAnyWhere, Category = "Mana",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "EnableDecodeSkip",
			DisplayName = "Enable Decode Skip",
			Tooltip = "Enable decode skipping."
		)
	)
	bool EnableDecodeSkip;
	bool AppliedEnableDecodeSkip;

	UPROPERTY(config, EditAnyWhere, Category = "Mana",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "MaxDecoderHandles",
			DisplayName = "Maximum Number of Decoder Handles",
			Tooltip = "Maximum of decoder handles."
		)
	)
	int32 MaxDecoderHandles;
	int32 AppliedMaxDecoderHandles;

	UPROPERTY(config, EditAnyWhere, Category = "Mana",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "MaxManaBPS",
			DisplayName = "Maximum Bitrate of Movie",
			Tooltip = "Maximun bitrate of Mana movie in BPS."
		)
	)
	int32 MaxManaBPS;
	int32 AppliedMaxManaBPS;

	UPROPERTY(config, EditAnyWhere, Category = "Mana",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "MaxManaStreams",
			DisplayName = "Maximum Number of Movie Streams",
			Tooltip = "Maximun number of streams to allocate for Movies. Overall stream bandwith can be shared between multiple movie sources. Using less streams reduce used resources but may result in latency in audio, and movie frame drops."
			)
	)
	int32 MaxManaStreams;
	int32 AppliedMaxManaStreams;

	UPROPERTY(config, EditAnyWhere, Category = "Mana",
		meta = (
			editcondition = "bNoExistCriWareIni",
			ConsoleVariable = "UseH264Decoder",
			DisplayName = "Use Hardware Decoder",
			Tooltip = "Use Hardware Decoder. (Allow H264 decoding and H/W VP9 decoding if available.)"
			)
	)
	bool UseH264Decoder;
	bool AppliedUseH264Decoder;

	/* === ADX LipSync 関連パラメータ === */
	UPROPERTY(config, EditAnyWhere, Category = "ADX LipSync",
		meta = (
			editcondition = "bEditableCriWareAdxLipSyncSetting",
			ConsoleVariable = "InitializeAdxLipSync",
			DisplayName = "Initialize ADX LipSync",
			Tooltip = "Initialize ADX LipSync Library.")
	)
	bool InitializeAdxLipSync;
	bool AppliedInitializeAdxLipSync;

	UPROPERTY(config, EditAnyWhere, Category = "ADX LipSync",
		meta = (
			editcondition = "bEditableCriWareAdxLipSyncSetting",
			ConsoleVariable = "MaxNumAnalyzerHandles",
			DisplayName = "Maximum Number of Analyzer Handles",
			Tooltip = "Maximun number of criAtomLipsAnalyzer handles.",
			ClampMin = 1, ClampMax = 64
			)
	)
	int32 MaxNumAnalyzerHandles;
	int32 AppliedMaxNumAnalyzerHandles;

	////// BOOT VIDEO //////

	/** If enabled, Unreal Startup movies are overrided by Mana Startup movies. */
	UPROPERTY(globalconfig, EditAnywhere, Category = "Mana", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni"
		)
	)
	bool bUseManaStartupMovies;

	/** If enabled, The game waits for startup movies to complete even if loading has finished. */
	UPROPERTY(globalconfig, EditAnywhere, Category = "Mana", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni"
		)
	)
	bool bWaitForMoviesToComplete;

	/** If enabled, Startup movies can be skipped by the user when a mouse button is pressed. */
	UPROPERTY(globalconfig, EditAnywhere, Category = "Mana", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni"
		)
	)
	bool bMoviesAreSkippable;

	/** Movies to play on startup. Note that these must be in your game's Game/Content/Movies directory. */
	UPROPERTY(globalconfig, EditAnywhere, Category = "Mana", AdvancedDisplay,
		meta = (
			editcondition = "bNoExistCriWareIni"
		)
	)
	TArray<FString> StartupMovies;
	/* </cri_delete_if_LE> */

public:
#if WITH_EDITOR
	/* Try to load any configuration possible, convert then if needed or update from source control if new version available. */
	void InitializeSettings();

	/* CriWare.iniが存在するか確認する関数 */
	bool IsExistingCriWareIniFile() const;

	/* ConfigファイルをCriWare.iniからEngine.iniにコンバートするための関数 */
	bool ConvertConfigrationFile();

	bool IsMixedConfigFiles() const;

	/* コンバート時に削除するCriWare.iniのファイルの名前を取得する関数 */
	TArray<FString> GetDeletedFilesName() const;

	/* UE4がProjectSettingでの変数の変更をチェックする関数 */
	bool HasPendingChanges() const;

	/** Returns an event delegate that is executed when a setting has changed. */
	DECLARE_EVENT(UCriWarePluginSetting, FSettingChangedEvent);
	FSettingChangedEvent& OnSettingChanged() { return SettingChangedEvent; }

	/* SourceControl接続時ファイルをチェックアウトするための関数 */
	/* filenameにパスを含むファイル名を入力することでそのファイルをチェックアウトすることができる */
	void CheckoutConfigFile(FString fileName);

	/* 各ヒエラルキーに該当するCriWare.iniファイルパス格納用アレイ */
	TArray<FString> CriWareIniFilePath;
#endif

	/* 各ヒエラルキーに該当するEngine.iniファイルパス格納用アレイ */
	TArray<FString> EngineIniFilePath;

protected:
#if WITH_EDITOR
	/** Called when a property on this object is changed */
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
#if WITH_EDITOR
	bool CreateEngineIniFile(FString EngineIniFileName, FString CriWareIniFileName);
#endif

	void UpdateUProperty(int isConvert);

	/* GConfig->GetSectionでとりだしたセクション情報が"ContentDir=AudioData"というようにセクション名とパラメタ情報がセットとなっている */
	/* 取得した情報の"="以下を取得するための関数 */
	FString GetParameterString(const TArray<FString>& str, FString SelectorName);

	/* Same as GetParameterString() but for arrays. */
	TArray<FString> GetParemeterArray(const TArray<FString>& str, FString SelectorName);
	TArray<FAtomAsrRackConfig> GetParemeterArrayAsr(const TArray<FString>& str, FString SelectorName);

	/* UProperty変数にConfigファイルから取得したパラメータを代入する関数 */
	void InitializeCriWarePlugins(const TArray<FString>& Section_UClass);

	/* Iniファイルに記載されるパラメータが空の場合にデフォルト値を設定するための関数 */
	FString CheckIniParamString(FString ini_param_string, int Ddefault_value);
	FString CheckIniParamString(FString ini_param_string, float Ddefault_value);
	FString CheckIniParamString(FString ini_param_string, FString default_value);

#if WITH_EDITOR
	/* 旧フォーマットのデータを新フォーマットのデータに上書きするための関数 */
	void UpdateCriData(TArray<FString> Section_Old, TArray<FString>* Section_New, FString CategoryType);
	void ConvertCriData(TArray<FString> Section_Old, TArray<FString>* Section_New, FString CategoryType);
#endif

#if WITH_EDITOR
	/** Holds an event delegate that is executed when a setting has changed. */
	FSettingChangedEvent SettingChangedEvent;
#endif
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
