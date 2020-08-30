/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRI Mana (Sofdec2)
 * Module   : Library User's Header for PC
 * File     : cri_mana_pc.h
 * Date     : 2018-07-23
 *
 ****************************************************************************/
/*!
 *	\file		cri_mana_pc.h
 */
 
/* 多重定義防止 */
/* Prevention of redefinition */
#ifndef CRI_INCL_CRI_MANA_PC_H
#define CRI_INCL_CRI_MANA_PC_H

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include <cri_xpt.h>
#include <cri_mana.h>

#include <windows.h>

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/
/* criMana_SetDefaultProcessorConfig_PC */
/*JP
 * \brief デコード用プロセッサコンフィグ構造体にデフォルト値をセット
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \param[out] p_config
 * \par 説明:
 * ::criMana_Initialize_PC 関数に設定するプロセッサコンフィグ構造体( ::CriManaProcessorConfig_PC ) に
 * デフォルトの値をセットします。<br>
 * \sa
 * CriManaProcessorConfig_PC
 */
#define criMana_SetDefaultProcessorConfig_PC(p_config) \
{\
	(p_config)->num_threads = 0;	\
	(p_config)->affinity_masks = NULL;	\
	(p_config)->priority = -1;	\
}

/* criMana_SetDefaultLibConfig_PC */
/*JP
 * \brief ライブラリ初期化用コンフィグ構造体にデフォルト値をセット
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \param[out] p_config	初期化用コンフィグ構造体へのポインタ
 * \par 説明:
 * ::criMana_Initialize_PC 関数に設定するコンフィグ構造体( ::CriManaLibConfig_PC ) に
 * デフォルトの値をセットします。<br>
 * \sa
 * CriManaLibConfig_PC
 */
#define criMana_SetDefaultLibConfig_PC(p_config)	\
{\
	criMana_SetDefaultLibConfig(&(p_config)->mana);	\
	criMana_SetDefaultProcessorConfig_PC(&(p_config)->processor);	\
}

/* criMana_SetDefaultMediaFoundationH264DecoderConfig_PC */
/*JP
 * \brief MediaFoundation H.264ムービー用デコーダコンフィグ構造体にデフォルト値をセット
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \param[in] p_config MediaFoundation H.264ムービー用デコーダコンフィグ構造体へのポインタ
 * \par 説明:
 * ::criMana_SetupMediaFoundationH264Decoder_PC 関数に設定するデコーダコンフィグ構造体( ::CriManaMediaFoundationH264DecoderConfig_PC ) に
 * デフォルトの値をセットします。<br>
 * \sa
 * CriManaMediaFoundationH264DecoderConfig_PC
 */
#define criMana_SetDefaultMediaFoundationH264DecoderConfig_PC(p_config)

/***************************************************************************
 *      列挙子宣言
 *      Enum Declarations
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
/* CriManaProcessorConfig_PC */
/*JP
 * \brief デコード用プロセッサコンフィグ構造体
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \par 説明:
 * PC固有のデコードに使用するプロセッサーを指定します。<br>
 * <br>
 * デコード処理に行うプロセッサーやスレッド優先度を変更したい場合に使用してください。<br>
 * <br>
 * Manaライブラリは初期化の際に３つのマルチコア分散デコード用のワーカースレッドを用意します。<br>
 * num_threads引数で、そのうちのいくつのスレッドを実際に使用するかを指定できます。
 * アプリケーションから明示的にプロセッサー割り当てを行いたい場合、個々のスレッドに対して
 * アフィニティマスクを設定してください。<br>
 * アフィニティマスクの値は、Win32 APIのSetThreadAffinityMaskの引数と同じ書式です。<br>
 * スレッド優先度は、num_threadsで指定したデコードに使用するスレッドに対して適用されます。
 *
 * \sa CriManaLibConfig_PC, criMana_Initialize_PC
 */
