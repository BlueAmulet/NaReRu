/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2014-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Spectrum Analyzer
 * File     : AtomSpectrumAnalyzer.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
 /* Unreal Engine 4関連ヘッダ */
#include "Kismet/BlueprintFunctionLibrary.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareApi.h"
#include "AtomAsrRack.h"
#include "SoundAtomCue.h"
#include "AtomComponent.h"
#include "SoundAtomConfig.h"

/* モジュールヘッダ */
#include "AtomSpectrumAnalyzer.generated.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

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
 UCLASS(meta=(ToolTip = "AtomComponent class."))
class CRIWARERUNTIME_API UAtomSpectrumAnalyzer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UAtomSpectrumAnalyzer();

	/* DSPSpectra作成 */
	UFUNCTION(BlueprintCallable, Category="Atom", meta=(UnsafeDuringActorConstruction = "true", ToolTip = "Create Dsp Spectra."))
	static void CreateDspSpectra(UAtomAsrRack* asr_rack, FString bus_name, int32 num_bands);

	/* スペクトル解析結果の出力 */
	UFUNCTION(BlueprintCallable, Category="Atom", meta=(UnsafeDuringActorConstruction = "true", ToolTip = "Get spectrum information."))
	static void GetLevels(TArray<float>& spectra);
	
	/* スペクトル解析結果の出力（デシベル値変換） */
	/* display_range:表示範囲(例：display_range=96の場合は-96dB以上の値を格納) */
	UFUNCTION(BlueprintCallable, Category="Atom", meta=(UnsafeDuringActorConstruction = "true", ToolTip = "Get spectrum information with converting to decibel value."))
	static void GetLevelsDB(float display_range, TArray<float>& spectra);

private:
	static int32 NumBands;
};
 /***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/