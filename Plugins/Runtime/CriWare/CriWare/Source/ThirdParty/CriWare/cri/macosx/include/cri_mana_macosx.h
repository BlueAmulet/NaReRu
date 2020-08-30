/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2010-2015 CRI Middleware Co., Ltd.
 *
 * Library  : CRI Mana (Sofdec2)
 * Module   : Library User's Header for MACOSX
 * File     : cri_mana_macosx.h
 * Date     : 2020-03-25
 *
 ****************************************************************************/
/*!
 *	\file		cri_mana_macosx.h
 */
 
/* 多重定義防止 */
/* Prevention of redefinition */
#ifndef CRI_INCL_CRI_MANA_MACOSX_H
#define CRI_INCL_CRI_MANA_MACOSX_H

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include <cri_xpt.h>
#include <cri_mana.h>

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/
/* 互換性維持用置換マクロ */
/* 将来のリリースでこれらのマクロは削除される予定です。*/
#define criMana_SetDecodeThreadPriority_MACOSX     criMana_SetDecodeThreadPriority_MACOSX

/*JP
 * \brief H.264ムービー用デコーダコンフィグ構造体にデフォルト値をセット
 * \ingroup MDL_MANALIB_GLOBAL_MACOSX
 * \param[in] p_config デコード用のプロセッサコンフィグ構造体へのポインタ
 * \par 説明:
 * ::criMana_SetupH264Decoder_MACOSX 関数に設定するプロセッサコンフィグ構造体( ::CriManaH264DecoderConfig_MACOSX ) に
 * デフォルトの値をセットします。<br>
 * \sa
 * CriManaH264DecoderConfig_MACOSX
 */
#define criMana_SetDefaultVideoToolboxH264DecoderConfig_MACOSX(p_config)    {\
    (p_config)->render_context = NULL;\
    (p_config)->decoder_output = CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_IMAGE_BUFFER;\
}

/***************************************************************************
 *      列挙子宣言
 *      Enum Declarations
 ***************************************************************************/

typedef enum {
    /*JP \brief Decode to textures and return pointers to host(CPU) texture data arrays. (defaults to 422 Bi-planar) */
    CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_IMAGE_BUFFER = 0,
    /*JP \brief Decode to CVOpenGLTextureRef for optimized usage with OpenGL. (Native with OpenGL) (DEPRECATED in macOS 10.14)*/
    CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_OPENGL_TEXTURES,
    /*JP \brief Decode to CVMetalTextureRef for optimized usage with Metal. (Native with Metal) */
    CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_METAL_TEXTURES,
    /*JP \brief Decode to 1 CVOpenGlESTextureRef (RGBA texture) for OpenGL. (DEPRECATED in macOS 10.14)*/
    CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_OPENGL_ARGB_TEXTURE,
    /*JP \brief Decode to 1 CVMetalTextureRef (RGBA texture) for Metal. */
    CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_METAL_ARGB_TEXTURE,
    /*JP \brief Decode to 2 CVMetalTextureRef (Y and C textures) for Metal. (Decoder Native) */
    CRIMANAVIDEOTOOLBOXH264MACOSX_DECODE_TO_METAL_NV12_TEXTURES
} CriManaVideoToolboxH264DecoderOutput_MACOSX;

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
/*JP
 * \brief H.264デコーダー用コンフィグ構造体
 * \ingroup MDL_MANALIB_GLOBAL_MACOSX
 * \par 説明:
 * H.264ムービー用デコードモジュールを制御するための設定構造体です。
 *
 * \sa criMana_CalculateH264DecoderWorkSize_MACOSX, criMana_SetupH264Decoder_MACOSX
 */
typedef struct {
    /*JP \brief Graphics context (GL context or Metal device or NULL if not used) */
    void*                                       render_context;
    /*JP \brief Output memory target of decoded frames */
    CriManaVideoToolboxH264DecoderOutput_MACOSX decoder_output;
} CriManaVideoToolboxH264DecoderConfig_MACOSX;

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
/*JP
 * \brief デコードマスタースレッドのプライオリティ変更
 * \ingroup MDL_MANALIB_GLOBAL_MACOSX
 * \param[in]	prio	スレッドプライオリティ
 * \par 呼び出し条件:
 * マルチスレッドモデルでManaライブラリの初期化した後に本関数を実行してください。
 * \par 説明:
 * デコード処理を行うスレッドのプライオリティを変更します。<br>
 * デフォルト状態（本関数を実行しない場合）では、デコード処理のスレッドプライオリティは、
 * 各機種の標準的なメインスレッドプライオリティよりも低くなるように設定されています。
 * \attention
 * 本関数は、ライブラリ初期化時にスレッドモデルをマルチスレッドモデル
 * （ ::CRIMANA_THREAD_MODEL_MULTI ）に設定した場合にのみ効果を発揮します。<br>
 * シングルスレッドモデルでライブラリを初期化した状態、もしくはライブラリ初期化前に
 * 本関数を呼び出した場合は何も処理を行いません。
 * \sa criMana_Initialize
 */
void CRIAPI criMana_SetDecodeThreadPriority_MACOSX(int prio);