typedef struct {
	/* CriManaProcessorConfig_PC.num_threads */
	/*JP< デコードに使用する追加スレッドの数 (最大3つ) */
	CriSint32	num_threads;

	/* CriManaProcessorConfig_PC.affinity_masks */
	/*JP< スレッドアフィニティマスクの配列へのポインタ。num_threadsで指定したスレッド分のマスク値 */
	const DWORD*	affinity_masks;

	/* CriManaProcessorConfig_PC.priority */
	/*JP< 追加スレッド全体の優先度 */
	int	priority;
} CriManaProcessorConfig_PC;

/* CriManaLibConfig_PC */
/*JP
 * \brief Manaライブラリ初期化パラメータ
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \par 説明:
 * PC固有のCRI Manaライブラリの動作仕様を指定するための構造体です。
 * \sa criMana_Initialize_PC
 */
typedef struct {
	/* CriManaLibConfig_PC.mana */
	/*JP< 機種共通初期化ライブラリコンフィグ構造体 */
	CriManaLibConfig mana;

	/* CriManaLibConfig_PC.processor */
	/*JP< マルチコアデコード用プロセッサコンフィグ構造体 */
	CriManaProcessorConfig_PC processor;
} CriManaLibConfig_PC;

/* CriManaMediaFoundationH264DecoderConfig_PC */
/*JP
 * \brief H.264デコーダー用コンフィグ構造体
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \par 説明:
 * H.264ムービー用デコードモジュールを制御するための設定構造体です。
 *
 * \sa criMana_SetDefaultMediaFoundationH264DecoderConfig_PC, criMana_SetupMediaFoundationH264Decoder_PC,
 */
typedef struct {
	/*JP< 未使用 */
	CriUint8 dummy;
} CriManaMediaFoundationH264DecoderConfig_PC;

/***************************************************************************
 *      コールバック宣言
 *      Prototype Callback
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*==========================================================================
 *      CRI Mana API
 *=========================================================================*/
/* criMana_CalculateLibWorkSize_PC */
/*JP
 * \brief ライブラリ初期化用ワークサイズ計算関数 (PC固有機能つき)
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \param[in]	config	初期化用コンフィグ構造体
 * \return		CriSint32 ライブラリワーク領域サイズ
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * PC固有機能を有効にしたManaライブラリを使用するために必要なワーク領域のサイズを取得します。<br>
 * ライブラリが必要とするワーク領域のサイズは、ライブラリの初期化コンフィグ構造体の
 * 内容によって変化します。<br>
 * 引数 config の情報は、関数内でのみ参照されます。関数実行後に config の領域を解放
 * しても問題ありません。
 *
 */
CriSint32 CRIAPI criMana_CalculateLibWorkSize_PC(const CriManaLibConfig_PC *config);

/* criMana_Initialize_PC */
/*JP
 * \brief ライブラリ初期化関数 (PC固有機能つき)
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \param[in]	config	PC固有パラメータつき初期化用コンフィグ構造体
 * \param[in]	work	アプリ側で確保済みのワーク領域のアドレス (内部でアライン調整する)
 * \param[in]	work_size	ワーク領域のサイズ (=criMana_CalculateLibWorkSizeの戻り値)
 * \par 呼び出し条件::
 * CriAtomExライブラリの初期化後、かつ ::criManaPlayer_Create 関数の前に呼んでください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * Manaライブラリの初期化をします。<br>
 * PC固有でマルチコアを使ったデコード制御を行ないたい場合、::criMana_Initialize 関数の変わりに
 * 本関数を実行してください。渡されたコンフィグに従ってマルチコアの設定を行ったあと、内部で
 * ::criMana_Initialize 関数を呼び出します。
 * <br>
 * ライブラリを初期化する際には、ライブラリが内部で利用するためのメモリ領域（ワーク領域）
 * を確保する必要があります。<br>
 * <br>
 * workにNULL、work_sizeに0を指定した場合は、::criMana_SetUserAllocator 関数で登録された
 * メモリ確保関数を使用して内部でメモリ確保を行います。
 * <br>
 * 引数 config の情報は、関数内でのみ参照されます。<br>
 * 関数を抜けた後は参照されませんので、関数実行後に config の領域を解放しても
 * 問題ありません。
 *
 * \attention
 * 本関数を実行後、必ず対になる ::criMana_Finalize_PC 関数を実行してください。<br>
 * また、 ::criMana_Finalize_PC 関数を実行するまでは、本関数を再度実行しないでください。<br> 
 *
 * \sa criMana_Finalize_PC
 */
