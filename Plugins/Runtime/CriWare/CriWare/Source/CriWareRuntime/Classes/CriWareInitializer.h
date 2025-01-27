﻿/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Initializer
 * File     : CriWareInitializer.h
 *
 ****************************************************************************/

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

/* CRIWARE Plugin Headers */
#include "CriWareApi.h"
#include "AtomComponent.h"
#include "AtomSoundConcurrency.h"
#include "CriWarePlatform.h"

/* モジュールヘッダ */
#include "CriWareInitializer.generated.h"

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/
#define CRIWARE_UE4_COMBINE_STRINGS_ACTUALLY(a, b) a ## b
#define CRIWARE_UE4_COMBINE_STRINGS(a, b) CRIWARE_UE4_COMBINE_STRINGS_ACTUALLY(a, b)
#define CRIWARE_UE4_CREATE_STRING(str) #str
#define CRIWARE_UE4_CREATE_VERSION_STRING(a, b, c, d) CRIWARE_UE4_CREATE_STRING(a.b.c.d)

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
/* バージョン情報 */
/* Version informaiton */
#define CRIWARE_PLUGIN_MAJOR_VERSION		1
#define CRIWARE_PLUGIN_CORERUNTIME_VERSION	28
#define CRIWARE_PLUGIN_FEATURE_VERSION		0
#define CRIWARE_PLUGIN_PATCH_VERSION		2

#if CRIWARE_PLUGIN_CORERUNTIME_VERSION >= 10
#    define CRIWARE_PLUGIN_CORERUNTIME_VERSION_STR CRIWARE_PLUGIN_CORERUNTIME_VERSION
#else
#    define CRIWARE_PLUGIN_CORERUNTIME_VERSION_STR CRIWARE_UE4_COMBINE_STRINGS(0, CRIWARE_PLUGIN_CORERUNTIME_VERSION)
#endif

#if CRIWARE_PLUGIN_FEATURE_VERSION >= 10
#    define CRIWARE_PLUGIN_FEATURE_VERSION_STR CRIWARE_PLUGIN_FEATURE_VERSION
#else
#    define CRIWARE_PLUGIN_FEATURE_VERSION_STR CRIWARE_UE4_COMBINE_STRINGS(0, CRIWARE_PLUGIN_FEATURE_VERSION)
#endif

#if CRIWARE_PLUGIN_PATCH_VERSION >= 10
#    define CRIWARE_PLUGIN_PATCH_VERSION_STR CRIWARE_PLUGIN_PATCH_VERSION
#else
#    define CRIWARE_PLUGIN_PATCH_VERSION_STR CRIWARE_UE4_COMBINE_STRINGS(0, CRIWARE_PLUGIN_PATCH_VERSION)
#endif

#define CRIWARE_UE4_PLUGIN_VERSION CRIWARE_UE4_CREATE_VERSION_STRING(CRIWARE_PLUGIN_MAJOR_VERSION, CRIWARE_PLUGIN_CORERUNTIME_VERSION_STR, CRIWARE_PLUGIN_FEATURE_VERSION_STR, CRIWARE_PLUGIN_PATCH_VERSION_STR)

#ifndef UE_DEPRECATED
#define UE_DEPRECATED DEPRECATED
#endif

#define FS_NUM_BINDERS                                 CRIFS_CONFIG_DEFAULT_NUM_BINDERS
#define FS_MAX_BINDS                                   CRIFS_CONFIG_DEFAULT_MAX_BINDS
#define FS_NUM_LOADERS                                 CRIFS_CONFIG_DEFAULT_NUM_LOADERS
#define FS_MAX_PATH                                    1024
#define FS_OUTPUT_LOG                                  false
#define ATOM_AUTOMATICALLY_CREATE_CUE_ASSET            true
#define ATOM_USES_INGAME_PREVIEW                       false
#define ATOM_OUTPUT_LOG                                false
#define ATOM_MONITOR_COMMUNICATION_BUFFER              2 * 1024 * 1024
#define ATOM_MAX_VIRTUAL_VOICES                        32
#define ATOM_NUM_STANDARD_MEMORY_VOICES                16
#define ATOM_STANDARD_MEMORY_VOICE_NUM_CHANNELS        CRIATOM_DEFAULT_INPUT_MAX_CHANNELS
#define ATOM_STANDARD_MEMORY_VOICE_SAMPLING_RATE       CRIATOM_DEFAULT_INPUT_MAX_SAMPLING_RATE
#define ATOM_NUM_STANDARD_STREAMING_VOICES             8
#define ATOM_STANDARD_STREAMING_VOICE_NUM_CHANNELS     CRIATOM_DEFAULT_INPUT_MAX_CHANNELS
#define ATOM_STANDARD_STREAMING_VOICE_SAMPLING_RATE    CRIATOM_DEFAULT_INPUT_MAX_SAMPLING_RATE
#define ATOM_DISTANCE_FACTOR                           0.01f
#define CRIATOM_DEFAULT_ECONOMICTICK_MARGIN_DISTANCE (1000000.0f)
#define CRIATOM_DEFAULT_ECONOMICTICK_FREQUENCY (15)
#define CRIATOM_DEFAULT_CULLING_MARGIN_DISTANCE (1000000.0f)
#define ATOM_USE_UNREAL_SOUND_RENDERER                 false