/*JP
 * \brief フレーム変換処理の初期化 (16bit RGB565形式)
 * \ingroup MDL_MANALIB_GLOBAL
 * \par 呼び出し条件:
 * CRI Manaライブラリの初期化後、::criManaPlayer_CopyFrameToBufferRGB565関数の呼び出し前に本関数を実行してください。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 16bit RGB565フォーマットで、フレームを取得するための初期化関数です。<br>
 * 上記フォーマットでフレームを取得する場合は必ず本関数を実行してください。<br>
 */
void CRIAPI criMana_InitializeFrameRGB565(void);
	
/*==========================================================================
 *      CRI Mana Player API
 *=========================================================================*/
/*JP
 * \brief デコード結果の取得 (16bit RGB565形式)
 * \ingroup MDL_MANALIB_PLAYER
 * \param[in]	player		Manaプレーヤハンドル
 * \param[out]	frame_info	フレーム情報構造体
 * \param[out]	frame_buf	コピー先のフレームバッファ（描画対象）
 * \par 呼び出し条件:
 * Manaプレーヤーのハンドル状態が CRIMANAPLAYER_STATUS_PLAYING の時に本関数を呼び出してください。<br>
 * また、本関数の実行には ::criMana_InitializeFrameRGB565 関数を必ず事前に呼び出してください。
 * <br>
 * スレッドセーフではありません。
 * \par 説明:
 * 16bit RGB565フォーマットで、表示時刻になっているビデオフレームを取得(コピー)します。<br>
 * この関数を呼び出す場合は、RGBバッファーの実体を確保したうえで呼び出す必要があります。<br>
 * <br>
 * 16bit RGB565 の実際のピクセルデータの並びについては、そのプラットフォームで最も標準的な
 * フォーマットになります。
 */
void CRIAPI criManaPlayer_CopyFrameToBufferRGB565(
	CriManaPlayerHn player, const CriManaFrameInfo *frame_info, CriManaTextureBuffer *frame_buf);

/*==========================================================================
 *      CRI VideoToolbox API
 *=========================================================================*/
/*JP
 * \brief H.264デコーダー初期化用ワークサイズ計算関数
 * \ingroup MDL_MANALIB_H264_MACOSX
 * \param[in]	config	 H.264デコーダー初期化用コンフィグ構造体
 * \return		CriSint32 H.264デコーダー初期化用ワークサイズ
 * \par 呼び出し条件:
 * スレッドセーフです。
 * \par 説明:
 * H.264ムービー再生用のデコードモジュールを初期化するためのワーク領域のサイズを取得します。
 * 必要とするワーク領域のサイズは、初期化コンフィグ構造体の内容によって変化します。<br>
 * 引数 config の情報は、関数内でのみ参照されます。関数実行後に config の領域を解放
 * しても問題ありません。<br>
 * <br>
 * ワークサイズとして 0 が返ってきた場合、デコーダワークメモリは不要です。
 *
 * \sa CriManaVideoToolboxH264DecoderConfig_MACOSX, criMana_SetupVideoToolboxH264Decoder_MACOSX
 */
CriSint32 CRIAPI criMana_CalculateVideoToolboxH264DecoderWorkSize_MACOSX(const CriManaVideoToolboxH264DecoderConfig_MACOSX *config);

/*JP
 * \brief H.264デコーダー初期化パラメータのセットアップ関数
 * \ingroup MDL_MANALIB_H264_MACOSX
 * \param[in]	config		H.264デコーダー初期化用コンフィグ構造体
 * \param[in]	work		アプリ側で確保済みのワーク領域のアドレス (内部でアライン調整する)
 * \param[in]	work_size	ワークサイズ
 *
 * \par 呼び出し条件:
 * スレッドセーフではありません。<br>
 * ::criMana_Initialize関数よりも前に呼び出してください。<br>
 * アプリ側で確保済みのワーク領域、ワークサイズ（work、work_size）への指定には未対応で、<br>
 * 関数内部ではこれらの引数は使用しておりません。それぞれNULL、0を指定してご使用下さい。<br>
 * \par 説明:
 * H.264ムービー再生を行うための、H.264デコードライブラリの初期化パラメータを設定します。<br>
 * <br>
 * 実際のH.264デコーダライブラリの初期化は::criMana_Initialize関数の呼び出し時になります。
 * <br>
 * 引数 config の情報は、関数内でのみ参照されます。関数実行後に config の領域を解放
 * しても問題ありません。
 * <br>
 * \attention
 * この関数を呼び出すとH.264デコーダーへの関数テーブルがリンクされます。<br>
 * 本関数を使用してH.264デコードを有効にした場合は以下のフレームワークをリンクしてください。<br>
 * - <b>VideoToolbox.Framework</b>
 * - <b>CoreVideo.Framework</b>
 * - <b>CoreMedia.Framework</b>
 * \par
 * H.264ムービー再生の動作要件は、<b>iOS8以降</b> です。
 *
 * \sa CriManaVideoToolboxH264DecoderConfig_MACOSX, criMana_CalculateVideoToolboxH264DecoderWorkSize_MACOSX, criMana_Initialize
 */
void CRIAPI criMana_SetupVideoToolboxH264Decoder_MACOSX(const CriManaVideoToolboxH264DecoderConfig_MACOSX* config, void *work, CriSint32 work_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //CRI_INCL_CRI_MANA_MACOSX_H

/* end of file */