void CRIAPI criMana_Initialize_PC(const CriManaLibConfig_PC *config, void *work, CriSint32 work_size);

/* criMana_Finalize_PC */
/*JP
 * \brief ライブラリの終了処理 (PC固有機能つき)
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \par 呼び出し条件:
 * 全てのハンドルが STOP or PLAYEND 状態であること <br>
 * AtomExライブラリ終了関数の前に本関数を呼び出してください。<br>
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * Manaライブラリの終了処理を行います。<br>
 *
 * \sa criMana_Initialize_PC
 */
void CRIAPI criMana_Finalize_PC(void);

/* criMana_SetDecodeThreadPriority_PC */
/*JP
 * \brief デコードマスタースレッドのプライオリティ変更
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \param[in]	prio	スレッドプライオリティ
 * \par 呼び出し条件:
 * マルチスレッドモデルでManaライブラリの初期化した後に本関数を実行してください。
 * \par 説明:
 * デコード処理を行うスレッドのプライオリティを変更します。<br>
 * デフォルト状態（本関数を実行しない場合）では、
 * デコード処理のスレッドプライオリティは、THREAD_PRIORITY_BELOW_NORMAL に設定されています。
 * \attention
 * 本関数は、ライブラリ初期化時にスレッドモデルをマルチスレッドモデル
 * （ ::CRIMANA_THREAD_MODEL_MULTI ）に設定した場合にのみ効果を発揮します。<br>
 * シングルスレッドモデルでライブラリを初期化した状態、もしくはライブラリ初期化前に
 * 本関数を呼び出した場合は何も処理を行いません。
 * \sa criMana_Initialize, criMana_Initialize_PC
 */
void CRIAPI criMana_SetDecodeThreadPriority_PC(int prio);

/* criMana_SetupMediaFoundationH264Decoder_PC */
/*JP
 * \brief H.264デコーダー初期化パラメータのセットアップ関数
 * \ingroup MDL_MANALIB_GLOBAL_PC
 * \param[in]	config		MediaFoundation H.264デコーダー初期化用コンフィグ構造体
 * \param[in]	work		アプリ側で確保済みのワーク領域のアドレス (内部でアライン調整する)
 * \param[in]	work_size	ワークサイズ
 * \par 呼び出し条件:
 * スレッドセーフではありません。<br>
 * ::criMana_Initialize_PC関数よりも前に呼び出してください。<br>
 * 本関数に対する終了処理は不要です。<br>
 * 本関数を使用した場合、下記ライブラリファイルへのリンクが必要です。<br>
 * - Mfplat.lib<br>
 * - mfuuid.lib<br>
 * アプリ側で確保済みのワーク領域、ワークサイズ（work、work_size）への指定には未対応で、<br>
 * 関数内部ではこれらの引数は使用しておりません。それぞれNULL、0を指定してご使用下さい。<br>
 * 
 * \par 説明:
 * H.264ムービー再生を行うための、H.264デコードライブラリの初期化パラメータを設定します。<br>
 * <br>
 * 実際のH.264デコーダライブラリの初期化は::criMana_Initialize_PC関数の呼び出し時になります。
 * <br>
 * 引数 config の情報は、関数内でのみ参照されます。関数実行後に config の領域を解放
 * しても問題ありません。
 * <br>
 * \sa CriManaMediaFoundationH264DecoderConfig_PC, criMana_Initialize_PC
 */
void CRIAPI criMana_SetupMediaFoundationH264Decoder_PC(const CriManaMediaFoundationH264DecoderConfig_PC *config, void *work, CriSint32 work_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //CRI_INCL_CRI_MANA_PC_H

/* end of file */
