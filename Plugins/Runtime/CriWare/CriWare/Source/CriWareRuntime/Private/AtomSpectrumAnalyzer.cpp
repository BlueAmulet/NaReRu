/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Spectrum Analyzer
 * File     : AtomSpectrumAnalyzer.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
 /* モジュールヘッダ */
#include "AtomSpectrumAnalyzer.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "CriWareInitializer.h"

/* Unreal Engine 4関連ヘッダ */
#include "UObject/Package.h"
#include "GameFramework/WorldSettings.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "AtomSpectrumAnalyzer"

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

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

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
static void app_filter_callback(void *obj, CriAtomPcmFormat format,
	CriSint32 num_channels, CriSint32 num_samples, void *data[]);
	
/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/
int32 UAtomSpectrumAnalyzer::NumBands = 0;

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
UAtomSpectrumAnalyzer::UAtomSpectrumAnalyzer()
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif
}

void UAtomSpectrumAnalyzer::CreateDspSpectra(UAtomAsrRack* asr_rack, FString bus_name, int32 num_bands)
{
	CriAtomDspSpectraConfig spectra_config;
	
	if (UCriWareInitializer::dsp_hn != nullptr) {
		/* スペクトラムアナライザの破棄 */
		/* Destroy spectrum analyzer */
		criAtomDspSpectra_Destroy(UCriWareInitializer::dsp_hn);
		UCriWareInitializer::dsp_hn = nullptr;
	}

	/* スペクトラムアナライザの作成 */
	/* Create a spectrum analyzer */
	spectra_config.num_bands = num_bands;
	NumBands = num_bands;
	UCriWareInitializer::dsp_hn = criAtomDspSpectra_Create(&spectra_config, NULL, 0);

	/* バスフィルタコールバックの登録 */
	/* Register a bus filter callback */
	/* [Note] In the following example, a signal that passes through bus7 is analyzed. */
	int32 bus_name_array_id = UCriWareInitializer::bus_name.Find(bus_name);
	if (bus_name_array_id == INDEX_NONE) {
		bus_name_array_id = UCriWareInitializer::bus_name.Add(bus_name);
	}

	criAtomExAsrRack_SetBusFilterCallbackByName(asr_rack->RackId, TCHAR_TO_UTF8(*UCriWareInitializer::bus_name[bus_name_array_id]), app_filter_callback, NULL, UCriWareInitializer::dsp_hn);
	UCriWareInitializer::asr_rack_id.Add(asr_rack->RackId);
}

void UAtomSpectrumAnalyzer::GetLevels(TArray<float>& spectra)
{
	const CriFloat32* spectra_ptr = criAtomDspSpectra_GetLevels(UCriWareInitializer::dsp_hn);
	for(int iter = 0;iter < NumBands; iter++){
		spectra.Add(spectra_ptr[iter]);
	}
}

void UAtomSpectrumAnalyzer::GetLevelsDB(float display_range, TArray<float>& spectra)
{
	const CriFloat32* spectra_ptr = criAtomDspSpectra_GetLevels(UCriWareInitializer::dsp_hn);
	CriFloat32 decibel = 0.0f;

	for(int iter = 0; iter < NumBands; iter++){
		/* 値をデシベル値に変換 */
		/* Conversion to decibel value */
		
		/* 表示範囲の選択 */
		/* 下記の例では-96dB以上の値を表示 */
		/* Select the display range */
		/* The following example shows values larger than -96dB. */
		
		decibel = 20.0f * log10f(spectra_ptr[iter]);
		spectra.Add(decibel + display_range);
	}
}

/* フィルターコールバック */
/* Filter callback function */
static void app_filter_callback(void *obj, CriAtomPcmFormat format,
	CriSint32 num_channels, CriSint32 num_samples, void *data[])
{
	CriAtomDspSpectraHn dspHn = (CriAtomDspSpectraHn)obj;

	if (dspHn == nullptr) {
		return;
	}

	/* 音声再生時以外は何もしない */
	/* Do nothing if it's not sound playback. */
	if ((num_channels <= 0) || (num_samples <= 0)) {
		criAtomDspSpectra_Reset(dspHn);
		return;
	}

	/* 【注意】本サンプルは32bit float PCMデータ形式の環境専用です。	*/
	/* 　　　　（16bit int PCMデータ形式の環境では動作しません。）		*/
	/* [Note] This sample is exclusively used for the environment of 32-bit float PCM data. */
	/*        It does not work on the environment of 16-bit int PCM data.                   */
	if (format != CRIATOM_PCM_FORMAT_FLOAT32) {
		criErr_NotifyError("E2014031799", "This sample is available only on float PCM platform.");
		return;
	}

	/* スペクトルの解析 */
	/* Perform spectrum analysis */
	criAtomDspSpectra_Process(dspHn, num_channels, num_samples, (CriFloat32 **)data);
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