#if !defined(CRIWARE_UE4_LE)	/* <cri_delete_if_LE> */
#define B_INITIALIZE_MANA                              true
#define MANA_ENABLE_DECODE_SKIP                        false
#define MANA_MAX_DECODER_HANDLES                       4
#define MANA_MAX_MANABPS                               0
#define MANA_MAX_MANA_STREAMS                          1
#define B_MANA_USE_H264_DECODER                        false
#define FS_FILE_ACCESS_THREAD_PRIORITY_PS4             500
#define FS_DATA_DECOMPRESSION_THREAD_PRIORITY_PS4      730
#define FS_MEMORY_FILE_SYSTEM_THREAD_PRIORITY_PS4      720
#define ATOM_SERVER_THREAD_PRIORITY_PS4                400
#define ATOM_OUTPUT_THREAD_PRIORITY_PS4                300

#define FS_FILE_ACCESS_THREAD_AFFINITY_MASK_PS4        63
#define FS_DATA_DECOMPRESSION_THREAD_AFFINITY_MASK_PS4 63
#define FS_MEMORY_FILE_SYSTEM_THREAD_AFFINITY_MASK_PS4 63
#define ATOM_SERVER_THREAD_AFFINITY_MASK_PS4           63
#define ATOM_OUTPUT_THREAD_AFFINITY_MASK_PS4           63

#define B_INITIALIZE_ADXLIPSYNC                        true
#define ADXLIPSYNC_MAX_ANALYZER_HANDLES                64
#endif	/* </cri_delete_if_LE> */

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/
/* UE4プロファイラ用 */
#if STATS
DECLARE_STATS_GROUP(TEXT("CRIWARE"), STATGROUP_CriWare, STATCAT_Advanced);
DECLARE_MEMORY_STAT_EXTERN(TEXT("CriWare Work Size"), STAT_CriWare_WorkSize, STATGROUP_CriWare, );
#endif

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/
UCLASS(meta=(ToolTip="CriWareInitializer class."))
class CRIWARERUNTIME_API UCriWareInitializer : public UObject
{
	GENERATED_BODY()

public:
	UCriWareInitializer(const FObjectInitializer& ObjectInitializer);

	/* コンテンツパスの取得 */
	static FString GetContentDir();

	/* 絶対パスの取得 */
	static FString ConvertToAbsolutePathForExternalAppForRead(const TCHAR* Filename);

	/* リスナの取得 */
	static CriAtomEx3dListenerHn GetListener(int32 PlayerIndex = 0);

	/* 距離係数の取得 */
	static float GetDistanceFactor();

	/* 出力サンプリングレートの取得 */
	static int32 GetOutputSamplingRate();

	/* メモリ再生ボイスプールの取得 */
	static CriAtomExVoicePoolHn GetMemoryVoicePool();

	/* ストリーム再生ボイスプールの取得 */
	static CriAtomExVoicePoolHn GetStreamingVoicePool();

	/* ACFアセットパスの取得 */
	static FSoftObjectPath GetAtomConfigAssetReference();

	/* リスナの有効化／無効化 */
	static void SetListenerAutoUpdateEnabled(bool bEnabled, int32 PlayerIndex = 0);

	/* リスナ位置の指定 */
	static void SetListenerLocation(FVector Location, int32 PlayerIndex = 0);

	/* リスナの向きの指定 */
	static void SetListenerRotation(FRotator Rotation, int32 PlayerIndex = 0);

	/* リスナ位置の取得 */
	static FVector GetListenerLocation(int32 PlayerIndex = 0);

	/* リスニングポイントの取得 */
	static FVector GetListeningPoint(int32 PlayerIndex = 0);

	/* ACFがロード済みかどうか */
	static bool IsAcfLoaded();

	/* モニタライブラリの処理を無効化 */
	static void DisableMonitor();

	/* AtomComponentPoolを利用する場合のPoolするAtomCompoenntの数を返します。   */
	/* AtomComponentPoolが無効になっている場合は0が返ります。 */
	static int32 GetPooledAtomComponentNum();

	static float GetEconomicTickDistanceMargin();
	static int32 GetEconomicTickFrequency();
	static float GetCullDistanceMargin();

	/** Access to platform specific functions. */
	static FCriWarePlatform Platform;

	// Begin UObject interface.
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	// End  UObject interface

#if defined(CRIWARE_UE4_LE)
	static void* CriWareDllHandle;
	static uint32 NumInstances;
#endif

	static FSoftObjectPath AcfAssetReference;
	static FSoftObjectPath AcfDataTableAssetReference;
	static UDataTable* AcfDataTableObject;
	static uint8* AcfData;
	static CriAtomDspSpectraHn dsp_hn;
	static TArray<FString> bus_name;
	static TArray<int32> asr_rack_id;

	static class FAtomSoundConcurrencyManager* ConcurrencyManager;

	/* 生成されたAtomAudioVolumeの管理用アレイ(Snapshot用) */
	static TArray<TWeakObjectPtr<class AAtomAudioVolume>> AudioVolumeArray_Snapshot;
	static TArray<TWeakObjectPtr<class AAtomAudioVolume>> AudioVolumeArray_Bus;
	static TArray<TWeakObjectPtr<class AAtomAudioVolume>> AudioVolumeArray_Aisac;
	static bool bIsInitialised_AudioVolume;
	
	UE_DEPRECATED(4.21, "Please use GetEconomicTickDistanceMargin() instead.")
	static float GetEconomicTickMarginDistance();

	UE_DEPRECATED(4.21, "Please use GetCullDistanceMargin() instead.")
	static float GetCullingMarginDistance();

#if WITH_EDITOR
	static FAtomListener* AtomListenerForPreviewEditor;
#endif

};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/* --- end of file --- */
